#include <pybind11/pybind11.h>
#include <iostream>
#include <string>
#include "bitfile.h"
#include "uploader.h"

namespace py = pybind11;
using namespace pybind11::literals;

static void mem_dump(const void* ptr, int size);
static void dump_to_file(const void* ptr, int size);

template<typename TSIZE>
void skip_boundary(const char* &buf, TSIZE &size);
//---------------------------------------------------------------------------
py::dict dump(const std::string& bitstream) {
	BitFile bitFile;
	auto data = bitstream.data();
	auto size = bitstream.size();
	skip_boundary(data, size);
	std::cout << "Read bitstream from buffer" << std::endl;
	bitFile.readBuff(data, size, false);
	dump_to_file(bitFile.getData(), bitFile.getLength() / 8);
	// bitFile.print();
	return py::dict{
		"NCDFilename"_a = bitFile.getNCDFilename(),
		"device"_a = bitFile.getPartName(),
		"date"_a = bitFile.getDate(),
		"time"_a = bitFile.getTime(),
		"length"_a = bitFile.getLength(),
	};
}
//---------------------------------------------------------------------------
py::dict upload(const std::string& portName, const std::string& bitstream) {
	BitFile bitFile;
	auto data = bitstream.data();
	auto size = bitstream.size();
	skip_boundary(data, size);
	try {
		bitFile.readBuff(data, size, false);
	}
	catch (...) {
		std::cout << "Fail read bitstream" << std::endl;
		return py::dict{};
	}
	bitFile.print();
	if (!Uploader::upload(portName,
		bitFile.getData(), bitFile.getLength() / 8)) {
		std::cout << "Fail upload bitstream" << std::endl;
		return py::dict{};
	}
	return py::dict{
		"NCDFilename"_a = bitFile.getNCDFilename(),
		"device"_a = bitFile.getPartName(),
		"date"_a = bitFile.getDate(),
		"time"_a = bitFile.getTime(),
		"length"_a = bitFile.getLength(),
	};
}
//---------------------------------------------------------------------------
PYBIND11_MODULE(XilinxUpload, m) {
	m.doc() = "pybind11 example plugin"; // optional module docstring
	m.def("dump", &dump, "Print Xilinx bitstream");
	m.def("upload", &upload, "Upload Xilinx bitstream");
}
//---------------------------------------------------------------------------
void mem_dump(const void* ptr, int size) {
	auto data = reinterpret_cast<const uint8_t*>(ptr);
	while (size--) {
		printf("%02X ", *data++);
	}
	printf("\n");
}
void dump_to_file(const void* ptr, int size) {
	FILE* fd;
	if(!(fd = fopen("bitstream.dump", "wb"))) {
		return;
	}
	fwrite(ptr, size, 1, fd);
	fclose(fd);
}
//---------------------------------------------------------------------------
template<typename TSIZE>
void skip_boundary(const char* &buf, TSIZE &size) {
	TSIZE len = 0;
	while (size) {
		if (*buf++ == '\r') {
			--size;
			if (*buf++ == '\n') {
				--size;
				if (len == 0) break;
				len = 0;
				continue;
			}
		}
		--size;
		++len;
	}
}

