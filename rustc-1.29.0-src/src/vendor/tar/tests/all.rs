extern crate filetime;
extern crate tar;
extern crate tempdir;
#[cfg(all(unix, feature = "xattr"))]
extern crate xattr;

use std::fs::{self, File};
use std::io::prelude::*;
use std::io::{self, Cursor};
use std::iter::repeat;
use std::path::{Path, PathBuf};

use filetime::FileTime;
use self::tempdir::TempDir;
use tar::{Archive, Builder, Header, EntryType};

macro_rules! t {
    ($e:expr) => (match $e {
        Ok(v) => v,
        Err(e) => panic!("{} returned {}", stringify!($e), e),
    })
}

macro_rules! tar {
    ($e:expr) => (&include_bytes!(concat!("archives/", $e))[..])
}

mod header;

#[test]
fn simple() {
    let mut ar = Archive::new(Cursor::new(tar!("simple.tar")));
    for entry in t!(ar.entries()) {
        t!(entry);
    }
    let mut ar = Archive::new(Cursor::new(tar!("simple.tar")));
    for entry in t!(ar.entries()) {
        t!(entry);
    }
}

#[test]
fn header_impls() {
    let mut ar = Archive::new(Cursor::new(tar!("simple.tar")));
    let hn = Header::new_old();
    let hnb = hn.as_bytes();
    for file in t!(ar.entries()) {
        let file = t!(file);
        let h1 = file.header();
        let h1b = h1.as_bytes();
        let h2 = h1.clone();
        let h2b = h2.as_bytes();
        assert!(h1b[..] == h2b[..] && h2b[..] != hnb[..])
    }
}

#[test]
fn reading_files() {
    let rdr = Cursor::new(tar!("reading_files.tar"));
    let mut ar = Archive::new(rdr);
    let mut entries = t!(ar.entries());

    let mut a = t!(entries.next().unwrap());
    assert_eq!(&*a.header().path_bytes(), b"a");
    let mut s = String::new();
    t!(a.read_to_string(&mut s));
    assert_eq!(s, "a\na\na\na\na\na\na\na\na\na\na\n");

    let mut b = t!(entries.next().unwrap());
    assert_eq!(&*b.header().path_bytes(), b"b");
    s.truncate(0);
    t!(b.read_to_string(&mut s));
    assert_eq!(s, "b\nb\nb\nb\nb\nb\nb\nb\nb\nb\nb\n");

    assert!(entries.next().is_none());
}

#[test]
fn writing_files() {
    let mut ar = Builder::new(Vec::new());
    let td = t!(TempDir::new("tar-rs"));

    let path = td.path().join("test");
    t!(t!(File::create(&path)).write_all(b"test"));

    t!(ar.append_file("test2", &mut t!(File::open(&path))));

    let data = t!(ar.into_inner());
    let mut ar = Archive::new(Cursor::new(data));
    let mut entries = t!(ar.entries());
    let mut f = t!(entries.next().unwrap());

    assert_eq!(&*f.header().path_bytes(), b"test2");
    assert_eq!(f.header().size().unwrap(), 4);
    let mut s = String::new();
    t!(f.read_to_string(&mut s));
    assert_eq!(s, "test");

    assert!(entries.next().is_none());
}

#[test]
fn large_filename() {
    let mut ar = Builder::new(Vec::new());
    let td = t!(TempDir::new("tar-rs"));

    let path = td.path().join("test");
    t!(t!(File::create(&path)).write_all(b"test"));

    let filename = repeat("abcd/").take(50).collect::<String>();
    let mut header = Header::new_ustar();
    header.set_path(&filename).unwrap();
    header.set_metadata(&t!(fs::metadata(&path)));
    header.set_cksum();
    t!(ar.append(&header, &b"test"[..]));
    let too_long = repeat("abcd").take(200).collect::<String>();
    t!(ar.append_file(&too_long, &mut t!(File::open(&path))));
    t!(ar.append_data(&mut header, &too_long, &b"test"[..]));

    let rd = Cursor::new(t!(ar.into_inner()));
    let mut ar = Archive::new(rd);
    let mut entries = t!(ar.entries());

    // The short entry added with `append`
    let mut f = entries.next().unwrap().unwrap();
    assert_eq!(&*f.header().path_bytes(), filename.as_bytes());
    assert_eq!(f.header().size().unwrap(), 4);
    let mut s = String::new();
    t!(f.read_to_string(&mut s));
    assert_eq!(s, "test");

    // The long entry added with `append_file`
    let mut f = entries.next().unwrap().unwrap();
    assert_eq!(&*f.path_bytes(), too_long.as_bytes());
    assert_eq!(f.header().size().unwrap(), 4);
    let mut s = String::new();
    t!(f.read_to_string(&mut s));
    assert_eq!(s, "test");

    // The long entry added with `append_data`
    let mut f = entries.next().unwrap().unwrap();
    assert!(f.header().path_bytes().len() < too_long.len());
    assert_eq!(&*f.path_bytes(), too_long.as_bytes());
    assert_eq!(f.header().size().unwrap(), 4);
    let mut s = String::new();
    t!(f.read_to_string(&mut s));
    assert_eq!(s, "test");

    assert!(entries.next().is_none());
}

#[test]
fn reading_entries() {
    let rdr = Cursor::new(tar!("reading_files.tar"));
    let mut ar = Archive::new(rdr);
    let mut entries = t!(ar.entries());
    let mut a = t!(entries.next().unwrap());
    assert_eq!(&*a.header().path_bytes(), b"a");
    let mut s = String::new();
    t!(a.read_to_string(&mut s));
    assert_eq!(s, "a\na\na\na\na\na\na\na\na\na\na\n");
    s.truncate(0);
    t!(a.read_to_string(&mut s));
    assert_eq!(s, "");
    let mut b = t!(entries.next().unwrap());

    assert_eq!(&*b.header().path_bytes(), b"b");
    s.truncate(0);
    t!(b.read_to_string(&mut s));
    assert_eq!(s, "b\nb\nb\nb\nb\nb\nb\nb\nb\nb\nb\n");
    assert!(entries.next().is_none());
}

fn check_dirtree(td: &TempDir) {
    let dir_a = td.path().join("a");
    let dir_b = td.path().join("a/b");
    let file_c = td.path().join("a/c");
    assert!(fs::metadata(&dir_a).map(|m| m.is_dir()).unwrap_or(false));
    assert!(fs::metadata(&dir_b).map(|m| m.is_dir()).unwrap_or(false));
    assert!(fs::metadata(&file_c).map(|m| m.is_file()).unwrap_or(false));
}

#[test]
fn extracting_directories() {
    let td = t!(TempDir::new("tar-rs"));
    let rdr = Cursor::new(tar!("directory.tar"));
    let mut ar = Archive::new(rdr);
    t!(ar.unpack(td.path()));
    check_dirtree(&td);
}

#[test]
#[cfg(all(unix, feature = "xattr"))]
fn xattrs() {
    // If /tmp is a tmpfs, xattr will fail
    // The xattr crate's unit tests also use /var/tmp for this reason
    let td = t!(TempDir::new_in("/var/tmp", "tar-rs"));
    let rdr = Cursor::new(tar!("xattrs.tar"));
    let mut ar = Archive::new(rdr);
    ar.set_unpack_xattrs(true);
    t!(ar.unpack(td.path()));

    let val = xattr::get(td.path().join("a/b"), "user.pax.flags").unwrap();
    assert_eq!(val.unwrap(), "epm".as_bytes());
}

#[test]
#[cfg(all(unix, feature = "xattr"))]
fn no_xattrs() {
    // If /tmp is a tmpfs, xattr will fail
    // The xattr crate's unit tests also use /var/tmp for this reason
    let td = t!(TempDir::new_in("/var/tmp", "tar-rs"));
    let rdr = Cursor::new(tar!("xattrs.tar"));
    let mut ar = Archive::new(rdr);
    ar.set_unpack_xattrs(false);
    t!(ar.unpack(td.path()));

    assert_eq!(xattr::get(td.path().join("a/b"), "user.pax.flags").unwrap(), None);
}

#[test]
fn writing_and_extracting_directories() {
    let td = t!(TempDir::new("tar-rs"));

    let mut ar = Builder::new(Vec::new());
    let tmppath = td.path().join("tmpfile");
    t!(t!(File::create(&tmppath)).write_all(b"c"));
    t!(ar.append_dir("a", "."));
    t!(ar.append_dir("a/b", "."));
    t!(ar.append_file("a/c", &mut t!(File::open(&tmppath))));
    t!(ar.finish());

    let rdr = Cursor::new(t!(ar.into_inner()));
    let mut ar = Archive::new(rdr);
    t!(ar.unpack(td.path()));
    check_dirtree(&td);
}

#[test]
fn writing_directories_recursively() {
    let td = t!(TempDir::new("tar-rs"));

    let base_dir = td.path().join("base");
    t!(fs::create_dir(&base_dir));
    t!(t!(File::create(base_dir.join("file1"))).write_all(b"file1"));
    let sub_dir = base_dir.join("sub");
    t!(fs::create_dir(&sub_dir));
    t!(t!(File::create(sub_dir.join("file2"))).write_all(b"file2"));

    let mut ar = Builder::new(Vec::new());
    t!(ar.append_dir_all("foobar", base_dir));
    let data = t!(ar.into_inner());

    let mut ar = Archive::new(Cursor::new(data));
    t!(ar.unpack(td.path()));
    let base_dir = td.path().join("foobar");
    assert!(fs::metadata(&base_dir).map(|m| m.is_dir()).unwrap_or(false));
    let file1_path = base_dir.join("file1");
    assert!(fs::metadata(&file1_path).map(|m| m.is_file()).unwrap_or(false));
    let sub_dir = base_dir.join("sub");
    assert!(fs::metadata(&sub_dir).map(|m| m.is_dir()).unwrap_or(false));
    let file2_path = sub_dir.join("file2");
    assert!(fs::metadata(&file2_path).map(|m| m.is_file()).unwrap_or(false));
}

#[test]
fn append_dir_all_blank_dest() {
    let td = t!(TempDir::new("tar-rs"));

    let base_dir = td.path().join("base");
    t!(fs::create_dir(&base_dir));
    t!(t!(File::create(base_dir.join("file1"))).write_all(b"file1"));
    let sub_dir = base_dir.join("sub");
    t!(fs::create_dir(&sub_dir));
    t!(t!(File::create(sub_dir.join("file2"))).write_all(b"file2"));

    let mut ar = Builder::new(Vec::new());
    t!(ar.append_dir_all("", base_dir));
    let data = t!(ar.into_inner());

    let mut ar = Archive::new(Cursor::new(data));
    t!(ar.unpack(td.path()));
    let base_dir = td.path();
    assert!(fs::metadata(&base_dir).map(|m| m.is_dir()).unwrap_or(false));
    let file1_path = base_dir.join("file1");
    assert!(fs::metadata(&file1_path).map(|m| m.is_file()).unwrap_or(false));
    let sub_dir = base_dir.join("sub");
    assert!(fs::metadata(&sub_dir).map(|m| m.is_dir()).unwrap_or(false));
    let file2_path = sub_dir.join("file2");
    assert!(fs::metadata(&file2_path).map(|m| m.is_file()).unwrap_or(false));
}

#[test]
fn append_dir_all_does_not_work_on_non_directory() {
    let td = t!(TempDir::new("tar-rs"));
    let path = td.path().join("test");
    t!(t!(File::create(&path)).write_all(b"test"));

    let mut ar = Builder::new(Vec::new());
    let result = ar.append_dir_all("test", path);
    assert!(result.is_err());
}

#[test]
fn extracting_duplicate_dirs() {
    let td = t!(TempDir::new("tar-rs"));
    let rdr = Cursor::new(tar!("duplicate_dirs.tar"));
    let mut ar = Archive::new(rdr);
    t!(ar.unpack(td.path()));

    let some_dir = td.path().join("some_dir");
    assert!(fs::metadata(&some_dir).map(|m| m.is_dir()).unwrap_or(false));
}

#[test]
fn handling_incorrect_file_size() {
    let td = t!(TempDir::new("tar-rs"));

    let mut ar = Builder::new(Vec::new());

    let path = td.path().join("tmpfile");
    t!(File::create(&path));
    let mut file = t!(File::open(&path));
    let mut header = Header::new_old();
    t!(header.set_path("somepath"));
    header.set_metadata(&t!(file.metadata()));
    header.set_size(2048); // past the end of file null blocks
    header.set_cksum();
    t!(ar.append(&header, &mut file));

    // Extracting
    let rdr = Cursor::new(t!(ar.into_inner()));
    let mut ar = Archive::new(rdr);
    assert!(ar.unpack(td.path()).is_err());

    // Iterating
    let rdr = Cursor::new(ar.into_inner().into_inner());
    let mut ar = Archive::new(rdr);
    assert!(t!(ar.entries()).any(|fr| fr.is_err()));
}

#[test]
fn extracting_malicious_tarball() {
    let td = t!(TempDir::new("tar-rs"));

    let mut evil_tar = Vec::new();

    {
        let mut a = Builder::new(&mut evil_tar);
        let mut append = |path: &str| {
            let mut header = Header::new_gnu();
            assert!(header.set_path(path).is_err(),
                    "was ok: {:?}", path);
            {
                let h = header.as_gnu_mut().unwrap();
                for (a, b) in h.name.iter_mut().zip(path.as_bytes()) {
                    *a = *b;
                }
            }
            header.set_size(1);
            header.set_cksum();
            t!(a.append(&header, io::repeat(1).take(1)));
        };
        append("/tmp/abs_evil.txt");
        append("//tmp/abs_evil2.txt");
        append("///tmp/abs_evil3.txt");
        append("/./tmp/abs_evil4.txt");
        append("//./tmp/abs_evil5.txt");
        append("///./tmp/abs_evil6.txt");
        append("/../tmp/rel_evil.txt");
        append("../rel_evil2.txt");
        append("./../rel_evil3.txt");
        append("some/../../rel_evil4.txt");
        append("");
        append("././//./..");
        append("..");
        append("/////////..");
        append("/////////");
    }

    let mut ar = Archive::new(&evil_tar[..]);
    t!(ar.unpack(td.path()));

    assert!(fs::metadata("/tmp/abs_evil.txt").is_err());
    assert!(fs::metadata("/tmp/abs_evil.txt2").is_err());
    assert!(fs::metadata("/tmp/abs_evil.txt3").is_err());
    assert!(fs::metadata("/tmp/abs_evil.txt4").is_err());
    assert!(fs::metadata("/tmp/abs_evil.txt5").is_err());
    assert!(fs::metadata("/tmp/abs_evil.txt6").is_err());
    assert!(fs::metadata("/tmp/rel_evil.txt").is_err());
    assert!(fs::metadata("/tmp/rel_evil.txt").is_err());
    assert!(fs::metadata(td.path().join("../tmp/rel_evil.txt")).is_err());
    assert!(fs::metadata(td.path().join("../rel_evil2.txt")).is_err());
    assert!(fs::metadata(td.path().join("../rel_evil3.txt")).is_err());
    assert!(fs::metadata(td.path().join("../rel_evil4.txt")).is_err());

    // The `some` subdirectory should not be created because the only
    // filename that references this has '..'.
    assert!(fs::metadata(td.path().join("some")).is_err());

    // The `tmp` subdirectory should be created and within this
    // subdirectory, there should be files named `abs_evil.txt` through
    // `abs_evil6.txt`.
    assert!(fs::metadata(td.path().join("tmp")).map(|m| m.is_dir())
               .unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil2.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil3.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil4.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil5.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
    assert!(fs::metadata(td.path().join("tmp/abs_evil6.txt"))
               .map(|m| m.is_file()).unwrap_or(false));
}

#[test]
fn octal_spaces() {
    let rdr = Cursor::new(tar!("spaces.tar"));
    let mut ar = Archive::new(rdr);

    let entry = ar.entries().unwrap().next().unwrap().unwrap();
    assert_eq!(entry.header().mode().unwrap() & 0o777, 0o777);
    assert_eq!(entry.header().uid().unwrap(), 0);
    assert_eq!(entry.header().gid().unwrap(), 0);
    assert_eq!(entry.header().size().unwrap(), 2);
    assert_eq!(entry.header().mtime().unwrap(), 0o12440016664);
    assert_eq!(entry.header().cksum().unwrap(), 0o4253);
}

#[test]
fn extracting_malformed_tar_null_blocks() {
    let td = t!(TempDir::new("tar-rs"));

    let mut ar = Builder::new(Vec::new());

    let path1 = td.path().join("tmpfile1");
    let path2 = td.path().join("tmpfile2");
    t!(File::create(&path1));
    t!(File::create(&path2));
    t!(ar.append_file("tmpfile1", &mut t!(File::open(&path1))));
    let mut data = t!(ar.into_inner());
    let amt = data.len();
    data.truncate(amt - 512);
    let mut ar = Builder::new(data);
    t!(ar.append_file("tmpfile2", &mut t!(File::open(&path2))));
    t!(ar.finish());

    let data = t!(ar.into_inner());
    let mut ar = Archive::new(&data[..]);
    assert!(ar.unpack(td.path()).is_err());
}

#[test]
fn empty_filename()
{
    let td = t!(TempDir::new("tar-rs"));
    let rdr = Cursor::new(tar!("empty_filename.tar"));
    let mut ar = Archive::new(rdr);
    assert!(ar.unpack(td.path()).is_err());
}

#[test]
fn file_times() {
    let td = t!(TempDir::new("tar-rs"));
    let rdr = Cursor::new(tar!("file_times.tar"));
    let mut ar = Archive::new(rdr);
    t!(ar.unpack(td.path()));

    let meta = fs::metadata(td.path().join("a")).unwrap();
    let mtime = FileTime::from_last_modification_time(&meta);
    let atime = FileTime::from_last_access_time(&meta);
    assert_eq!(mtime.unix_seconds(), 1000000000);
    assert_eq!(mtime.nanoseconds(), 0);
    assert_eq!(atime.unix_seconds(), 1000000000);
    assert_eq!(atime.nanoseconds(), 0);
}

#[test]
fn backslash_treated_well() {
    // Insert a file into an archive with a backslash
    let td = t!(TempDir::new("tar-rs"));
    let mut ar = Builder::new(Vec::<u8>::new());
    t!(ar.append_dir("foo\\bar", td.path()));
    let mut ar = Archive::new(Cursor::new(t!(ar.into_inner())));
    let f = t!(t!(ar.entries()).next().unwrap());
    if cfg!(unix) {
        assert_eq!(t!(f.header().path()).to_str(), Some("foo\\bar"));
    } else {
        assert_eq!(t!(f.header().path()).to_str(), Some("foo/bar"));
    }

    // Unpack an archive with a backslash in the name
    let mut ar = Builder::new(Vec::<u8>::new());
    let mut header = Header::new_gnu();
    header.set_metadata(&t!(fs::metadata(td.path())));
    header.set_size(0);
    for (a, b) in header.as_old_mut().name.iter_mut().zip(b"foo\\bar\x00") {
        *a = *b;
    }
    header.set_cksum();
    t!(ar.append(&header, &mut io::empty()));
    let data = t!(ar.into_inner());
    let mut ar = Archive::new(&data[..]);
    let f = t!(t!(ar.entries()).next().unwrap());
    assert_eq!(t!(f.header().path()).to_str(), Some("foo\\bar"));

    let mut ar = Archive::new(&data[..]);
    t!(ar.unpack(td.path()));
    assert!(fs::metadata(td.path().join("foo\\bar")).is_ok());
}

#[cfg(unix)]
#[test]
fn nul_bytes_in_path() {
    use std::os::unix::prelude::*;
    use std::ffi::OsStr;

    let nul_path = OsStr::from_bytes(b"foo\0");
    let td = t!(TempDir::new("tar-rs"));
    let mut ar = Builder::new(Vec::<u8>::new());
    let err = ar.append_dir(nul_path, td.path()).unwrap_err();
    assert!(err.to_string().contains("contains a nul byte"));
}

#[test]
fn links() {
    let mut ar = Archive::new(Cursor::new(tar!("link.tar")));
    let mut entries = t!(ar.entries());
    let link = t!(entries.next().unwrap());
    assert_eq!(t!(link.header().link_name()).as_ref().map(|p| &**p),
               Some(Path::new("file")));
    let other = t!(entries.next().unwrap());
    assert!(t!(other.header().link_name()).is_none());
}

#[test]
#[cfg(unix)] // making symlinks on windows is hard
fn unpack_links() {
    let td = t!(TempDir::new("tar-rs"));
    let mut ar = Archive::new(Cursor::new(tar!("link.tar")));
    t!(ar.unpack(td.path()));

    let md = t!(fs::symlink_metadata(td.path().join("lnk")));
    assert!(md.file_type().is_symlink());
    assert_eq!(&*t!(fs::read_link(td.path().join("lnk"))),
               Path::new("file"));
    t!(File::open(td.path().join("lnk")));
}

#[test]
fn pax_simple() {
    let mut ar = Archive::new(tar!("pax.tar"));
    let mut entries = t!(ar.entries());

    let mut first = t!(entries.next().unwrap());
    let mut attributes = t!(first.pax_extensions()).unwrap();
    let first = t!(attributes.next().unwrap());
    let second = t!(attributes.next().unwrap());
    let third = t!(attributes.next().unwrap());
    assert!(attributes.next().is_none());

    assert_eq!(first.key(), Ok("mtime"));
    assert_eq!(first.value(), Ok("1453146164.953123768"));
    assert_eq!(second.key(), Ok("atime"));
    assert_eq!(second.value(), Ok("1453251915.24892486"));
    assert_eq!(third.key(), Ok("ctime"));
    assert_eq!(third.value(), Ok("1453146164.953123768"));
}

#[test]
fn pax_path() {
    let mut ar = Archive::new(tar!("pax2.tar"));
    let mut entries = t!(ar.entries());

    let first = t!(entries.next().unwrap());
    assert!(first.path().unwrap().ends_with("aaaaaaaaaaaaaaa"));
}

#[test]
fn long_name_trailing_nul() {
    let mut b = Builder::new(Vec::<u8>::new());

    let mut h = Header::new_gnu();
    t!(h.set_path("././@LongLink"));
    h.set_size(4);
    h.set_entry_type(EntryType::new(b'L'));
    h.set_cksum();
    t!(b.append(&h, "foo\0".as_bytes()));
    let mut h = Header::new_gnu();

    t!(h.set_path("bar"));
    h.set_size(6);
    h.set_entry_type(EntryType::file());
    h.set_cksum();
    t!(b.append(&h, "foobar".as_bytes()));

    let contents = t!(b.into_inner());
    let mut a = Archive::new(&contents[..]);

    let e = t!(t!(a.entries()).next().unwrap());
    assert_eq!(&*e.path_bytes(), b"foo");
}

#[test]
fn long_linkname_trailing_nul() {
    let mut b = Builder::new(Vec::<u8>::new());

    let mut h = Header::new_gnu();
    t!(h.set_path("././@LongLink"));
    h.set_size(4);
    h.set_entry_type(EntryType::new(b'K'));
    h.set_cksum();
    t!(b.append(&h, "foo\0".as_bytes()));
    let mut h = Header::new_gnu();

    t!(h.set_path("bar"));
    h.set_size(6);
    h.set_entry_type(EntryType::file());
    h.set_cksum();
    t!(b.append(&h, "foobar".as_bytes()));

    let contents = t!(b.into_inner());
    let mut a = Archive::new(&contents[..]);

    let e = t!(t!(a.entries()).next().unwrap());
    assert_eq!(&*e.link_name_bytes().unwrap(), b"foo");
}

#[test]
fn encoded_long_name_has_trailing_nul() {
    let td = t!(TempDir::new("tar-rs"));
    let path = td.path().join("foo");
    t!(t!(File::create(&path)).write_all(b"test"));

    let mut b = Builder::new(Vec::<u8>::new());
    let long = repeat("abcd").take(200).collect::<String>();

    t!(b.append_file(&long, &mut t!(File::open(&path))));

    let contents = t!(b.into_inner());
    let mut a = Archive::new(&contents[..]);

    let mut e = t!(t!(a.entries()).raw(true).next().unwrap());
    let mut name = Vec::new();
    t!(e.read_to_end(&mut name));
    assert_eq!(name[name.len() - 1], 0);

    let header_name = &e.header().as_gnu().unwrap().name;
    assert!(header_name.starts_with(b"././@LongLink\x00"));
}

#[test]
fn reading_sparse() {
    let rdr = Cursor::new(tar!("sparse.tar"));
    let mut ar = Archive::new(rdr);
    let mut entries = t!(ar.entries());

    let mut a = t!(entries.next().unwrap());
    let mut s = String::new();
    assert_eq!(&*a.header().path_bytes(), b"sparse_begin.txt");
    t!(a.read_to_string(&mut s));
    assert_eq!(&s[..5], "test\n");
    assert!(s[5..].chars().all(|x| x == '\u{0}'));

    let mut a = t!(entries.next().unwrap());
    let mut s = String::new();
    assert_eq!(&*a.header().path_bytes(), b"sparse_end.txt");
    t!(a.read_to_string(&mut s));
    assert!(s[..s.len() - 9].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[s.len() - 9..], "test_end\n");

    let mut a = t!(entries.next().unwrap());
    let mut s = String::new();
    assert_eq!(&*a.header().path_bytes(), b"sparse_ext.txt");
    t!(a.read_to_string(&mut s));
    assert!(s[..0x1000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x1000..0x1000+5], "text\n");
    assert!(s[0x1000+5..0x3000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x3000..0x3000+5], "text\n");
    assert!(s[0x3000+5..0x5000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x5000..0x5000+5], "text\n");
    assert!(s[0x5000+5..0x7000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x7000..0x7000+5], "text\n");
    assert!(s[0x7000+5..0x9000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x9000..0x9000+5], "text\n");
    assert!(s[0x9000+5..0xb000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0xb000..0xb000+5], "text\n");

    let mut a = t!(entries.next().unwrap());
    let mut s = String::new();
    assert_eq!(&*a.header().path_bytes(), b"sparse.txt");
    t!(a.read_to_string(&mut s));
    assert!(s[..0x1000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x1000..0x1000+6], "hello\n");
    assert!(s[0x1000+6..0x2fa0].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x2fa0..0x2fa0+6], "world\n");
    assert!(s[0x2fa0+6..0x4000].chars().all(|x| x == '\u{0}'));

    assert!(entries.next().is_none());
}

#[test]
fn extract_sparse() {
    let rdr = Cursor::new(tar!("sparse.tar"));
    let mut ar = Archive::new(rdr);
    let td = t!(TempDir::new("tar-rs"));
    t!(ar.unpack(td.path()));

    let mut s = String::new();
    t!(t!(File::open(td.path().join("sparse_begin.txt"))).read_to_string(&mut s));
    assert_eq!(&s[..5], "test\n");
    assert!(s[5..].chars().all(|x| x == '\u{0}'));

    s.truncate(0);
    t!(t!(File::open(td.path().join("sparse_end.txt"))).read_to_string(&mut s));
    assert!(s[..s.len() - 9].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[s.len() - 9..], "test_end\n");

    s.truncate(0);
    t!(t!(File::open(td.path().join("sparse_ext.txt"))).read_to_string(&mut s));
    assert!(s[..0x1000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x1000..0x1000+5], "text\n");
    assert!(s[0x1000+5..0x3000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x3000..0x3000+5], "text\n");
    assert!(s[0x3000+5..0x5000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x5000..0x5000+5], "text\n");
    assert!(s[0x5000+5..0x7000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x7000..0x7000+5], "text\n");
    assert!(s[0x7000+5..0x9000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x9000..0x9000+5], "text\n");
    assert!(s[0x9000+5..0xb000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0xb000..0xb000+5], "text\n");

    s.truncate(0);
    t!(t!(File::open(td.path().join("sparse.txt"))).read_to_string(&mut s));
    assert!(s[..0x1000].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x1000..0x1000+6], "hello\n");
    assert!(s[0x1000+6..0x2fa0].chars().all(|x| x == '\u{0}'));
    assert_eq!(&s[0x2fa0..0x2fa0+6], "world\n");
    assert!(s[0x2fa0+6..0x4000].chars().all(|x| x == '\u{0}'));
}

#[test]
fn path_separators() {
    let mut ar = Builder::new(Vec::new());
    let td = t!(TempDir::new("tar-rs"));

    let path = td.path().join("test");
    t!(t!(File::create(&path)).write_all(b"test"));

    let short_path: PathBuf = repeat("abcd").take(2).collect();
    let long_path: PathBuf = repeat("abcd").take(50).collect();

    // Make sure UStar headers normalize to Unix path separators
    let mut header = Header::new_ustar();

    t!(header.set_path(&short_path));
    assert_eq!(t!(header.path()), short_path);
    assert!(!header.path_bytes().contains(&b'\\'));

    t!(header.set_path(&long_path));
    assert_eq!(t!(header.path()), long_path);
    assert!(!header.path_bytes().contains(&b'\\'));

    // Make sure GNU headers normalize to Unix path separators,
    // including the `@LongLink` fallback used by `append_file`.
    t!(ar.append_file(&short_path, &mut t!(File::open(&path))));
    t!(ar.append_file(&long_path, &mut t!(File::open(&path))));

    let rd = Cursor::new(t!(ar.into_inner()));
    let mut ar = Archive::new(rd);
    let mut entries = t!(ar.entries());

    let entry = t!(entries.next().unwrap());
    assert_eq!(t!(entry.path()), short_path);
    assert!(!entry.path_bytes().contains(&b'\\'));

    let entry = t!(entries.next().unwrap());
    assert_eq!(t!(entry.path()), long_path);
    assert!(!entry.path_bytes().contains(&b'\\'));

    assert!(entries.next().is_none());
}

#[test]
#[cfg(unix)]
fn append_path_symlink() {
    use std::os::unix::fs::symlink;
    use std::env;
    use std::borrow::Cow;

    let mut ar = Builder::new(Vec::new());
    ar.follow_symlinks(false);
    let td = t!(TempDir::new("tar-rs"));

    t!(env::set_current_dir(td.path()));
    t!(symlink("testdest", "test"));
    t!(ar.append_path("test"));

    let rd = Cursor::new(t!(ar.into_inner()));
    let mut ar = Archive::new(rd);
    let mut entries = t!(ar.entries());

    let entry = t!(entries.next().unwrap());
    assert_eq!(t!(entry.path()), Path::new("test"));
    assert_eq!(t!(entry.link_name()), Some(Cow::from(Path::new("testdest"))));
    assert_eq!(t!(entry.header().size()), 0);

    assert!(entries.next().is_none());
}
