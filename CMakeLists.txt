cmake_minimum_required(VERSION 3.14)

project(MicroTest VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(MICROTEST_BUILD_EXAMPLES "Build and register examples/ExampleTest.c as a test" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#-----Library-----
# Static only: MicroTest_SetUp / MicroTest_TearDown from the test program must replace the defaults,
# which is not possible with a shared library (DLL) on Windows.
add_library(MicroTest STATIC
	src/MicroTest.c
	src/Defaults/MicroTestSetUp.c
	src/Defaults/MicroTestTearDown.c
)
add_library(MicroTest::MicroTest ALIAS MicroTest)

target_compile_definitions(MicroTest PRIVATE MICROTEST_LIBRARY)
target_compile_features(MicroTest PUBLIC c_std_99)
target_include_directories(MicroTest PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(MicroTest PROPERTIES
	PUBLIC_HEADER src/MicroTest.h
	DEBUG_POSTFIX d
)

if(MSVC)
	target_compile_options(MicroTest PRIVATE /W4)
	target_compile_definitions(MicroTest PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
	target_compile_options(MicroTest PRIVATE -Wall -Wextra)
	find_library(MICROTEST_MATH_LIBRARY m)
	if(MICROTEST_MATH_LIBRARY)
		target_link_libraries(MicroTest PUBLIC m)
		set(MICROTEST_PC_LIBS "-lm")
	endif()
endif()
#-----Library-----

#-----Install-----
install(TARGETS MicroTest
	EXPORT        MicroTestTargets
	ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

set(MICROTEST_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/MicroTest)

install(EXPORT MicroTestTargets
	FILE        MicroTestTargets.cmake
	NAMESPACE   MicroTest::
	DESTINATION ${MICROTEST_CMAKE_DIR}
)

configure_package_config_file(cmake/MicroTestConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/MicroTestConfig.cmake
	INSTALL_DESTINATION ${MICROTEST_CMAKE_DIR}
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/MicroTestConfigVersion.cmake
	COMPATIBILITY SameMajorVersion
)
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/MicroTestConfig.cmake
	${CMAKE_CURRENT_BINARY_DIR}/MicroTestConfigVersion.cmake
	DESTINATION ${MICROTEST_CMAKE_DIR}
)

configure_file(cmake/MicroTest.pc.in ${CMAKE_CURRENT_BINARY_DIR}/MicroTest.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MicroTest.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
#-----Install-----

#-----Uninstall-----
if(NOT TARGET uninstall)
	add_custom_target(uninstall
		COMMAND ${CMAKE_COMMAND} -DMANIFEST=${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
		        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake
	)
endif()
#-----Uninstall-----

#-----Examples-----
if(MICROTEST_BUILD_EXAMPLES)
	enable_testing()
	add_executable(ExampleTest examples/ExampleTest.c)
	target_link_libraries(ExampleTest PRIVATE MicroTest::MicroTest)
	add_test(NAME ExampleTest COMMAND ExampleTest)
endif()
#-----Examples-----
