#ifndef _SPI_DRV_H_
#define _SPI_DRV_H_

int SPISetup(uint8_t channel,uint32_t speed,uint8_t mode);

int SPIDataRW(uint8_t channel, unsigned char* dataRX, unsigned char* dataTX, uint32_t len);

int SPIDataR(uint8_t channel, unsigned char* dataRX, uint32_t len);

#endif //#ifndef _SPI_DRV_H_