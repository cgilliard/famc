//! Schannel TLS streams.
use std::any::Any;
use std::cmp;
use std::error::Error;
use std::fmt;
use std::io::{self, Read, BufRead, Write, Cursor};
use std::mem;
use std::ptr;
use std::slice;
use std::sync::Arc;
use winapi::shared::minwindef as winapi;
use winapi::shared::{ntdef, sspi, winerror};
use winapi::um::{self, wincrypt};

use {INIT_REQUESTS, ACCEPT_REQUESTS, Inner, secbuf, secbuf_desc};
use cert_chain::{CertChain, CertChainContext};
use cert_store::{CertAdd, CertStore};
use cert_context::CertContext;
use security_context::SecurityContext;
use context_buffer::ContextBuffer;
use schannel_cred::SchannelCred;

lazy_static! {
    static ref szOID_PKIX_KP_SERVER_AUTH: Vec<u8> =
        wincrypt::szOID_PKIX_KP_SERVER_AUTH.bytes().chain(Some(0)).collect();
    static ref szOID_SERVER_GATED_CRYPTO: Vec<u8> =
        wincrypt::szOID_SERVER_GATED_CRYPTO.bytes().chain(Some(0)).collect();
    static ref szOID_SGC_NETSCAPE: Vec<u8> =
        wincrypt::szOID_SGC_NETSCAPE.bytes().chain(Some(0)).collect();
}

/// A builder type for `TlsStream`s.
pub struct Builder {
    domain: Option<Vec<u16>>,
    use_sni: bool,
    accept_invalid_hostnames: bool,
    verify_callback: Option<Arc<Fn(CertValidationResult) -> io::Result<()> + Sync + Send>>,
    cert_store: Option<CertStore>,
}

impl Default for Builder {
    fn default() -> Builder {
        Builder {
            domain: None,
            use_sni: true,
            accept_invalid_hostnames: false,
            verify_callback: None,
            cert_store: None,
        }
    }
}

impl Builder {
    /// Returns a new `Builder`.
    pub fn new() -> Builder {
        Builder::default()
    }

    /// Sets the domain associated with connections created with this `Builder`.
    ///
    /// The domain will be used for Server Name Indication as well as
    /// certificate validation.
    pub fn domain(&mut self, domain: &str) -> &mut Builder {
        self.domain = Some(domain.encode_utf16().chain(Some(0)).collect());
        self
    }

    /// Determines if Server Name Indication (SNI) will be used.
    ///
    /// Defaults to `true`.
    pub fn use_sni(&mut self, use_sni: bool) -> &mut Builder {
        self.use_sni = use_sni;
        self
    }

    /// Determines if the server's hostname will be checked during certificate verification.
    ///
    /// Defaults to `false`.
    pub fn accept_invalid_hostnames(&mut self, accept_invalid_hostnames: bool) -> &mut Builder {
        self.accept_invalid_hostnames = accept_invalid_hostnames;
        self
    }

    /// Set a verification callback to be used for connections created with this `Builder`.
    ///
    /// The callback is provided with an io::Result indicating if the (pre)validation was
    /// successful. The Ok() variant indicates a successful validation while the Err() variant
    /// contains the errorcode returned from the internal verification process.
    /// The validated certificate, is accessible through the second argument of the closure.
    pub fn verify_callback<F>(&mut self, callback: F) -> &mut Builder
        where F: Fn(CertValidationResult) -> io::Result<()> + 'static + Sync + Send
    {
        self.verify_callback = Some(Arc::new(callback));
        self
    }

    /// Specifies a custom certificate store which is later used when validating
    /// a server's certificate.
    ///
    /// This option is only used for client connections and is used to construct
    /// the certificate chain which the server's certificate is validated
    /// against.
    ///
    /// Note that adding certificates here means that they are
    /// implicitly trusted.
    pub fn cert_store(&mut self, cert_store: CertStore) -> &mut Builder {
        self.cert_store = Some(cert_store);
        self
    }

    /// Initialize a new TLS session where the stream provided will be
    /// connecting to a remote TLS server.
    ///
    /// If the stream provided is a blocking stream then the entire handshake
    /// will be performed if possible, but if the stream is in nonblocking mode
    /// then a `HandshakeError::Interrupted` variant may be returned. This
    /// type can then be extracted to later call
    /// `MidHandshakeTlsStream::handshake` when data becomes available.
    pub fn connect<S>(&mut self,
                      cred: SchannelCred,
                      stream: S)
                      -> Result<TlsStream<S>, HandshakeError<S>>
        where S: Read + Write
    {
        self.initialize(cred, false, stream)
    }

    /// Initialize a new TLS session where the stream provided will be
    /// accepting a connection.
    ///
    /// This method will tweak the protocol for "who talks first" and also
    /// currently disables validation of the client that's connecting to us.
    ///
    /// If the stream provided is a blocking stream then the entire handshake
    /// will be performed if possible, but if the stream is in nonblocking mode
    /// then a `HandshakeError::Interrupted` variant may be returned. This
    /// type can then be extracted to later call
    /// `MidHandshakeTlsStream::handshake` when data becomes available.
    pub fn accept<S>(&mut self,
                     cred: SchannelCred,
                     stream: S)
                     -> Result<TlsStream<S>, HandshakeError<S>>
        where S: Read + Write
    {
        self.initialize(cred, true, stream)
    }

    fn initialize<S>(&mut self,
                     mut cred: SchannelCred,
                     server: bool,
                     stream: S)
                         -> Result<TlsStream<S>, HandshakeError<S>>
        where S: Read + Write
    {
        let domain = match self.domain {
            Some(ref domain) if self.use_sni => Some(&domain[..]),
            _ => None,
        };
        let (ctxt, buf) = match SecurityContext::initialize(&mut cred,
                                                            server,
                                                            domain) {
            Ok(pair) => pair,
            Err(e) => return Err(HandshakeError::Failure(e)),
        };

        let stream = TlsStream {
            cred: cred,
            context: ctxt,
            cert_store: self.cert_store.clone(),
            domain: self.domain.clone(),
            use_sni: self.use_sni,
            accept_invalid_hostnames: self.accept_invalid_hostnames,
            verify_callback: self.verify_callback.clone(),
            stream: stream,
            server: server,
            accept_first: true,
            state: State::Initializing {
                needs_flush: false,
                more_calls: true,
                shutting_down: false,
                validated: false,
            },
            needs_read: 1,
            dec_in: Cursor::new(Vec::new()),
            enc_in: Cursor::new(Vec::new()),
            out_buf: Cursor::new(buf.map(|b| b.to_owned()).unwrap_or(Vec::new())),
            last_write_len: 0,
        };

        MidHandshakeTlsStream {
            inner: stream,
        }.handshake()
    }
}

enum State {
    Initializing {
        needs_flush: bool,
        more_calls: bool,
        shutting_down: bool,
        validated: bool,
    },
    Streaming { sizes: sspi::SecPkgContext_StreamSizes, },
    Shutdown,
}

/// An Schannel TLS stream.
pub struct TlsStream<S> {
    cred: SchannelCred,
    context: SecurityContext,
    cert_store: Option<CertStore>,
    domain: Option<Vec<u16>>,
    use_sni: bool,
    accept_invalid_hostnames: bool,
    verify_callback: Option<Arc<Fn(CertValidationResult) -> io::Result<()> + Sync + Send>>,
    stream: S,
    state: State,
    server: bool,
    accept_first: bool,
    needs_read: usize,
    // valid from position() to len()
    dec_in: Cursor<Vec<u8>>,
    // valid from 0 to position()
    enc_in: Cursor<Vec<u8>>,
    // valid from position() to len()
    out_buf: Cursor<Vec<u8>>,
    /// the (unencrypted) length of the last write call used to track writes
    last_write_len: usize,
}

/// ensures that a TlsStream is always Sync/Send
fn _is_sync() {
    fn sync<T: Sync + Send>() {}
    sync::<TlsStream<()>>();
}

/// A failure which can happen during the `Builder::initialize` phase, either an
/// I/O error or an intermediate stream which has not completed its handshake.
#[derive(Debug)]
pub enum HandshakeError<S> {
    /// A fatal I/O error occurred
    Failure(io::Error),
    /// The stream connection is in progress, but the handshake is not completed
    /// yet.
    Interrupted(MidHandshakeTlsStream<S>),
}

/// A struct used to wrap various cert chain validation results for callback processing.
pub struct CertValidationResult {
    chain: CertChainContext,
    res: i32,
    chain_index: i32,
    element_index: i32,
}

impl CertValidationResult {
    /// Returns the certificate that failed validation if applicable
    pub fn failed_certificate(&self) -> Option<CertContext> {
        if let Some(cert_chain) = self.chain.get_chain(self.chain_index as usize) {
            return cert_chain.get(self.element_index as usize);
        }
        None
    }

    /// Returns the final certificate chain in the certificate context if applicable
    pub fn chain(&self) -> Option<CertChain> {
        self.chain.final_chain()
    }

    /// Returns the result of the built-in certificate verification process.
    pub fn result(&self) -> io::Result<()> {
        if self.res as u32 != winerror::ERROR_SUCCESS {
                Err(io::Error::from_raw_os_error(self.res))
        } else {
                Ok(())
        }
    }
}

impl<S: fmt::Debug + Any> Error for HandshakeError<S> {
    fn description(&self) -> &str {
        match *self {
            HandshakeError::Failure(_) => "failed to perform handshake",
            HandshakeError::Interrupted(_) => "interrupted performing handshake",
        }
    }

    fn cause(&self) -> Option<&Error> {
        match *self {
            HandshakeError::Failure(ref e) => Some(e),
            HandshakeError::Interrupted(_) => None,
        }
    }
}

impl<S: fmt::Debug + Any> fmt::Display for HandshakeError<S> {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        try!(f.write_str(self.description()));
        if let Some(e) = self.cause() {
           try!(write!(f, ": {}", e));
        }
        Ok(())
    }
}

/// A stream which has not yet completed its handshake.
#[derive(Debug)]
pub struct MidHandshakeTlsStream<S> {
    inner: TlsStream<S>,
}

impl<S> fmt::Debug for TlsStream<S>
    where S: fmt::Debug
{
    fn fmt(&self, fmt: &mut fmt::Formatter) -> fmt::Result {
        fmt.debug_struct("TlsStream")
            .field("stream", &self.stream)
            .finish()
    }
}

impl<S> TlsStream<S>
    where S: Read + Write
{
    /// Returns a reference to the wrapped stream.
    pub fn get_ref(&self) -> &S {
        &self.stream
    }

    /// Returns a mutable reference to the wrapped stream.
    pub fn get_mut(&mut self) -> &mut S {
        &mut self.stream
    }

    /// Indicates if this stream is the server- or client-side of a TLS session.
    pub fn is_server(&self) -> bool {
        self.server
    }

    /// Returns the certificate used to identify this side of the TLS session.
    ///
    /// Its associated cert store contains any intermediate certificates sent
    /// along with the leaf.
    pub fn certificate(&self) -> io::Result<CertContext> {
        self.context.local_cert()
    }

    /// Returns the peer's certificate, if available.
    ///
    /// Its associated cert store contains any intermediate certificates sent
    /// by the server.
    pub fn peer_certificate(&self) -> io::Result<CertContext> {
        self.context.remote_cert()
    }

    /// Returns a reference to the buffer of pending data.
    ///
    /// Like `BufRead::fill_buf` except that it will return an empty slice
    /// rather than reading from the wrapped stream if there is no buffered
    /// data.
    pub fn get_buf(&self) -> &[u8] {
        &self.dec_in.get_ref()[self.dec_in.position() as usize..]
    }

    /// Shuts the TLS session down.
    pub fn shutdown(&mut self) -> io::Result<()> {
        match self.state {
            State::Shutdown => return Ok(()),
            State::Initializing { shutting_down: true, .. } => {}
            _ => {
                unsafe {
                    let mut token = um::schannel::SCHANNEL_SHUTDOWN;
                    let ptr = &mut token as *mut _ as *mut u8;
                    let size = mem::size_of_val(&token);
                    let token = slice::from_raw_parts_mut(ptr, size);
                    let mut buf = [secbuf(sspi::SECBUFFER_TOKEN, Some(token))];
                    let mut desc = secbuf_desc(&mut buf);

                    match sspi::ApplyControlToken(self.context.get_mut(), &mut desc) {
                        winerror::SEC_E_OK => {}
                        err => return Err(io::Error::from_raw_os_error(err as i32)),
                    }
                }

                self.state = State::Initializing {
                    needs_flush: false,
                    more_calls: true,
                    shutting_down: true,
                    validated: false,
                };
                self.needs_read = 0;
            }
        }

        self.initialize().map(|_| ())
    }

    fn step_initialize(&mut self) -> io::Result<()> {
        unsafe {
            let pos = self.enc_in.position() as usize;
            let mut inbufs = [secbuf(sspi::SECBUFFER_TOKEN,
                                     Some(&mut self.enc_in.get_mut()[..pos])),
                              secbuf(sspi::SECBUFFER_EMPTY, None)];
            let mut inbuf_desc = secbuf_desc(&mut inbufs);

            let mut outbufs = [secbuf(sspi::SECBUFFER_TOKEN, None),
                               secbuf(sspi::SECBUFFER_ALERT, None),
                               secbuf(sspi::SECBUFFER_EMPTY, None)];
            let mut outbuf_desc = secbuf_desc(&mut outbufs);

            let mut attributes = 0;

            let status = if self.server {
                let ptr = if self.accept_first {
                    ptr::null_mut()
                } else {
                    self.context.get_mut()
                };
                sspi::AcceptSecurityContext(self.cred.get_mut(),
                                            ptr,
                                            &mut inbuf_desc,
                                            ACCEPT_REQUESTS,
                                            0,
                                            self.context.get_mut(),
                                            &mut outbuf_desc,
                                            &mut attributes,
                                            ptr::null_mut())
            } else {
                let domain = match self.domain {
                    Some(ref domain) if self.use_sni => domain.as_ptr() as *mut u16,
                    _ => ptr::null_mut(),
                };

                sspi::InitializeSecurityContextW(self.cred.get_mut(),
                                                 self.context.get_mut(),
                                                 domain,
                                                 INIT_REQUESTS,
                                                 0,
                                                 0,
                                                 &mut inbuf_desc,
                                                 0,
                                                 ptr::null_mut(),
                                                 &mut outbuf_desc,
                                                 &mut attributes,
                                                 ptr::null_mut())
            };

            for buf in &outbufs[1..] {
                if !buf.pvBuffer.is_null() {
                    sspi::FreeContextBuffer(buf.pvBuffer);
                }
            }

            match status {
                winerror::SEC_I_CONTINUE_NEEDED => {
                    // Windows apparently doesn't like AcceptSecurityContext
                    // being called as if it were the second time unless the
                    // first call to AcceptSecurityContext succeeded with
                    // CONTINUE_NEEDED.
                    //
                    // In other words, if we were to set `accept_first` to
                    // `false` after the literal first call to
                    // `AcceptSecurityContext` while the call returned
                    // INCOMPLETE_MESSAGE, the next call would return an error.
                    //
                    // For that reason we only set `accept_first` to false here
                    // once we've actually successfully received the full
                    // "token" from the client.
                    self.accept_first = false;
                    let nread = if inbufs[1].BufferType == sspi::SECBUFFER_EXTRA {
                        self.enc_in.position() as usize - inbufs[1].cbBuffer as usize
                    } else {
                        self.enc_in.position() as usize
                    };
                    let to_write = ContextBuffer(outbufs[0]);

                    self.consume_enc_in(nread);
                    self.needs_read = (self.enc_in.position() == 0) as usize;
                    self.out_buf.get_mut().extend_from_slice(&to_write);
                }
                winerror::SEC_E_INCOMPLETE_MESSAGE => {
                    self.needs_read = if inbufs[1].BufferType == sspi::SECBUFFER_MISSING {
                        inbufs[1].cbBuffer as usize
                    } else {
                        1
                    };
                }
                winerror::SEC_E_OK => {
                    let nread = if inbufs[1].BufferType == sspi::SECBUFFER_EXTRA {
                        self.enc_in.position() as usize - inbufs[1].cbBuffer as usize
                    } else {
                        self.enc_in.position() as usize
                    };
                    let to_write = if outbufs[0].pvBuffer.is_null() {
                        None
                    } else {
                        Some(ContextBuffer(outbufs[0]))
                    };

                    self.consume_enc_in(nread);
                    self.needs_read = (self.enc_in.position() == 0) as usize;
                    if let Some(to_write) = to_write {
                        self.out_buf.get_mut().extend_from_slice(&to_write);
                    }
                    if self.enc_in.position() != 0 {
                        try!(self.decrypt());
                    }
                    if let State::Initializing { ref mut more_calls, .. } = self.state {
                        *more_calls = false;
                    }
                }
                _ => {
                    return Err(io::Error::from_raw_os_error(status as i32))
                }
            }
            Ok(())
        }
    }

    fn initialize(&mut self) -> io::Result<Option<sspi::SecPkgContext_StreamSizes>> {
        loop {
            match self.state {
                State::Initializing { mut needs_flush, more_calls, shutting_down, validated } => {
                    if try!(self.write_out()) > 0 {
                        needs_flush = true;
                        if let State::Initializing { ref mut needs_flush, .. } = self.state {
                            *needs_flush = true;
                        }
                    }

                    if needs_flush {
                        try!(self.stream.flush());
                        if let State::Initializing { ref mut needs_flush, .. } = self.state {
                            *needs_flush = false;
                        }
                    }

                    if !shutting_down && !validated {
                        // on the last call, we require a valid certificate
                        if try!(self.validate(!more_calls)) {
                            if let State::Initializing { ref mut validated, .. } = self.state {
                                *validated = true;
                            }
                        }
                    }

                    if !more_calls {
                        self.state = if shutting_down {
                            State::Shutdown
                        } else {
                            State::Streaming { sizes: try!(self.context.stream_sizes()) }
                        };
                        continue;
                    }

                    if self.needs_read > 0 {
                        if try!(self.read_in()) == 0 {
                            return Err(io::Error::new(io::ErrorKind::UnexpectedEof,
                                                      "unexpected EOF during handshake"));
                        }
                    }

                    try!(self.step_initialize());
                }
                State::Streaming { sizes } => return Ok(Some(sizes)),
                State::Shutdown => return Ok(None),
            }
        }
    }

    /// Returns true when the certificate was succesfully verified
    /// Returns false, when a verification isn't necessary (yet)
    /// Returns an error when the verification failed
    fn validate(&mut self, require_cert: bool) -> io::Result<bool> {
        // If we're accepting connections then we don't perform any validation
        // for the remote certificate, that's what they're doing!
        if self.server {
            return Ok(false);
        }

        let cert_context = match self.context.remote_cert() {
            Err(_) if !require_cert => return Ok(false),
            ret => try!(ret)
        };

        let cert_chain = unsafe {
            let cert_store = match (cert_context.cert_store(), &self.cert_store) {
                (Some(ref mut chain_certs), &Some(ref extra_certs)) => {
                    for extra_cert in extra_certs.certs() {
                        try!(chain_certs.add_cert(&extra_cert, CertAdd::ReplaceExisting));
                    }
                    chain_certs.as_inner()
                },
                (Some(chain_certs), &None) => chain_certs.as_inner(),
                (None, &Some(ref extra_certs)) => extra_certs.as_inner(),
                (None, &None) => ptr::null_mut()
            };

            let flags = wincrypt::CERT_CHAIN_CACHE_END_CERT |
                        wincrypt::CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY |
                        wincrypt::CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT;

            let mut para: wincrypt::CERT_CHAIN_PARA = mem::zeroed();
            para.cbSize = mem::size_of_val(&para) as winapi::DWORD;
            para.RequestedUsage.dwType = wincrypt::USAGE_MATCH_TYPE_OR;

            let mut identifiers = [szOID_PKIX_KP_SERVER_AUTH.as_ptr() as ntdef::LPSTR,
                                   szOID_SERVER_GATED_CRYPTO.as_ptr() as ntdef::LPSTR,
                                   szOID_SGC_NETSCAPE.as_ptr() as ntdef::LPSTR];
            para.RequestedUsage.Usage.cUsageIdentifier = identifiers.len() as winapi::DWORD;
            para.RequestedUsage.Usage.rgpszUsageIdentifier = identifiers.as_mut_ptr();

            let mut cert_chain = mem::zeroed();

            let res = wincrypt::CertGetCertificateChain(ptr::null_mut(),
                                                        cert_context.as_inner(),
                                                        ptr::null_mut(),
                                                        cert_store,
                                                        &mut para,
                                                        flags,
                                                        ptr::null_mut(),
                                                        &mut cert_chain);

            if res == winapi::TRUE {
                CertChainContext(cert_chain as *mut _)
            } else {
                return Err(io::Error::last_os_error())
            }
        };

        unsafe {
            // check if we trust the root-CA explicitly
            let mut para_flags = wincrypt::CERT_CHAIN_POLICY_IGNORE_ALL_REV_UNKNOWN_FLAGS;
            if let Some(ref mut store) = self.cert_store {
                if let Some(chain) = cert_chain.final_chain() {
                    // check if any cert of the chain is in the passed store (and therefore trusted)
                    if chain.certificates().any(|cert| store.certs().any(|root_cert| root_cert == cert)) {
                        para_flags |= wincrypt::CERT_CHAIN_POLICY_ALLOW_UNKNOWN_CA_FLAG;
                    }
                }
            }

            let mut extra_para: wincrypt::SSL_EXTRA_CERT_CHAIN_POLICY_PARA = mem::zeroed();
            *extra_para.u.cbSize_mut() = mem::size_of_val(&extra_para) as winapi::DWORD;
            extra_para.dwAuthType = wincrypt::AUTHTYPE_SERVER;
            match self.domain {
                Some(ref mut domain) if !self.accept_invalid_hostnames => {
                    extra_para.pwszServerName = domain.as_mut_ptr();
                }
                _ => {}
            }

            let mut para: wincrypt::CERT_CHAIN_POLICY_PARA = mem::zeroed();
            para.cbSize = mem::size_of_val(&para) as winapi::DWORD;
            para.dwFlags = para_flags;
            para.pvExtraPolicyPara = &mut extra_para as *mut _ as *mut _;

            let mut status: wincrypt::CERT_CHAIN_POLICY_STATUS = mem::zeroed();
            status.cbSize = mem::size_of_val(&status) as winapi::DWORD;

            let verify_chain_policy_structure = wincrypt::CERT_CHAIN_POLICY_SSL as ntdef::LPCSTR;
            let res = wincrypt::CertVerifyCertificateChainPolicy(verify_chain_policy_structure,
                                                                cert_chain.0,
                                                                &mut para,
                                                                &mut status);
            if res == winapi::FALSE {
                return Err(io::Error::last_os_error())
            }

            let mut verify_result = if status.dwError != winerror::ERROR_SUCCESS {
                Err(io::Error::from_raw_os_error(status.dwError as i32))
            } else {
                Ok(())
            };

            // check if there's a user-specified verify callback
            if let Some(ref callback) = self.verify_callback {
                verify_result = callback(CertValidationResult{
                    chain: cert_chain,
                    res: status.dwError as i32,
                    chain_index: status.lChainIndex,
                    element_index: status.lElementIndex});
            }
            try!(verify_result);
        }
        Ok(true)
    }

    fn write_out(&mut self) -> io::Result<usize> {
        let mut out = 0;
        while self.out_buf.position() as usize != self.out_buf.get_ref().len() {
            let position = self.out_buf.position() as usize;
            let nwritten = try!(self.stream.write(&self.out_buf.get_ref()[position..]));
            out += nwritten;
            self.out_buf.set_position((position + nwritten) as u64);
        }

        Ok(out)
    }

    fn read_in(&mut self) -> io::Result<usize> {
        let mut sum_nread = 0;

        while self.needs_read > 0 {
            let existing_len = self.enc_in.position() as usize;
            let min_len = cmp::max(cmp::max(1024, 2 * existing_len), self.needs_read);
            if self.enc_in.get_ref().len() < min_len {
                self.enc_in.get_mut().resize(min_len, 0);
            }
            let nread = {
                let buf = &mut self.enc_in.get_mut()[existing_len..];
                try!(self.stream.read(buf))
            };
            self.enc_in.set_position((existing_len + nread) as u64);
            self.needs_read = self.needs_read.saturating_sub(nread);
            if nread == 0 {
                break;
            }
            sum_nread += nread;
        }

        Ok(sum_nread)
    }

    fn consume_enc_in(&mut self, nread: usize) {
        let size = self.enc_in.position() as usize;
        assert!(size >= nread);
        let count = size - nread;

        if count > 0 {
            self.enc_in.get_mut().drain(..nread);
        }

        self.enc_in.set_position(count as u64);
    }

    fn decrypt(&mut self) -> io::Result<bool> {
        unsafe {
            let position = self.enc_in.position() as usize;
            let mut bufs = [secbuf(sspi::SECBUFFER_DATA,
                                   Some(&mut self.enc_in.get_mut()[..position])),
                            secbuf(sspi::SECBUFFER_EMPTY, None),
                            secbuf(sspi::SECBUFFER_EMPTY, None),
                            secbuf(sspi::SECBUFFER_EMPTY, None)];
            let mut bufdesc = secbuf_desc(&mut bufs);

            match sspi::DecryptMessage(self.context.get_mut(),
                                          &mut bufdesc,
                                          0,
                                          ptr::null_mut()) {
                winerror::SEC_E_OK => {
                    let start = bufs[1].pvBuffer as usize - self.enc_in.get_ref().as_ptr() as usize;
                    let end = start + bufs[1].cbBuffer as usize;
                    self.dec_in.get_mut().clear();
                    self.dec_in
                        .get_mut()
                        .extend_from_slice(&self.enc_in.get_ref()[start..end]);
                    self.dec_in.set_position(0);

                    let nread = if bufs[3].BufferType == sspi::SECBUFFER_EXTRA {
                        self.enc_in.position() as usize - bufs[3].cbBuffer as usize
                    } else {
                        self.enc_in.position() as usize
                    };
                    self.consume_enc_in(nread);
                    self.needs_read = (self.enc_in.position() == 0) as usize;
                    Ok(false)
                }
                winerror::SEC_E_INCOMPLETE_MESSAGE => {
                    self.needs_read = if bufs[1].BufferType == sspi::SECBUFFER_MISSING {
                        bufs[1].cbBuffer as usize
                    } else {
                        1
                    };
                    Ok(false)
                }
                winerror::SEC_I_CONTEXT_EXPIRED => Ok(true),
                winerror::SEC_I_RENEGOTIATE => {
                    self.state = State::Initializing {
                        needs_flush: false,
                        more_calls: true,
                        shutting_down: false,
                        validated: false,
                    };

                    let nread = if bufs[3].BufferType == sspi::SECBUFFER_EXTRA {
                        self.enc_in.position() as usize - bufs[3].cbBuffer as usize
                    } else {
                        self.enc_in.position() as usize
                    };
                    self.consume_enc_in(nread);
                    self.needs_read = 0;
                    Ok(false)
                }
                e => Err(io::Error::from_raw_os_error(e as i32)),
            }
        }
    }

    fn encrypt(&mut self, buf: &[u8], sizes: &sspi::SecPkgContext_StreamSizes) -> io::Result<()> {
        assert!(buf.len() <= sizes.cbMaximumMessage as usize);

        unsafe {
            let len = sizes.cbHeader as usize + buf.len() + sizes.cbTrailer as usize;

            if self.out_buf.get_ref().len() < len {
                self.out_buf.get_mut().resize(len, 0);
            }

            let message_start = sizes.cbHeader as usize;
            self.out_buf
                .get_mut()[message_start..message_start + buf.len()]
                .clone_from_slice(buf);

            let mut bufs = {
                let out_buf = self.out_buf.get_mut();
                let size = sizes.cbHeader as usize;

                let header = secbuf(sspi::SECBUFFER_STREAM_HEADER,
                                    Some(&mut out_buf[..size]));
                let data = secbuf(sspi::SECBUFFER_DATA,
                                  Some(&mut out_buf[size..size + buf.len()]));
                let trailer = secbuf(sspi::SECBUFFER_STREAM_TRAILER,
                                     Some(&mut out_buf[size + buf.len()..]));
                let empty = secbuf(sspi::SECBUFFER_EMPTY, None);
                [header, data, trailer, empty]
            };
            let mut bufdesc = secbuf_desc(&mut bufs);

            match sspi::EncryptMessage(self.context.get_mut(), 0, &mut bufdesc, 0) {
                winerror::SEC_E_OK => {
                    let len = bufs[0].cbBuffer + bufs[1].cbBuffer + bufs[2].cbBuffer;
                    self.out_buf.get_mut().truncate(len as usize);
                    self.out_buf.set_position(0);
                    Ok(())
                }
                err => Err(io::Error::from_raw_os_error(err as i32)),
            }
        }
    }
}

impl<S> MidHandshakeTlsStream<S>
    where S: Read + Write,
{
    /// Returns a shared reference to the inner stream.
    pub fn get_ref(&self) -> &S {
        self.inner.get_ref()
    }

    /// Returns a mutable reference to the inner stream.
    pub fn get_mut(&mut self) -> &mut S {
        self.inner.get_mut()
    }

    /// Restarts the handshake process.
    pub fn handshake(mut self) -> Result<TlsStream<S>, HandshakeError<S>> {
        match self.inner.initialize() {
            Ok(_) => Ok(self.inner),
            Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {
                Err(HandshakeError::Interrupted(self))
            }
            Err(e) => Err(HandshakeError::Failure(e)),
        }
    }
}

impl<S> Write for TlsStream<S>
    where S: Read + Write
{
    /// In the case of a WouldBlock error, we expect another call
    /// starting with the same input data
    /// This is similar to the use of ACCEPT_MOVING_WRITE_BUFFER in openssl
    fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
        let sizes = match try!(self.initialize()) {
            Some(sizes) => sizes,
            None => return Err(io::Error::from_raw_os_error(winerror::SEC_E_CONTEXT_EXPIRED as i32)),
        };

        // if we have pending output data, it must have been because a previous
        // attempt to send this part of the data ran into an error.
        if self.out_buf.position() == self.out_buf.get_ref().len() as u64 {
            let len = cmp::min(buf.len(), sizes.cbMaximumMessage as usize);
            try!(self.encrypt(&buf[..len], &sizes));
            self.last_write_len = len;
        }
        try!(self.write_out());

        Ok(self.last_write_len)
    }

    fn flush(&mut self) -> io::Result<()> {
        // Make sure the write buffer is emptied
        try!(self.write_out());
        self.stream.flush()
    }
}

impl<S> Read for TlsStream<S>
    where S: Read + Write
{
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        let nread = {
            let read_buf = try!(self.fill_buf());
            let nread = cmp::min(buf.len(), read_buf.len());
            buf[..nread].copy_from_slice(&read_buf[..nread]);
            nread
        };
        self.consume(nread);
        Ok(nread)
    }
}

impl<S> BufRead for TlsStream<S>
    where S: Read + Write
{
    fn fill_buf(&mut self) -> io::Result<&[u8]> {
        while self.get_buf().is_empty() {
            if let None = try!(self.initialize()) {
                break;
            }

            if self.needs_read > 0 {
                if try!(self.read_in()) == 0 {
                    break;
                }
                self.needs_read = 0;
            }

            let eof = try!(self.decrypt());
            if eof {
                break;
            }
        }

        Ok(self.get_buf())
    }

    fn consume(&mut self, amt: usize) {
        let pos = self.dec_in.position() + amt as u64;
        assert!(pos <= self.dec_in.get_ref().len() as u64);
        self.dec_in.set_position(pos);
    }
}
