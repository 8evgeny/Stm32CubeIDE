#include "eeprom.h"
#include <string.h>
#include "main.h"
extern void UART_Printf(const char* fmt, ...);
extern int AT24C_ReadBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count);
extern int AT24C_WriteBytes (uint16_t addr,uint8_t *buf, uint16_t bytes_count);

__IO uint16_t  EEPROMDataRead;
__IO uint8_t   EEPROMDataWrite;
// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;
extern I2C_HandleTypeDef hi2c1;
//Изменил параметры EEPROM
uint8_t littlefs_mem[32768];
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
//    UART_Printf("*** device_read ***\n"); delayUS_ASM(2000);
    uint32_t addr = (uint32_t)(block * c->block_size + off);
    uint16_t NumByteToRead = (uint16_t)size;
    uint8_t *buffer_data = (uint8_t *)buffer;
//    AT24C_ReadBytes(addr, buffer_data, NumByteToRead);
//    return 0;
    return BSP_EEPROM_ReadBuffer(buffer_data, addr, &NumByteToRead);
}

int user_provided_block_device_prog(const struct lfs_config *c,
                                    lfs_block_t block, lfs_off_t off,
                                    const void *buffer, lfs_size_t size)
{
//    UART_Printf("*** device_prog ***\n"); delayUS_ASM(2000);
    uint32_t addr = (uint32_t)(block * c->block_size + off);
    uint8_t *buffer_data = (uint8_t *)buffer;
//    AT24C_WriteBytes(addr, buffer_data, size);
//    return 0;
    return BSP_EEPROM_WriteBuffer(buffer_data, addr, size);
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
    printf("\nLittleFsInit\n");
// release any resources we were using
    lfs_unmount(&lfs);
// example littlefs
    cfg.read = user_provided_block_device_read;
    cfg.prog = user_provided_block_device_prog;
    cfg.erase = user_provided_block_device_erase;
    cfg.sync = user_provided_block_device_sync;
//Параметры lfs для IPS
//    cfg.read_size = 256;
//    cfg.prog_size = 256;
//    cfg.block_size = 1024;
//    cfg.block_count = 32;
//    cfg.lookahead = 256;

//Работает
    cfg.read_size = 64;
    cfg.prog_size = 64;
    cfg.block_size = 64; //Size of an erasable block
    cfg.block_count = 512; // Number of erasable blocks on the device
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
    char fileToEEPROM_2[] = "1234567890asdfghjklzxcv";
    char fileFromEEPROM_2[sizeof (fileToEEPROM_2)];

    lfs_file_open(&lfs, &file, "testEEPROM__", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &fileToEEPROM, sizeof(fileToEEPROM));
    lfs_file_close(&lfs, &file);
    lfs_file_open(&lfs, &file, "testEEPROM__", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &fileFromEEPROM, sizeof(fileFromEEPROM));
    lfs_file_close(&lfs, &file);

    lfs_file_open(&lfs, &file, "testEEPROM5", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &fileToEEPROM_2, sizeof(fileToEEPROM_2));
    lfs_file_close(&lfs, &file);
    lfs_file_open(&lfs, &file, "testEEPROM5", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &fileFromEEPROM_2, sizeof(fileFromEEPROM_2));
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
        UART_Printf("FAILED\r\n"); delayUS_ASM(10000);
    if(!error)
        UART_Printf("OK\r\n"); delayUS_ASM(10000);

}

__weak void BSP_EEPROM_TIMEOUT_UserCallback(void)
{
}

static uint32_t EEPROM_WaitEepromStandbyState(void)
{
    /* Check if the maximum allowed number of trials has bee reached */
    if (EEPROM_IO_IsDeviceReady(EEPROM_I2C_ADDRESS, EEPROM_MAX_TRIALS) != HAL_OK) {
        /* If the maximum number of trials has been reached, exit the function */
        BSP_EEPROM_TIMEOUT_UserCallback();
        return EEPROM_TIMEOUT;
    }
    return EEPROM_OK;
}

static uint32_t EEPROM_WritePage(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t *NumByteToWrite)
{
    uint32_t buffersize = *NumByteToWrite;
    uint32_t status = EEPROM_OK;

    /* Set the pointer to the Number of data to be written */
    EEPROMDataWrite = *NumByteToWrite;
    if (EEPROM_IO_WriteData(EEPROM_I2C_ADDRESS, WriteAddr, pBuffer, buffersize) != HAL_OK)
    {
        BSP_EEPROM_TIMEOUT_UserCallback();
        status = EEPROM_FAIL;
    }


    while (EEPROM_WaitEepromStandbyState() != EEPROM_OK) {
        return EEPROM_FAIL;
    }


    /* If all operations OK, return EEPROM_OK (0) */
    return status;
}

HAL_StatusTypeDef readData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
        HAL_Delay(10);
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, HAL_MAX_DELAY);

//    while(i2cReadReady != SET); //Выпилил
//    i2cReadReady = RESET;

    return status;
}

HAL_StatusTypeDef writeData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{

    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, Size, HAL_MAX_DELAY);
    for(;;)
    { // wait...
        status = HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, 1, HAL_MAX_DELAY);
        if(status == HAL_OK)
            break;
    }

//    while( i2cWriteReady != SET); //Выпилил
//    i2cWriteReady = RESET;

    return status;
}

HAL_StatusTypeDef EEPROM_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
    HAL_StatusTypeDef status = readData(DevAddress, MemAddress, pBuffer, BufferSize);
    return status;
}

HAL_StatusTypeDef EEPROM_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
    HAL_StatusTypeDef status = writeData(DevAddress, MemAddress, pBuffer, BufferSize);
    return status;
}

uint32_t BSP_EEPROM_ReadBuffer(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t *NumByteToRead)
{
    uint32_t buffersize = *NumByteToRead;

    /* Set the pointer to the Number of data to be read */
    EEPROMDataRead = *NumByteToRead;

    if (EEPROM_IO_ReadData(EEPROM_I2C_ADDRESS, ReadAddr, pBuffer, buffersize) != HAL_OK)
    {

        BSP_EEPROM_TIMEOUT_UserCallback();
        return EEPROM_FAIL;
    }

    /* If all operations OK, return EEPROM_OK (0) */
    return EEPROM_OK;
}

uint32_t BSP_EEPROM_WriteBuffer(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{

    uint8_t  numofpage = 0, numofsingle = 0, count = 0;
    uint16_t addr = 0;
    uint8_t  dataindex = 0;
    uint32_t status = EEPROM_OK;

    addr = WriteAddr % EEPROM_PAGESIZE;
    count = EEPROM_PAGESIZE - addr;
    numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
    numofsingle = NumByteToWrite % EEPROM_PAGESIZE;

    /* If WriteAddr is EEPROM_PAGESIZE aligned */
    if (addr == 0) {
        /* If NumByteToWrite < EEPROM_PAGESIZE */
        if (numofpage == 0) {
            /* Store the number of data to be written */
            dataindex = numofsingle;
            /* Start writing data */
            status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
            if (status != EEPROM_OK) {
                return status;
            }
        }
        /* If NumByteToWrite > EEPROM_PAGESIZE */
        else {
            while (numofpage--) {
                /* Store the number of data to be written */
                dataindex = EEPROM_PAGESIZE;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }

                WriteAddr +=  EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }

            if (numofsingle != 0) {
                /* Store the number of data to be written */
                dataindex = numofsingle;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
            }
        }
    }
    /* If WriteAddr is not EEPROM_PAGESIZE aligned */
    else {
        /* If NumByteToWrite < EEPROM_PAGESIZE */
        if (numofpage == 0) {
            /* If the number of data to be written is more than the remaining space
            in the current page: */
            if (NumByteToWrite > count) {
                /* Store the number of data to be written */
                dataindex = count;
                /* Write the data contained in same page */
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }

                /* Store the number of data to be written */
                dataindex = (NumByteToWrite - count);
                /* Write the remaining data in the following page */
                status = EEPROM_WritePage((uint8_t *)(pBuffer + count), (WriteAddr + count), (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
            } else {
                /* Store the number of data to be written */
                dataindex = numofsingle;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
            }
        }
        /* If NumByteToWrite > EEPROM_PAGESIZE */
        else {
            NumByteToWrite -= count;
            numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
            numofsingle = NumByteToWrite % EEPROM_PAGESIZE;

            if (count != 0) {
                /* Store the number of data to be written */
                dataindex = count;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
                WriteAddr += count;
                pBuffer += count;
            }

            while (numofpage--) {
                /* Store the number of data to be written */
                dataindex = EEPROM_PAGESIZE;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
                WriteAddr +=  EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }
            if (numofsingle != 0) {
                /* Store the number of data to be written */
                dataindex = numofsingle;
                status = EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t *)(&dataindex));
                if (status != EEPROM_OK) {
                    return status;
                }
            }
        }
    }

    /* If all operations OK, return EEPROM_OK (0) */
    return EEPROM_OK;
}

HAL_StatusTypeDef EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, 300);
    return status;
}
