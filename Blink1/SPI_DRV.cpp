#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "SPI_DRV.h"

const char* spiDev0 = "/dev/spidev0.0";
const char* spiDev1 = "/dev/spidev0.1";
const uint8_t spiBPW = 8;
const uint16_t spiDelay = 0;

uint32_t spiSpeed[2];
int spiFds[2];

int SPISetup(uint8_t channel, uint32_t speed, uint8_t mode)
{
	int fd;
	mode &= 3;
	channel &= 1;

	if ((fd = open(channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0) {
		return -1;
	}

	spiSpeed[channel] = speed;
	spiFds[channel] = fd;

	if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
		return -1;
	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0)
		return -1;

	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
		return -1;
	//if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW) < 0)
	//	return -1;

	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
		return -1;
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0)
		return -1;

	return fd;
}

int SPIDataRW(uint8_t channel, unsigned char * dataRX, unsigned char * dataTX, uint32_t len)
{
	struct spi_ioc_transfer spi;
	channel &= 1;

	memset(&spi, 0, sizeof(spi));

	spi.tx_buf = (unsigned long)dataTX;
	spi.rx_buf = (unsigned long)dataRX;
	spi.len = len;
	spi.delay_usecs = spiDelay;
	spi.speed_hz = spiSpeed[channel];
	spi.bits_per_word = spiBPW;

	return ioctl(spiFds[channel], SPI_IOC_MESSAGE(1), &spi);
}

int SPIDataR(uint8_t channel, unsigned char* dataRX, uint32_t len)
{
	return read(spiFds[channel],(void*)dataRX,len);
}
