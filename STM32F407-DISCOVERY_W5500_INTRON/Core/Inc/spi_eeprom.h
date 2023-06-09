#ifndef _STM32_EEPROM_SPI_H
#define _STM32_EEPROM_SPI_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 25AA1024 SPI EEPROM defines */
#define EEPROM_READ  0x03  /* Read data from memory array beginning at selected address */
#define EEPROM_WRITE 0x02  /* Write data to memory array beginning at selected address */
#define EEPROM_WREN  0x06  /* Write Enable */
#define EEPROM_WRDI  0x04  /* Write Disable */
#define EEPROM_RDSR  0x05  /* Read Status Register */
#define EEPROM_WRSR  0x01  /* Write Status Register */
//Я добавил
#define EEPROM_PE    0x42  /* Page Erase – erase one page in memory array */
#define EEPROM_SE    0xD8  /* Sector Erase – erase one sector in memory array */
#define EEPROM_CE    0xC7  /* Chip Erase – erase all sectors in memory array */
#define EEPROM_RDIR  0xAB  /* Release from Deep Power-down and Read Electronic Signature */
#define EEPROM_DPD   0xB9  /* Deep Power-Down mode */

#define EEPROM_WIP_FLAG        0x01  /*!< Write In Progress (WIP) flag */

#define EEPROM_PAGESIZE        256    /*!< Pagesize according to documentation */
#define EEPROM_BUFFER_SIZE     256    /*!< EEPROM Buffer size. Setup to your needs */

#define EEPROM_CS_HIGH()    HAL_GPIO_WritePin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin, GPIO_PIN_SET)
#define EEPROM_CS_LOW()     HAL_GPIO_WritePin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin, GPIO_PIN_RESET)

/**
 * @brief EEPROM Operations statuses
 */
typedef enum {
    EEPROM_STATUS_PENDING,
    EEPROM_STATUS_COMPLETE,
    EEPROM_STATUS_ERROR
} EepromOperations;

EepromOperations EEPROM_SPI_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
EepromOperations EEPROM_SPI_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint8_t NumByteToWrite);
EepromOperations EEPROM_SPI_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint8_t EEPROM_SPI_WaitStandbyState(void);
void EEPROM_SPI_WriteByte(uint8_t byte, uint32_t WriteAddr);
void EEPROM_SPI_ReadByte(uint32_t ReadAddr);

/* Low layer functions */
uint8_t EEPROM_SendByte(uint8_t byte);
void sEE_WriteEnable(void);
void sEE_WriteDisable(void);
void sEE_WriteStatusRegister(uint8_t regval);
uint8_t sEE_ReadStatusRegister(void);

void  EEPROM_SPI_SendInstruction(uint8_t *instruction, uint8_t size);
void  EEPROM_SPI_ReadStatusByte(SPI_HandleTypeDef SPIe, uint8_t *statusByte );

#ifdef __cplusplus
}
#endif

#endif // _STM32_EEPROM_SPI_H
