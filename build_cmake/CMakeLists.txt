cmake_minimum_required(VERSION 2.8)
project(Nuanceur)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release"
		FORCE)
endif()

if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_definitions(-DNDEBUG)
	else()
		add_definitions(-D_DEBUG)
	endif()
endif()

if (WIN32)
	set(TARGET_PLATFORM WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_LIB)
endif (WIN32)

add_library(Nuanceur
	../src/builder/Operations.cpp
	../src/builder/ShaderBuilder.cpp

	../src/generators/GlslShaderGenerator.cpp
	../src/generators/HlslShaderGenerator.cpp
	../src/generators/SpirvShaderGenerator.cpp

	../include/nuanceur/builder/BoolValue.h
	../include/nuanceur/builder/Float2Value.h
	../include/nuanceur/builder/Float3Value.h
	../include/nuanceur/builder/Float4Value.h
	../include/nuanceur/builder/FloatValue.h
	../include/nuanceur/builder/Matrix44Value.h
	../include/nuanceur/builder/Operations.h
	../include/nuanceur/builder/ShaderBuilder.h
	../include/nuanceur/builder/SwizzleSelector4.h
	../include/nuanceur/builder/Texture2DValue.h

	../include/nuanceur/generators/GlslShaderGenerator.h
	../include/nuanceur/generators/HlslShaderGenerator.h
	../include/nuanceur/generators/SpirvShaderGenerator.h
)
target_include_directories(Nuanceur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../Framework/include)

