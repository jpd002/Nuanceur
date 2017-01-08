cmake_minimum_required(VERSION 2.8)
project(Nuanceur)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release"
		FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DNDEBUG)
else()
	add_definitions(-D_DEBUG)
endif()

if (WIN32)
    set(TARGET_PLATFORM WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_LIB)
endif (WIN32)

include_directories(../include ../../Framework/include)

add_library(Nuanceur
	../src/builder/Operations.cpp
	../src/builder/ShaderBuilder.cpp
	../src/generators/GlslShaderGenerator.cpp
	../src/generators/HlslShaderGenerator.cpp
	../src/generators/SpirvShaderGenerator.cpp
)
