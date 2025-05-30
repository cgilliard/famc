use std::env;
use std::io::{self, Read, Write, Error};
use std::mem;
use std::net::{TcpStream, TcpListener};
use std::ptr;
use std::sync::{Once, ONCE_INIT};
use std::thread;
use winapi::shared::minwindef as winapi;
use winapi::shared::{basetsd, ntdef, lmcons, winerror};
use winapi::um::{minwinbase, sysinfoapi, timezoneapi, wincrypt};

use Inner;
use crypt_prov::{AcquireOptions, ProviderType};
use cert_context::{CertContext, KeySpec, HashAlgorithm};
use cert_store::{CertStore, Memory, CertAdd};
use schannel_cred::{Direction, Protocol, Algorithm, SchannelCred};
use tls_stream::{self, HandshakeError};

#[test]
fn basic() {
    let creds = SchannelCred::builder().acquire(Direction::Outbound).unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("google.com")
        .connect(creds, stream)
        .unwrap();
    stream.write_all(b"GET / HTTP/1.0\r\n\r\n").unwrap();
    let mut out = vec![];
    stream.read_to_end(&mut out).unwrap();
    assert!(out.starts_with(b"HTTP/1.0 200 OK") || out.starts_with(b"HTTP/1.0 302 Found"));
    assert!(out.ends_with(b"</html>") || out.ends_with(b"</HTML>\r\n"));
}

#[test]
fn invalid_algorithms() {
    let creds = SchannelCred::builder()
        .supported_algorithms(&[Algorithm::Rc2, Algorithm::Ecdsa])
        .acquire(Direction::Outbound);
    assert_eq!(creds.err().unwrap().raw_os_error().unwrap(),
               winerror::SEC_E_ALGORITHM_MISMATCH as i32);
}

#[test]
fn valid_algorithms() {
    let creds = SchannelCred::builder()
        .supported_algorithms(&[Algorithm::Aes128, Algorithm::Ecdsa])
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("google.com")
        .connect(creds, stream)
        .unwrap();
    stream.write_all(b"GET / HTTP/1.0\r\n\r\n").unwrap();
    let mut out = vec![];
    stream.read_to_end(&mut out).unwrap();
    assert!(out.starts_with(b"HTTP/1.0 200 OK") || out.starts_with(b"HTTP/1.0 302 Found"));
    assert!(out.ends_with(b"</html>") || out.ends_with(b"</HTML>\r\n"));
}

fn unwrap_handshake<S>(e: HandshakeError<S>) -> io::Error {
    match e {
        HandshakeError::Failure(e) => e,
        HandshakeError::Interrupted(_) => panic!("not an I/O error"),
    }
}

#[test]
#[ignore] // google's inconsistent about disallowing sslv3
fn invalid_protocol() {
    let creds = SchannelCred::builder()
        .enabled_protocols(&[Protocol::Ssl3])
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("google.com")
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::SEC_E_UNSUPPORTED_FUNCTION as i32);
}

#[test]
fn valid_protocol() {
    let creds = SchannelCred::builder()
        .enabled_protocols(&[Protocol::Tls12])
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("google.com")
        .connect(creds, stream)
        .unwrap();
    stream.write_all(b"GET / HTTP/1.0\r\n\r\n").unwrap();
    let mut out = vec![];
    stream.read_to_end(&mut out).unwrap();
    assert!(out.starts_with(b"HTTP/1.0 200 OK") || out.starts_with(b"HTTP/1.0 302 Found"));
    assert!(out.ends_with(b"</html>") || out.ends_with(b"</HTML>\r\n"));
}

#[test]
fn valid_protocol_with_intermediate_certs() {
    let creds = SchannelCred::builder()
        .enabled_protocols(&[Protocol::Tls12])
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("lh3.googleusercontent.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("lh3.googleusercontent.com")
        .connect(creds, stream)
        .unwrap();
    stream.write_all(b"GET / HTTP/1.0\r\n\r\n").unwrap();
    let mut out = vec![];
    stream.read_to_end(&mut out).unwrap();
    assert!(out.starts_with(b"HTTP/1.0 200 OK") || out.starts_with(b"HTTP/1.0 302 Found"));
    assert!(out.ends_with(b"</html>") || out.ends_with(b"</HTML>\r\n"));
}

#[test]
fn expired_cert() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("expired.badssl.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("expired.badssl.com")
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(), winerror::CERT_E_EXPIRED as i32);
}

#[test]
fn self_signed_cert() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("self-signed.badssl.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("self-signed.badssl.com")
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::CERT_E_UNTRUSTEDROOT as i32);
}

#[test]
fn self_signed_cert_manual_trust() {
    let cert = include_bytes!("../test/self-signed.badssl.com.cer");
    let mut store = Memory::new().unwrap();
    store.add_encoded_certificate(cert).unwrap();

    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("self-signed.badssl.com:443").unwrap();
    tls_stream::Builder::new()
        .domain("self-signed.badssl.com")
        .cert_store(store.into_store())
        .connect(creds, stream)
        .unwrap();
}

#[test]
fn wrong_host_cert() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("wrong.host.badssl.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("wrong.host.badssl.com")
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::CERT_E_CN_NO_MATCH as i32);
}

#[test]
fn wrong_host_cert_ignored() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("wrong.host.badssl.com:443").unwrap();
    tls_stream::Builder::new()
        .domain("wrong.host.badssl.com")
        .accept_invalid_hostnames(true)
        .connect(creds, stream)
        .unwrap();
}

#[test]
fn shutdown() {
    let creds = SchannelCred::builder().acquire(Direction::Outbound).unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("google.com")
        .connect(creds, stream)
        .unwrap();
    stream.shutdown().unwrap();
}

#[test]
fn validation_failure_is_permanent() {
    let creds = SchannelCred::builder().acquire(Direction::Outbound).unwrap();
    let stream = TcpStream::connect("self-signed.badssl.com:443").unwrap();
    // temporarily switch to nonblocking to allow us to construct the stream
    // without validating
    stream.set_nonblocking(true).unwrap();
    let stream = tls_stream::Builder::new()
        .domain("self-signed.badssl.com")
        .connect(creds, stream);
    let stream = match stream {
        Err(HandshakeError::Interrupted(s)) => s,
        _ => panic!(),
    };
    stream.get_ref().set_nonblocking(false).unwrap();
    let err = unwrap_handshake(stream.handshake().err().unwrap());
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::CERT_E_UNTRUSTEDROOT as i32);
}

#[test]
fn verify_callback_success() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("self-signed.badssl.com:443").unwrap();
    let mut stream = tls_stream::Builder::new()
        .domain("self-signed.badssl.com")
        .verify_callback(|validation_result| {
            assert!(validation_result.result().is_err());
            Ok(())
        })
        .connect(creds, stream)
        .unwrap();
    stream.write_all(b"GET / HTTP/1.0\r\nHost: self-signed.badssl.com\r\n\r\n").unwrap();
    let mut out = vec![];
    stream.read_to_end(&mut out).unwrap();
    assert!(out.starts_with(b"HTTP/1.1 200 OK"));
    assert!(out.ends_with(b"</html>\n"));
}

#[test]
fn verify_callback_error() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("google.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("google.com")
        .verify_callback(|validation_result| {
            assert!(validation_result.result().is_ok());
            Err(io::Error::from_raw_os_error(winerror::CERT_E_UNTRUSTEDROOT))
        })
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::CERT_E_UNTRUSTEDROOT as i32);
}

#[test]
fn verify_callback_gives_failed_cert() {
    let creds = SchannelCred::builder()
        .acquire(Direction::Outbound)
        .unwrap();
    let stream = TcpStream::connect("self-signed.badssl.com:443").unwrap();
    let err = tls_stream::Builder::new()
        .domain("self-signed.badssl.com")
        .verify_callback(|validation_result| {
            let expected_finger = vec!(100, 20, 80, 217, 74, 101, 250, 235, 59, 99, 16, 40, 216, 232, 108, 149, 67, 29, 184, 17);
            assert_eq!(validation_result.failed_certificate().unwrap().fingerprint(HashAlgorithm::sha1()).unwrap(), expected_finger);
            Err(io::Error::from_raw_os_error(winerror::CERT_E_UNTRUSTEDROOT))
        })
        .connect(creds, stream)
        .err()
        .unwrap();
    let err = unwrap_handshake(err);
    assert_eq!(err.raw_os_error().unwrap(),
               winerror::CERT_E_UNTRUSTEDROOT as i32);
}

const FRIENDLY_NAME: &'static str = "schannel-rs localhost testing cert";

lazy_static! {
    static ref szOID_RSA_SHA256RSA: Vec<u8> =
        wincrypt::szOID_RSA_SHA256RSA.bytes().chain(Some(0)).collect();
}

fn install_certificate() -> io::Result<CertContext> {
    unsafe {
        let mut provider = 0;
        let mut hkey = 0;

        let mut buffer = "schannel-rs test suite".encode_utf16()
                                                 .chain(Some(0))
                                                 .collect::<Vec<_>>();
        let res = wincrypt::CryptAcquireContextW(&mut provider,
                                                 buffer.as_ptr(),
                                                 ptr::null_mut(),
                                                 wincrypt::PROV_RSA_FULL,
                                                 wincrypt::CRYPT_MACHINE_KEYSET);
        if res != winapi::TRUE {
            // create a new key container (since it does not exist)
            let res = wincrypt::CryptAcquireContextW(&mut provider,
                                                     buffer.as_ptr(),
                                                     ptr::null_mut(),
                                                     wincrypt::PROV_RSA_FULL,
                                                     wincrypt::CRYPT_NEWKEYSET | wincrypt::CRYPT_MACHINE_KEYSET);
            if res != winapi::TRUE {
                return Err(Error::last_os_error())
            }
        }

        // create a new keypair (RSA-2048)
        let res = wincrypt::CryptGenKey(provider,
                                        wincrypt::AT_SIGNATURE,
                                        0x0800<<16 | wincrypt::CRYPT_EXPORTABLE,
                                        &mut hkey);
        if res != winapi::TRUE {
            return Err(Error::last_os_error());
        }

        // start creating the certificate
        let name = "CN=localhost,O=schannel-rs,OU=schannel-rs,G=schannel_rs".encode_utf16()
                                               .chain(Some(0))
                                               .collect::<Vec<_>>();
        let mut cname_buffer: [ntdef::WCHAR; lmcons::UNLEN as usize + 1] = mem::zeroed();
        let mut cname_len = cname_buffer.len() as winapi::DWORD;
        let res = wincrypt::CertStrToNameW(wincrypt::X509_ASN_ENCODING,
                                           name.as_ptr(),
                                           wincrypt::CERT_X500_NAME_STR,
                                           ptr::null_mut(),
                                           cname_buffer.as_mut_ptr() as *mut u8,
                                           &mut cname_len,
                                           ptr::null_mut());
        if res != winapi::TRUE {
            return Err(Error::last_os_error());
        }

        let mut subject_issuer = wincrypt::CERT_NAME_BLOB {
            cbData: cname_len,
            pbData: cname_buffer.as_ptr() as *mut u8,
        };
        let mut key_provider = wincrypt::CRYPT_KEY_PROV_INFO {
            pwszContainerName: buffer.as_mut_ptr(),
            pwszProvName: ptr::null_mut(),
            dwProvType: wincrypt::PROV_RSA_FULL,
            dwFlags: wincrypt::CRYPT_MACHINE_KEYSET,
            cProvParam: 0,
            rgProvParam: ptr::null_mut(),
            dwKeySpec: wincrypt::AT_SIGNATURE,
        };
        let mut sig_algorithm = wincrypt::CRYPT_ALGORITHM_IDENTIFIER {
            pszObjId: szOID_RSA_SHA256RSA.as_ptr() as *mut _,
            Parameters: mem::zeroed(),
        };
        let mut expiration_date: minwinbase::SYSTEMTIME = mem::zeroed();
        sysinfoapi::GetSystemTime(&mut expiration_date);
        let mut file_time: winapi::FILETIME = mem::zeroed();
        let res = timezoneapi::SystemTimeToFileTime(&mut expiration_date,
                                                    &mut file_time);
        if res != winapi::TRUE {
            return Err(Error::last_os_error());
        }
        let mut timestamp: u64 = file_time.dwLowDateTime as u64 |
                                 (file_time.dwHighDateTime as u64) << 32;
        // one day, timestamp unit is in 100 nanosecond intervals
        timestamp += (1E9 as u64) / 100 * (60 * 60 * 24);
        file_time.dwLowDateTime = timestamp as u32;
        file_time.dwHighDateTime = (timestamp >> 32) as u32;
        let res = timezoneapi::FileTimeToSystemTime(&file_time,
                                                    &mut expiration_date);
        if res != winapi::TRUE {
            return Err(Error::last_os_error());
        }

        // create a self signed certificate
        let cert_context = wincrypt::CertCreateSelfSignCertificate(0 as basetsd::ULONG_PTR,
                                                                   &mut subject_issuer,
                                                                   0,
                                                                   &mut key_provider,
                                                                   &mut sig_algorithm,
                                                                   ptr::null_mut(),
                                                                   &mut expiration_date,
                                                                   ptr::null_mut());
        if cert_context.is_null() {
            return Err(Error::last_os_error());
        }
        let cert_context = CertContext::from_inner(cert_context);
        try!(cert_context.set_friendly_name(FRIENDLY_NAME));

        // install the certificate to the machine's local store
        io::stdout().write_all(br#"

The schannel-rs test suite is about to add a certificate to your set of root
and trusted certificates. This certificate should be for the domain "localhost"
with the description related to "schannel". This certificate is only valid for
one day and will be automatically deleted if you re-run the schannel-rs test
suite later.

If you would rather not do this please cancel the addition and re-run the
test suite with SCHANNEL_RS_SKIP_SERVER_TESTS=1.

"#).unwrap();
        try!(local_root_store().add_cert(&cert_context, CertAdd::ReplaceExisting));
        Ok(cert_context)
    }
}

fn local_root_store() -> CertStore {
    if env::var("APPVEYOR").is_ok() {
        CertStore::open_local_machine("Root").unwrap()
    } else {
        CertStore::open_current_user("Root").unwrap()
    }
}

fn localhost_cert() -> Option<CertContext> {
    if env::var("SCHANNEL_RS_SKIP_SERVER_TESTS").is_ok() {
        return None
    }

    // Our tests need a certficiate that the system trusts to run with, and we
    // do this by basically generating a certificate on the fly. This
    // initialization block synchronizes tests to ensure that we only generate
    // one certificate which is then used by all the tests.
    //
    // First we check to see if the root trust store already has one of our
    // certificates, identified by the "friendly name" we set when the
    // certificate was created. If it's expired, then we delete it and generate
    // another. If none is found, we also generate another.
    //
    // Note that generating a certificate and adding it to the root trust store
    // will likely trigger a prompt to the user asking if they want to do that,
    // so we generate certificates that are valid for some amount of time so you
    // don't have to hit the "OK" button each time you run `cargo test`.
    //
    // After the initialization is performed we just probe the root store again
    // and find the certificate we added (or was already there).
    static INIT: Once = ONCE_INIT;
    INIT.call_once(|| {
        for cert in local_root_store().certs() {
            let name = match cert.friendly_name() {
                Ok(name) => name,
                Err(_) => continue,
            };
            if name != FRIENDLY_NAME {
                continue
            }
            if !cert.is_time_valid().unwrap() {
                io::stdout().write_all(br#"

The schannel-rs test suite is about to delete an old copy of one of its
certificates from your root trust store. This certificate was only valid for one
day and it is no longer needed. The host should be "localhost" and the
description should mention "schannel".

"#).unwrap();
                cert.delete().unwrap();
            } else {
                return
            }
        }

        install_certificate().unwrap();
    });

    for cert in local_root_store().certs() {
        let name = match cert.friendly_name() {
            Ok(name) => name,
            Err(_) => continue,
        };
        if name == FRIENDLY_NAME {
            return Some(cert)
        }
    }

    panic!("couldn't find a cert");
}

#[test]
fn accept_a_socket() {
    let cert = match localhost_cert() {
        Some(cert) => cert,
        None => return,
    };

    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    let addr = listener.local_addr().unwrap();
    let t = thread::spawn(move || {
        let stream = TcpStream::connect(&addr).unwrap();
        let creds = SchannelCred::builder()
                                 .acquire(Direction::Outbound).unwrap();
        let mut stream = tls_stream::Builder::new()
            .domain("localhost")
            .connect(creds, stream)
            .unwrap();
        stream.write_all(&[1, 2, 3, 4]).unwrap();
        stream.flush().unwrap();
        assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
        stream.shutdown().unwrap();
    });

    let stream = listener.accept().unwrap().0;
    let creds = SchannelCred::builder()
                        .cert(cert)
                        .acquire(Direction::Inbound)
                        .unwrap();
    let mut stream = tls_stream::Builder::new()
        .accept(creds, stream)
        .unwrap();
    assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
    stream.write_all(&[1, 2, 3, 4]).unwrap();
    stream.flush().unwrap();
    let mut buf = [0; 1];
    assert_eq!(stream.read(&mut buf).unwrap(), 0);

    t.join().unwrap();
}

#[test]
fn accept_one_byte_at_a_time() {
    let cert = match localhost_cert() {
        Some(cert) => cert,
        None => return,
    };

    #[derive(Debug)]
    struct OneByteAtATime<S> {
        inner: S,
    }

    impl<S: Read> Read for OneByteAtATime<S> {
        fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
            self.inner.read(&mut buf[..1])
        }
    }

    impl<S: Write> Write for OneByteAtATime<S> {
        fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
            self.inner.write(&buf[..1])
        }

        fn flush(&mut self) -> io::Result<()> {
            self.inner.flush()
        }
    }

    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    let addr = listener.local_addr().unwrap();
    let t = thread::spawn(move || {
        let stream = TcpStream::connect(&addr).unwrap();
        let creds = SchannelCred::builder()
                                 .acquire(Direction::Outbound).unwrap();
        let mut stream = tls_stream::Builder::new()
            .domain("localhost")
            .connect(creds, OneByteAtATime { inner: stream })
            .unwrap();
        stream.write_all(&[1, 2, 3, 4]).unwrap();
        stream.flush().unwrap();
        assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
        stream.shutdown().unwrap();
    });

    let stream = listener.accept().unwrap().0;
    let creds = SchannelCred::builder()
                        .cert(cert)
                        .acquire(Direction::Inbound)
                        .unwrap();
    let mut stream = tls_stream::Builder::new()
        .accept(creds, OneByteAtATime { inner: stream })
        .unwrap();
    assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
    stream.write_all(&[1, 2, 3, 4]).unwrap();
    stream.flush().unwrap();
    let mut buf = [0; 1];
    assert_eq!(stream.read(&mut buf).unwrap(), 0);

    t.join().unwrap();
}

#[test]
fn split_cert_key() {
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    let addr = listener.local_addr().unwrap();
    let t = thread::spawn(move || {
        let cert = include_bytes!("../test/cert.der");
        let mut store = Memory::new().unwrap();
        store.add_encoded_certificate(cert).unwrap();
        let store = store.into_store();

        let stream = TcpStream::connect(&addr).unwrap();
        let creds = SchannelCred::builder()
                                 .acquire(Direction::Outbound).unwrap();
        let mut stream = tls_stream::Builder::new()
            .domain("foobar.com")
            .cert_store(store)
            .connect(creds, stream)
            .unwrap();
        stream.write_all(&[1, 2, 3, 4]).unwrap();
        stream.flush().unwrap();
        assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
        stream.shutdown().unwrap();
    });

    let cert = include_bytes!("../test/cert.der");
    let cert = CertContext::new(cert).unwrap();

    let mut options = AcquireOptions::new();
    options.container("schannel-test");
    let type_ = ProviderType::rsa_full();

    let mut container = match options.acquire(type_) {
        Ok(container) => container,
        Err(_) => options.new_keyset(true).acquire(type_).unwrap(),
    };
    let key = include_bytes!("../test/key.key");
    container.import()
        .import(key)
        .unwrap();

    cert.set_key_prov_info()
        .container("schannel-test")
        .type_(type_)
        .keep_open(true)
        .key_spec(KeySpec::key_exchange())
        .set()
        .unwrap();

    let stream = listener.accept().unwrap().0;
    let creds = SchannelCred::builder()
                        .cert(cert)
                        .acquire(Direction::Inbound)
                        .unwrap();
    let mut stream = tls_stream::Builder::new()
        .accept(creds, stream)
        .unwrap();
    assert_eq!(stream.read(&mut [0; 1024]).unwrap(), 4);
    stream.write_all(&[1, 2, 3, 4]).unwrap();
    stream.flush().unwrap();
    let mut buf = [0; 1];
    assert_eq!(stream.read(&mut buf).unwrap(), 0);

    t.join().unwrap();
}
