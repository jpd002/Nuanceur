cmake_minimum_required(VERSION 2.8)
project(Nuanceur)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include ../../Framework/include)

add_library(Nuanceur
	../src/builder/Operations.cpp
	../src/builder/ShaderBuilder.cpp
	../src/generators/GlslShaderGenerator.cpp
	../src/generators/HlslShaderGenerator.cpp
	../src/generators/SpirvShaderGenerator.cpp
)
