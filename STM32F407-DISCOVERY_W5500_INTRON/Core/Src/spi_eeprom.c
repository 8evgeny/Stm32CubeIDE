#include "spi_eeprom.h"

uint8_t bufRead[5];
uint8_t EEPROM_StatusByte;

SPI_HandleTypeDef * EEPROM_SPI;// Define 25LC1024 SPI pointer handle

/**
   * Initialization EEPROM Communication SPI
 * @brief Init EEPROM SPI
 *
 * @param hspi Pointer to SPI struct handler
 */
void EEPROM_SPI_INIT(SPI_HandleTypeDef * hspi)
{
    EEPROM_SPI = hspi;
}

/**
     * Start reading the specified length data at the specified address
  * @brief  Reads a block of data from the EEPROM(READ sequence).
  *
  * @param  pBuffer: pointer to the buffer that receives the data read from the EEPROM.
  * @param  ReadAddr: EEPROM's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the EEPROM.
  * @retval None
  */
EEPROMStatus EEPROM_SPI_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    while (EEPROM_SPI->State != HAL_SPI_STATE_READY) {// Wait for SPI initialization
        HAL_Delay(1);
    }
    /*
        We gonna send all commands in one packet of 4 bytes
     */
    uint8_t header[4];

    header[0] = EEPROM_READ;           // Send "Read from Memory" instruction
    // send 24-bit Address (maximum address 1fffh)
    header[1] = (ReadAddr >> 16)&0xFF; // Send high 8-bit address
    header[2] = (ReadAddr >> 8 )&0xFF; // Send midlle 8-bit address
    header[3] =  ReadAddr       &0xFF; // Send low 8-bit address

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    /* Send WriteAddr address byte to read from */
    EEPROM_SPI_SendInstruction(header, 4);

    while (HAL_SPI_Receive(EEPROM_SPI, (uint8_t*)pBuffer, NumByteToRead, 1000) == HAL_BUSY) {
        HAL_Delay(1);
    };

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    return EEPROM_STATUS_COMPLETE;
}
/**
     * Start the data that is written to the specified length in the specified address
  * @brief  Writes block of data to the EEPROM. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.(WRITE sequence)
  *
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr: EEPROM's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EEPROM.
  * @retval EEPROMOperations value: EEPROM_STATUS_COMPLETE or EEPROM_STATUS_ERROR
  */
EEPROMStatus EEPROM_SPI_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    uint16_t sEE_DataNum = 0;

    EEPROMStatus pageWriteStatus = EEPROM_STATUS_PENDING;

    Addr = WriteAddr % EEPROM_PAGESIZE;// Used to determine if the start address is aligned with the page address
    count = EEPROM_PAGESIZE - Addr;// Offset number on the data page
    NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;// Data page number
    NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;// Number of additional data pages

    if (Addr == 0) { /* WriteAddr is EEPROM_PAGESIZE Aligned address alignment (256 multiples) */
        if (NumOfPage == 0) { /* NumByTetowrite <= EEPROM_PAGESIZE data less than 256 */
            sEE_DataNum = NumByteToWrite;
            pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);// No more than 1 page data, call the page write function

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }

        } else { /* NumByTetowrite> EEPROM_PAGESIZE data volume exceeds page data volume (256) */
            while (NumOfPage--) {// 1, first write the entire page data
                sEE_DataNum = EEPROM_PAGESIZE;
                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);// Call the page write function

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr +=  EEPROM_PAGESIZE;// Address Add page size 256
                pBuffer += EEPROM_PAGESIZE;// Data cache address plus page size 256
            }
            if (NumOfSingle != 0){// Judgment whether there is still the remaining data
            sEE_DataNum = NumOfSingle;// 2, write the additional data
            pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);
            }
            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }
        }
    } else { /* WRITEADDR IS NOT EEPROM_PAGESIZE Aligned address is not aligned (not 256 multiple) */
        if (NumOfPage == 0) { /* NumbyTetowrite <EEPROM_PAGESize data amount less than 256 */
            if (NumOfSingle > count) { /* (NumbyToWrite + Writeaddr)> EEPROM_PAGESIZE The remaining number is greater than the offset address */
                sEE_DataNum = count;
                 /* Write data in the remaining address of the current address page, the current address page is full */
                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr +=  count;
                pBuffer += count;

                sEE_DataNum = NumOfSingle - count;
                /* Write the remaining data in the current address */
                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);
            } else {// Address current page remaining space to write data
                sEE_DataNum = NumByteToWrite;
                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);
            }

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }
        } else { /* NumByTetowrite> EEPROM_PAGESIZE write quantity is greater than 256 */
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
            NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

            sEE_DataNum = count;
            /* Write data in the remaining address of the current address page, the current address page is full */
            pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);

            if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                return pageWriteStatus;
            }

            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--) {// 1, when the remaining number is greater than 256, first write the entire page data
                sEE_DataNum = EEPROM_PAGESIZE;

                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }

                WriteAddr +=  EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }

            if (NumOfSingle != 0) {// 2, less than 1 page data part, continue writing
                sEE_DataNum = NumOfSingle;

                pageWriteStatus = EEPROM_SPI_WritePage(pBuffer, WriteAddr, sEE_DataNum);

                if (pageWriteStatus != EEPROM_STATUS_COMPLETE) {
                    return pageWriteStatus;
                }
            }
        }
    }

    return EEPROM_STATUS_COMPLETE;
}

 /** Write the entire page (256byte) data function
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle
  *         (Page WRITE sequence).
  *
  * @note   The number of byte can't exceed the EEPROM page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EEPROM.
  * @param  WriteAddr: EEPROM's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EEPROM, must be equal
  *         or less than "EEPROM_PAGESIZE" value.
  * @retval EEPROMOperations value: EEPROM_STATUS_COMPLETE or EEPROM_STATUS_ERROR
  */

EEPROMStatus EEPROM_SPI_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    while (EEPROM_SPI->State != HAL_SPI_STATE_READY) {
        HAL_Delay(1);
    }
    HAL_StatusTypeDef spiTransmitStatus;// SPI transmission status

    EEPROM_WriteEnable();// write enable
    /*
        We gonna send commands in one packet of 4 bytes
     */
    uint8_t header[4];
    header[0] = EEPROM_WRITE;    // Send "Write to Memory" instruction
    // send 24-bit Address (maximum address 1fffh)
    header[1] = (WriteAddr >> 16)&0xFF;
    header[2] = (WriteAddr >> 8 )&0xFF;
    header[3] =  WriteAddr       &0xFF;

    EEPROM_WriteEnable();// write enable

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();
    EEPROM_SPI_SendInstruction((uint8_t*)header, 4);
    // Make 5 attemtps to write the data
    for (uint8_t i = 0; i < 5; i++) {
        spiTransmitStatus = HAL_SPI_Transmit(EEPROM_SPI, pBuffer, NumByteToWrite, 1000);
        if (spiTransmitStatus == HAL_BUSY) {
            HAL_Delay(5);
        } else {
            break;
        }
    }

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();
    // Wait the end of EEPROM writing
    EEPROM_SPI_WaitStandbyState();// Waiting for write to complete

    // Disable the write access to the EEPROM
    EEPROM_WriteDisable();

    if (spiTransmitStatus == HAL_ERROR) {
        return EEPROM_STATUS_ERROR;
    } else {
        return EEPROM_STATUS_COMPLETE;
    }
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
 * From the spi bus. Send a byte to receive one byte
  *
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t EEPROM_SendByte(uint8_t byte)
{
    uint8_t answerByte;

    /* Loop while DR register in not empty */
    while (EEPROM_SPI->State == HAL_SPI_STATE_RESET) {
        HAL_Delay(1);
    }

    /* Send byte through the SPI peripheral */
    if (HAL_SPI_Transmit(EEPROM_SPI, &byte, 1, 10000) != HAL_OK) {
        Error_Handler();
    }

    /* Wait to receive a byte */
    while (EEPROM_SPI->State == HAL_SPI_STATE_RESET) {
        HAL_Delay(1);
    }

    /* Return the byte read from the SPI bus */
    if (HAL_SPI_Receive(EEPROM_SPI, &answerByte, 1, 200) != HAL_OK) {
        Error_Handler();
    }

    return (uint8_t)answerByte;
}
/**
  * @brief  Enables the write access to the EEPROM.
     * Write enable
  * @param  None
  * @retval None
  */
void EEPROM_WriteEnable(void)
{
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    uint8_t command[1] = { EEPROM_WREN };//00000110
    /* Send "Write Enable" instruction */
    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();
}

/**
  * @brief  Disables the write access to the EEPROM.
     * Write fault
  * @param  None
  * @retval None
  */
void EEPROM_WriteDisable(void)
{
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    uint8_t command[1] = { EEPROM_WRDI };//00000100

    /* Send "Write Disable" instruction */
    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();
}

/**
  * @brief  Write new value in EEPROM Status Register.
  *
  * @param  regval : new value of register
  * @retval None
  */
void EEPROM_WriteStatusRegister(uint8_t regval)
{
    uint8_t command[2];

    command[0] = EEPROM_WRSR;
    command[1] = regval;

    // Enable the write access to the EEPROM
    EEPROM_WriteEnable();

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    // Send "Write Status Register" instruction
    // and Regval in one packet
    EEPROM_SPI_SendInstruction((uint8_t*)command, 2);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    EEPROM_WriteDisable();

    EEPROM_SPI_WaitStandbyState();// Waiting for the operation to complete
}

/**
  * @brief  Read EEPROM Status Register Instruction.
  *
  * @param  none
 * @retval answerbyte: SR value
  */

uint8_t EEPROM_ReadStatusRegister(void)
{
   uint8_t answerByte;
    // Select the EEPROM: Chip Select low
    EEPROM_CS_HIGH();
    EEPROM_CS_LOW();

    answerByte = EEPROM_SendByte(EEPROM_RDSR);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    return answerByte;

}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the EEPROM's
  *         status register and loop until write operation has completed.
     * Waiting for the operation to complete, enter the standby status
  *
  * @param  None
  * @retval None
  */
uint8_t EEPROM_SPI_WaitStandbyState(void)
{
    uint8_t EEPROMstatus[1] = { 0x00 };
    uint8_t command[1] = { EEPROM_RDSR };// Read status register

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();
    // Send "Read Status Register" instruction
    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);// Send a read status register command
    // Loop as long as the memory is busy with a write cycle
    do {

        while (HAL_SPI_Receive(EEPROM_SPI, (uint8_t*)EEPROMstatus, 1, 200) == HAL_BUSY) {// Waiting for the data reception completion
            HAL_Delay(1);
        };
        HAL_Delay(1);

       } while ((EEPROMstatus[0] & EEPROM_WIP_BUSY) == SET); // Write in progress

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    return 0;
}

/** Send instruction
 * @brief Low level function to send header data to EEPROM
 *
 * @param instruction array of bytes to send
 * @param size        data size in bytes
 */
void EEPROM_SPI_SendInstruction(uint8_t *instruction, uint8_t size)
{
    while (EEPROM_SPI->State == HAL_SPI_STATE_RESET) {// Wait for SPI initialization
        HAL_Delay(1);
    }

    if (HAL_SPI_Transmit(EEPROM_SPI, (uint8_t*)instruction, (uint16_t)size, 10000) != HAL_OK) {//send command
        Error_Handler();
    }

}

/**
 * @brief The Chip Erase function will erase all bits (FFh) in the array
 *
 * @param none
 * @param none
 */
void EEPROM_CHIP_ERASE(void)
{
    Printf("\nEEPROM_CHIP_ERASE\n");
    // Enable the write access to the EEPROM
    EEPROM_WriteEnable();
//    printEepromSpiStatus();
    EEPROM_SPI_WaitStandbyState();

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    uint8_t command[1] = { EEPROM_CE };//11000111

    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    EEPROM_SPI_WaitStandbyState();// Waiting for the operation to complete

    // Disable the write access to the EEPROM
    EEPROM_WriteDisable();
}

/**
 * @brief Deep Power-Down mode of the 25LC1024 is its lowest power consumption state
 *
 * @param none
 * @param none
 */
void EEPROM_PowerDown(void)
{
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();
    uint8_t command[1] = { EEPROM_DPD };//10111001

    /* Send "Write Disable" instruction */
    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    HAL_Delay(3);

}
/**
 * @brief release from Deep Power-down
 *
 * @param none
 * @param none
 */


void EEPROM_WakeUP(void)
{
    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();
    uint8_t command[1] = { EEPROM_RDID };//10101011

    /* Send "Write Disable" instruction */
    EEPROM_SPI_SendInstruction((uint8_t*)command, 1);

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    HAL_Delay(3);
}
/**
 * @brief release from Deep Power-down and Read Electronic Signature command
 *
 * @param WriteAddr:a dummy address of 24 bits
 * @param temp:Manufacturers ID 0x29
 */

uint8_t EEPROM_ReadID(uint32_t WriteAddr)
{
    uint8_t temp = 0;
    uint8_t header[4];

    header[0] = EEPROM_RDID;    // Send "EEPROM_RDID" instruction
    // send 24-bit Address (maximum address 1fffh)
    header[1] = (WriteAddr >> 16)&0XFF;
    header[2] = (WriteAddr >> 8 )&0XFF;
    header[3] =  WriteAddr       &0XFF;

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    EEPROM_SPI_SendInstruction((uint8_t*)header, 4);

    /* Return the byte read from the SPI bus */
    if (HAL_SPI_Receive(EEPROM_SPI, &temp, 1, 200) != HAL_OK) {
        Error_Handler();
    }
    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    return temp;

}

void EEPROM_PAGE_ERASE  (uint32_t WriteAddr)
{
    Printf("\nEEPROM_PAGE_ERASE\n");
    while (EEPROM_SPI->State != HAL_SPI_STATE_READY) {// Wait for SPI initialization
        HAL_Delay(1);
    }
    /*
        We gonna send all commands in one packet of 4 bytes
     */
    uint8_t header[4];

    header[0] = EEPROM_PE;           // Send "Page erase" instruction
    // send 24-bit Address (maximum address 1fffh)
    header[1] = (WriteAddr >> 16)&0xFF; // Send high 8-bit address
    header[2] = (WriteAddr >> 8 )&0xFF; // Send midlle 8-bit address
    header[3] =  WriteAddr       &0xFF; // Send low 8-bit address

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    /* Send WriteAddr address byte to read from */
    EEPROM_SPI_SendInstruction(header, 4);
    HAL_Delay(2000);
    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();
}

EEPROMStatus EEPROM_SPI_WriteByte  (uint8_t* pBuffer, uint32_t WriteAddr)
{
    while (EEPROM_SPI->State != HAL_SPI_STATE_READY) {
        HAL_Delay(1);
    }

    HAL_StatusTypeDef spiTransmitStatus;// SPI transmission status

    EEPROM_WriteEnable();// write enable

    /*
        We gonna send commands in one packet of 4 bytes
     */
    uint8_t header[4];

    header[0] = EEPROM_WRITE;    // Send "Write to Memory" instruction
    // send 24-bit Address (maximum address 1fffh)
    header[1] = (WriteAddr >> 16)&0xFF;
    header[2] = (WriteAddr >> 8 )&0xFF;
    header[3] =  WriteAddr       &0xFF;

    // Select the EEPROM: Chip Select low
    EEPROM_CS_LOW();

    EEPROM_SPI_SendInstruction((uint8_t*)header, 4);

    // Make 5 attemtps to write the data
    for (uint8_t i = 0; i < 5; i++) {
        spiTransmitStatus = HAL_SPI_Transmit(EEPROM_SPI, pBuffer, 1, 1000);

        if (spiTransmitStatus == HAL_BUSY) {
            HAL_Delay(5);
        } else {
            break;
        }
    }

    // Deselect the EEPROM: Chip Select high
    EEPROM_CS_HIGH();

    // Wait the end of EEPROM writing
    EEPROM_SPI_WaitStandbyState();// Waiting for write to complete

    // Disable the write access to the EEPROM
    EEPROM_WriteDisable();

    if (spiTransmitStatus == HAL_ERROR) {
        return EEPROM_STATUS_ERROR;
    } else {
        return EEPROM_STATUS_COMPLETE;
    }
}

void printEepromSpiStatus()
{
    uint8_t status = EEPROM_ReadStatusRegister();
    printf ("status Spi EEPROM: %X\r\n",status);
}
