#ifndef _SPI_H_
#define _SPI_H_

namespace IO {

	class SPI {
		int fd = -1;
		int ch = -1;
		std::string error_str;
		bool opened = false;
	public:
		SPI(int ch, int spd) {
			fd = wiringPiSPISetup(ch, spd);
			this->ch = ch;
			if (fd < 0) {
				error_str = "SPI Open error";
			}
			else {
				opened = true;
			}
		}

		SPI(int ch, int spd, int mode) {
			fd = wiringPiSPISetupMode(ch, spd, mode);
			this->ch = ch;
			if (fd < 0) {
				error_str = "SPI Open error";
			}
			else {
				opened = true;
			}
		}

		bool DataRW(uint8_t* RWBuf, size_t size) {
			int ret = wiringPiSPIDataRW(ch, RWBuf, size);
			uint32_t sym = 0;
			for (size_t i = 0; i < size; i++)
				sym += RWBuf[i];
			if (sym > 0)
				return true;
			else
				return false;
		}
	};
}
#endif	// _SPI_H_
