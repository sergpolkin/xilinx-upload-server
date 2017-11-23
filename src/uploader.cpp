#include <thread>
#include <chrono>
#include "uploader.h"
#include "serial/serial.h"
namespace {
	serial::Serial port;
	constexpr auto portBaud = 115200;
}
static uint32_t crc32(const void *buf, uint32_t len);
static std::string findPort(const std::string& portName);
static bool initPort(const std::string& portName);
//---------------------------------------------------------------------------
bool Uploader::upload(const std::string& portName,
	const void* data, uint32_t size)
{
	if(!initPort(portName)) return false;
	if(!reset()) return false;
	return send(data, size);
}
//---------------------------------------------------------------------------
bool Uploader::send(const void* data, uint32_t size) {
	uint32_t sendSize = size + 4;
	uint32_t crc = crc32(data, size);
	uint8_t c = 0x01;
	auto buf = reinterpret_cast<const uint8_t*>(data);

	fprintf(stdout, "Uploader::send %u bytes\n", size);

	port.write(&c, 1);
	if (!wait()) return false;
	port.read(&c, 1);
	// fprintf(stdout, "Start\n");
	if(c != 'S') return false;
	// size
	port.write(reinterpret_cast<const uint8_t*>(&sendSize), 4);
	if (!wait()) return false;
	port.read(&c, 1);
	// fprintf(stdout, "Size\n");
	if(c != 'D') return false;
	// data
	while(size) {
		auto i = port.write(buf, (size > 2048) ? 2048 : size);
		// std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if(i > 0) {
			size -= i;
			buf += i;
			fputc('.', stdout);
			fflush(stdout);
		}
	}
	fputc('\n', stdout);
	if (!wait()) return false;
	port.read(&c, 1);
	// fprintf(stdout, "Data\n");
	if(c != 'C') return false;
	// crc32
	port.write(reinterpret_cast<const uint8_t*>(&crc), 4);
	if (!wait()) return false;
	port.read(&c, 1);
	fprintf(stdout, "crc32: 0x%08X", crc);
	if(c == 'O') fprintf(stdout, " - OK\n");
	else if(c == 'E') fprintf(stdout, " - Error\n");
	else fprintf(stdout, " - Fail\n");
	return c == 'O';
}
//---------------------------------------------------------------------------
bool Uploader::reset(void) {
	uint8_t c = 0x00;
	port.write(&c, 1);
	if (!wait()) return false;
	port.read(&c, 1);
	return c == 'R';
}
//---------------------------------------------------------------------------
bool Uploader::wait(int n, int ms) {
	while(port.available() < n && ms--) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return ms > 0;
}
//---------------------------------------------------------------------------
static uint32_t crc32(const void *data, uint32_t len) {
	uint32_t crc_table[256];
	uint32_t crc;
	auto buf = reinterpret_cast<const uint8_t*>(data);

	for (int i = 0; i < 256; i++) {
		crc = i;
		for (int j = 0; j < 8; j++)
		crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

		crc_table[i] = crc;
	}

	crc = 0xFFFFFFFFUL;

	while (len--)
		crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

	return crc ^ 0xFFFFFFFFUL;
};
//---------------------------------------------------------------------------
static std::string findPort(const std::string& portName) {
	auto ports = serial::list_ports();
	for (auto& p : ports) {
		if (p.hardware_id.compare(0, portName.size(), portName) == 0) {
			return p.port;
		}
	}
	return {};
}
//---------------------------------------------------------------------------
static bool initPort(const std::string& portName) {
	auto p = findPort(portName);
	if (p.empty()) {
		fprintf(stderr, "Can't find port %s\n", portName.c_str());
		return false;
	}
	port.setPort(p);
	port.setBaudrate(portBaud);
	try { port.open(); }
	catch (...) {
		fprintf(stderr, "Can't open port %s\n", p.c_str());
		return false;
	};
	port.flush();
	fprintf(stdout, "Open port %s:%d\n",
		port.getPort().c_str(), port.getBaudrate());
	return true;
}
