#ifndef EEPROM_SPI_H
#define EEPROM_SPI_H
#include "main.h"

/* Includes ------------------------------------------------------------------*/
//#include "stm32f1xx_hal.h"
//#include "cmsis_os.h"


/* 25LC1024 SPI EEPROM defines */
/* SPI EEPROM supports the following table 6 command, ie Read (read memory), WRITE (write memory), WREN (write enable), WRDI (write prohibition), RDSR (read status register), WRSR (write status register) */
#define EEPROM_READ  0x03  /*!<Receive data from the specified address of the storage array */
#define EEPROM_WRITE 0x02  /*!<The specified address of the storage array begins to write data */
#define EEPROM_WREN  0x06  /*!<Setup write enable lockout (enable write) */
#define EEPROM_WRDI  0x04  /*!<Reset write enable lock storage (failback operation) */
#define EEPROM_RDSR  0x05  /*!<Read status register */
#define EEPROM_WRSR  0x01  /*!<Write status register */

/* Large capacity EEPROM, in order to improve the EEPROM operational efficiency, Microchip adds this EEPROM Page / Sector / CHIP ERASE command */
#define EEPROM_PE    0x42  /*!<Page erase - erase one page in the storage array */
#define EEPROM_SE    0xD8  /*!<Sector erase - erase a sector in the storage array */
#define EEPROM_CE    0xC7  /*!<Chip Erase - Erase all sectors in the storage array */
#define EEPROM_RDID  0xAB  /*!<Restore and read electronic signature from deep sleep */
#define EEPROM_DPD   0xB9  /*!<Enter deep sleep mode */

/* RDSR (Read Status Register 8bits) */
#define EEPROM_WIP_RDY         0x00  /*!<no write is in progress read-only */
#define EEPROM_WIP_BUSY        0x01  /*!<busy with a write operation read-only */
#define EEPROM_WEL_EN          0x02  /*!<The latch allows writes to the array read-only */
#define EEPROM_WEL_DIS         0x00  /*!<The latch prohibits Writes to the array read only */
#define EEPROM_BP00            0x00  /* No protected array */
#define EEPROM_BP01            0x04  /* Protected array high 1/4 address (sector 3) */
#define EEPROM_BP10            0x08  /* Protected array high 1/2 address (sector 2, 3) */
#define EEPROM_BP11            0x0C  /* Protected array all sectors (sectors 0, 1, 2, 3) */
#define EEPROM_WPEN            0x80  /* Write protection */


#define EEPROM_PAGESIZE        256   /* !< Pagesize according to documentation :256Byte*/
/*25LC1024：1Mbit= 1024Kbit =128KB  = 512*256B = 131072 X 8bit
 Page 1 Address: 00000-000FF   length 256b
                  0-255
 Page 2 Address: 00100-001FF length 256b
                  256-511
 Page 3 Address: 00200-002FF length 256b
                  512-1278
 Page 4 Address: 00300-003FF length 256b
`
`
`
 Page 510 Address: 1FD00-1FDFF length 256b
 Page 511 Address: 1FE00-1FEFF length 256b
 Page 512 Address: 1F000-1FFFF length 256b
*/


#define EEPROM_CS_HIGH()    HAL_GPIO_WritePin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin, GPIO_PIN_SET)
#define EEPROM_CS_LOW()     HAL_GPIO_WritePin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin, GPIO_PIN_RESET); HAL_Delay(50);

/**
 * @brief EEPROM Operations statuses
 */
typedef enum {
    EEPROM_STATUS_PENDING,
    EEPROM_STATUS_COMPLETE,
    EEPROM_STATUS_ERROR
} EEPROMStatus;

void EEPROM_SPI_INIT(SPI_HandleTypeDef * hspi);// Initialization function
uint8_t EEPROM_SPI_WaitStandbyState(void);// Waiting for the operation completion function
// Start reading data of the specified length at the specified address
EEPROMStatus EEPROM_SPI_ReadBuffer (uint8_t* pBuffer, uint32_t ReadAddr,  uint16_t NumByteToRead);

// Write a byte data and do not implement similar EEPROM_SENDBYTE ()
EEPROMStatus EEPROM_SPI_WriteByte  (uint8_t* pBuffer, uint32_t WriteAddr);
// Start writing a specified length of data at the specified address
EEPROMStatus EEPROM_SPI_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
// Start writing not greater than the entire page data at the specified address
EEPROMStatus EEPROM_SPI_WritePage  (uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);



/* Low Layer Functions underlayer function */
uint8_t EEPROM_SendByte(uint8_t byte);// Send a byte, return a byte

void EEPROM_WriteEnable(void);// write enable
void EEPROM_WriteDisable(void);// Writing

void    EEPROM_WriteStatusRegister(uint8_t regval);// Write status register
uint8_t EEPROM_ReadStatusRegister (void);          // Read status register

void EEPROM_SPI_SendInstruction(uint8_t *instruction, uint8_t size);// Send an operation command and data address


// Personalization function
void EEPROM_PAGE_ERASE  (uint32_t WriteAddr);// Specify the address page erase, not implemented
void EEPROM_SECTOR_ERASE(uint32_t WriteAddr);// Specify address sector erase, not implemented
/*25LC1024：1Mbit= 1024Kbit=128KB =4*32KB（32K Byte/sector）
 0 sector address: 00000H-07FFFH
 1st sector address: 08000H-0FFFH
 2nd sector address: 10000H-17FFFH
 Section 3 Address: 18000H-1FFFFH

*/

void EEPROM_CHIP_ERASE(void);// All-piece erasure
void EEPROM_PowerDown(void);// Enter the power-down mode
uint8_t EEPROM_ReadID(uint32_t WriteAddr);// Read the virtual address of the electronic signature ID, 24BITS, return 0x29h
void EEPROM_WakeUP(void);// Wake-up instruction
void printEepromSpiStatus();

#ifdef __cplusplus
}
#endif

#endif
