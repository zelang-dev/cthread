cmake_minimum_required(VERSION 2.8...3.14)

project(
    cthread
    VERSION 4.1.0.9
    DESCRIPTION "Emulated C11 threads, compatibility C11 atomics, with custom malloc replacement."
    HOMEPAGE_URL "https://github.com/zelang-dev/cthread"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 90)

include(CMakeDependentOption)
include(GNUInstallDirs)
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

if(WIN32)
    if($ENV{Platform} MATCHES x86)
        message("Building Windows x86-32bit")
        add_definitions(-D_WIN32_PLATFORM_X86=1)
    endif()
endif()

set(CMAKE_CONFIGURATION_TYPES=Debug;Release)
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/built")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

cmake_dependent_option(BUILD_TESTING
  "Build the unit tests when BUILD_TESTING is enabled and we are the root project" OFF
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

option(BUILD_SHARED_LIBS    "Build the library as a shared (dynamically-linked) " OFF)

add_definitions(-DENABLE_OVERRIDE=1)
set(cthread_files rpmalloc.c cthread.c)
if(BUILD_SHARED_LIBS)
    add_library(cthread SHARED ${cthread_files})
else()
    add_library(cthread STATIC ${cthread_files})
    add_definitions(-DPTW32_STATIC_LIB=1)
endif()

if(UNIX)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -D ENABLE_ASSERTS=1 ")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fomit-frame-pointer -Wno-return-type")
    find_package(Threads)
    if(APPLE)
        target_link_libraries(cthread PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    else()
        target_link_libraries(cthread PUBLIC ${CMAKE_THREAD_LIBS_INIT} atomic)
    endif()
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D ENABLE_ASSERTS=1 ")
    add_definitions(-DPTW32_CLEANUP_SEH=1 -D_CRT_SECURE_NO_DEPRECATE)
    add_definitions("/wd4244 /wd4267 /wd4033 /wd4715")
	add_subdirectory(deps)
    target_include_directories(cthread AFTER PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    target_link_libraries(cthread PUBLIC pthreadVSE3)
endif()

set_property(TARGET cthread PROPERTY POSITION_INDEPENDENT_CODE True)

enable_testing()
if(BUILD_TESTING)
	add_subdirectory(tests)
endif()

set(_fmt TGZ)
if(WIN32)
  set(_fmt ZIP)
endif()

set(CPACK_GENERATOR ${_fmt})
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_NUGET_COMPONENT_INSTALL ON)
set(CPACK_WIX_COMPONENT_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_VENDOR "https://github.com/zelang-dev/cthread")
include(CPack)

set(CMAKE_INSTALL_CONFIG_NAME ${CMAKE_BUILD_TYPE})
install(TARGETS ${cthread} DESTINATION lib)
install(FILES cthread.h rpmalloc.h catomic.h DESTINATION include )
if(WIN32)
    install(DIRECTORY include/ DESTINATION include )
endif()
