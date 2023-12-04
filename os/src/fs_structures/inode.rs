use chrono::prelude::*;
use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
pub struct Inode {
    pub mode: u16,
    pub uid: u16,
    pub size: u32,
    pub mtime: u64,
    pub ctime: u64,
    pub file_bloks: u32,
    pub addreses: [u32; 9],
}

impl Inode {
    pub fn new(mode: u16, uid: u16, size: u32, file_bloks: u32, addreses: [u32; 9]) -> Self {
        let time_now = Local::now().timestamp() as u64;
        Self {
            mode,
            uid,
            size,
            mtime: time_now,
            ctime: time_now,
            file_bloks,
            addreses
        }
    }

    
}