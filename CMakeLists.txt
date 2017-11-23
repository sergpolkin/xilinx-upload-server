cmake_minimum_required(VERSION 3.2)
project(XilinxUploadModule)
# serial
add_subdirectory(serial)
# pybind11
set(PYBIND11_PYTHON_VERSION 2.7)
add_subdirectory(pybind11)
pybind11_add_module(XilinxUpload
	src/bitfile.cpp src/bitfile.h src/io_exception.h
	src/uploader.cpp src/uploader.h
	src/upload_module.cpp
)
target_link_libraries(XilinxUpload PRIVATE serial)
if(UNIX)
	set_target_properties(serial PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
	add_custom_command(
		TARGET XilinxUpload POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_BINARY_DIR}/XilinxUpload.so
		${CMAKE_CURRENT_SOURCE_DIR}/XilinxUpload.so
	)
else()
	add_custom_command(
		TARGET XilinxUpload POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_BINARY_DIR}/Debug/XilinxUpload.pyd
		${CMAKE_CURRENT_SOURCE_DIR}/XilinxUpload.pyd
	)
endif()
