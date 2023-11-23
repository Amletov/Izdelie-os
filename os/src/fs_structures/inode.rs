pub struct Inode {
    pub mode: u16,
    pub uid: u16,
    pub size: u32,
    pub mtime: u64,
    pub ctime: u64,
    pub file_bloks: u32,
    pub addreses: [u32; 9],
}