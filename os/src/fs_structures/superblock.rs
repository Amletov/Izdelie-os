pub struct SuperBlock {
    pub block_size: u16,        // Размер одного блока
    pub first_block: u32,       // Номер первого блока
    pub total_blocks: u64,      // Размер ФС в блоках
    pub name: [char; 10],       // Название ФС
    pub free_blocks: u64,       // Свободный объём хранилища в байтах
    pub block_bitmap_size: u32, // Размер битовой карты св./з. блоков
    pub inode_bitmap_size: u32, // Размер битовой карты св./з. inode
}
