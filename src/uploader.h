#pragma once
#include <cstdint>
#include <string>
struct Uploader {
public:
	static bool upload(const std::string& portName,
		const void* data, uint32_t size);
private:
	static bool reset(void);
	static bool send(const void* data, uint32_t size);
	static bool wait(int n = 1, int ms = 3000);
};
