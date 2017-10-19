cmake_minimum_required(VERSION 3.2)
project(XilinxUploadModule)
set(PYBIND11_PYTHON_VERSION 2.7)
add_subdirectory(pybind11)
pybind11_add_module(XilinxUpload
	src/bitfile.cpp src/bitfile.h src/io_exception.h
	src/upload_module.cpp
)
add_custom_command(
	TARGET XilinxUpload POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy 
	${CMAKE_CURRENT_BINARY_DIR}/XilinxUpload.so
	${CMAKE_CURRENT_SOURCE_DIR}/XilinxUpload.so
)
