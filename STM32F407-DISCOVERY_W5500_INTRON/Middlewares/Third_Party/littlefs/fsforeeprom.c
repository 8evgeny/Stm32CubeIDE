#include "fsforeeprom.h"
#include <string.h>
#include "main.h"
extern void UART_Printf(const char* fmt, ...);
extern int AT24C_ReadBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count);
extern int AT24C_WriteBytes (uint16_t addr,uint8_t *buf, uint16_t bytes_count);

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

//Изменил параметры EEPROM
uint8_t littlefs_mem[8192];
//uint8_t littlefs_mem[131072];
//The 256K is internally organized as 512 pages of 64-bytes each
uint8_t lfs_read_buf[64];
uint8_t lfs_prog_buf[64];
uint8_t lfs_lookahead_buf[16];  // 128/8=16
uint8_t lfs_file_buf[64];

// configuration of the filesystem is provided by this struct
struct lfs_config cfg;

int user_provided_block_device_read(const struct lfs_config *c,
                                    lfs_block_t block, lfs_off_t off,
                                    void *buffer, lfs_size_t size)
{
//    UART_Printf("*** user_provided_block_device_read ***"); delayUS_ASM(10000);
    uint32_t addr = (uint32_t)(block * c->block_size + off);
    uint16_t NumByteToRead = (uint16_t)size;
    uint8_t *buffer_data = (uint8_t *)buffer;
    AT24C_ReadBytes(addr, buffer_data, NumByteToRead);
    return 0;
}

int user_provided_block_device_prog(const struct lfs_config *c,
                                    lfs_block_t block, lfs_off_t off,
                                    const void *buffer, lfs_size_t size)
{
//    UART_Printf("*** user_provided_block_device_prog ***"); delayUS_ASM(10000);
    uint32_t addr = (uint32_t)(block * c->block_size + off);
    uint8_t *buffer_data = (uint8_t *)buffer;
    AT24C_WriteBytes(addr, buffer_data, size);
    return 0;
}

int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
//    UART_Printf("*** user_provided_block_device_erase ***"); delayUS_ASM(10000);
    uint16_t i;
    uint32_t addr = (uint32_t)( block * c->block_size);
    memset((void *)littlefs_mem[addr], c->block_size, 0);
    for (i = 0; i < c->block_size; i++) littlefs_mem[addr + i] = 0;
    return 0;
}

int user_provided_block_device_sync(const struct lfs_config *c)
{

    return 0;
}

void littleFsInit()
{

// release any resources we were using
    lfs_unmount(&lfs);
// example littlefs
    cfg.read = user_provided_block_device_read;
    cfg.prog = user_provided_block_device_prog;
    cfg.erase = user_provided_block_device_erase;
    cfg.sync = user_provided_block_device_sync;
//Параметры lfs похожи на правильные
//    cfg.read_size = 256;
//    cfg.prog_size = 256;
//    cfg.block_size = 1024;
//    cfg.block_count = 32;
//    cfg.lookahead = 256;
    cfg.read_size = 64;
    cfg.prog_size = 64;
    cfg.block_size = 512;
    cfg.block_count = 64;
    cfg.lookahead = 64;


    cfg.read_buffer = lfs_read_buf;
    cfg.prog_buffer = lfs_prog_buf;
    cfg.lookahead_buffer = lfs_lookahead_buf;
    cfg.file_buffer = lfs_file_buf;
// mount the filesystem
    int err = lfs_mount(&lfs, &cfg);
// reformat if we can't mount the filesystem
// this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }
}

void FsForEeprom_test()
{
    char fileToEEPROM[] = "Red_on_top_Green_below._Red_says_Stop_Green_says_Go";
    char fileFromEEPROM[sizeof (fileToEEPROM)];
    char fileToEEPROM_2[] = "1234567890-=qwertyuiop[]asdfghjk";
    char fileFromEEPROM_2[sizeof (fileToEEPROM)];

    lfs_file_open(&lfs, &file, "testEEPROM", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &fileToEEPROM, sizeof(fileToEEPROM));
    lfs_file_close(&lfs, &file);
    lfs_file_open(&lfs, &file, "testEEPROM", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &fileFromEEPROM, sizeof(fileFromEEPROM));
    lfs_file_close(&lfs, &file);

    lfs_file_open(&lfs, &file, "testEEPROM_", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &fileToEEPROM_2, sizeof(fileToEEPROM));
    lfs_file_close(&lfs, &file);
    lfs_file_open(&lfs, &file, "testEEPROM_", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &fileFromEEPROM_2, sizeof(fileFromEEPROM));
    lfs_file_close(&lfs, &file);

    bool error = false;
    for (unsigned int i = 0; i < sizeof(fileToEEPROM); ++i)
    {
        if (fileToEEPROM[i] != fileFromEEPROM[i]) error = true;
    }
    for (unsigned int i = 0; i < sizeof(fileToEEPROM_2); ++i)
    {
        if (fileToEEPROM_2[i] != fileFromEEPROM_2[i]) error = true;
    }
    if(error)
        UART_Printf("test EEPROM FAILED"); delayUS_ASM(10000);
    if(!error)
        UART_Printf("** FsForEeprom_test OK **"); delayUS_ASM(10000);

}



