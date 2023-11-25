mod fs_structures;

use fs_structures::inode::Inode;

use chrono::prelude::*;
use std::time::{UNIX_EPOCH, Duration};

fn timestamp_u64_to_str(time: &u64) -> String {
    let d = UNIX_EPOCH + Duration::from_secs(*time);
    let datetime = DateTime::<Local>::from(d);

    datetime.format("%Y-%m-%d %H:%M:%S").to_string()
}

fn main() {
    let inode = Inode::new(0, 0, 0, 0, [0; 9]);
    let timestamp_str = timestamp_u64_to_str(&inode.ctime);

    println!{"{}",timestamp_str};
}
