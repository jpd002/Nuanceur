cmake_minimum_required(VERSION 3.5)
project(Nuanceur)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release"
		FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

if (WIN32)
	set(TARGET_PLATFORM_WIN32 TRUE)
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

	../include/nuanceur/Builder.h

	../include/nuanceur/builder/ArrayUintValue.h
	../include/nuanceur/builder/BoolValue.h
	../include/nuanceur/builder/Bool2Value.h
	../include/nuanceur/builder/Float2Value.h
	../include/nuanceur/builder/Float3Value.h
	../include/nuanceur/builder/Float4Value.h
	../include/nuanceur/builder/FloatValue.h
	../include/nuanceur/builder/FloatSwizzleSelector.h
	../include/nuanceur/builder/FloatSwizzleSelector4.h
	../include/nuanceur/builder/ImageUint2DValue.h
	../include/nuanceur/builder/Int2Value.h
	../include/nuanceur/builder/Int3Value.h
	../include/nuanceur/builder/Int4Value.h
	../include/nuanceur/builder/IntValue.h
	../include/nuanceur/builder/IntSwizzleSelector4.h
	../include/nuanceur/builder/Matrix44Value.h
	../include/nuanceur/builder/Operations.h
	../include/nuanceur/builder/ShaderBuilder.h
	../include/nuanceur/builder/SubpassInputValue.h
	../include/nuanceur/builder/Texture2DValue.h
	../include/nuanceur/builder/Uint3Value.h
	../include/nuanceur/builder/Uint4Value.h
	../include/nuanceur/builder/UintValue.h
	../include/nuanceur/builder/UintSwizzleSelector4.h

	../include/nuanceur/generators/GlslShaderGenerator.h
	../include/nuanceur/generators/HlslShaderGenerator.h
	../include/nuanceur/generators/SpirvShaderGenerator.h
)
target_include_directories(Nuanceur PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../../Framework/include)

if(TARGET_PLATFORM_WIN32)
	#Tests can probably run on other platforms, but they're untested.

	if(NOT TARGET vkrunner)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../deps/vkrunner
			${CMAKE_CURRENT_BINARY_DIR}/vkrunner
		)
	endif()

	add_executable(NuanceurTestSuite
		../tests/BasicTest.cpp
		../tests/BasicTest.h
		../tests/Main.cpp
		../tests/Swizzle1Test.cpp
		../tests/Swizzle1Test.h
		../tests/Swizzle2Test.cpp
		../tests/Swizzle2Test.h
		../tests/Test.cpp
		../tests/Test.h
	)
	target_include_directories(NuanceurTestSuite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include ${CMAKE_CURRENT_SOURCE_DIR}/../deps/vkrunner)
	target_link_libraries(NuanceurTestSuite PUBLIC Nuanceur Framework vkrunner)
endif()
