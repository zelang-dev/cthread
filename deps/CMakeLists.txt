cmake_minimum_required(VERSION 3.11)

include(cmake/get_version.cmake)

project(pthreads4w VERSION ${PTHREADS4W_VERSION} LANGUAGES C CXX)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Add support for standard installation
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
cmake_dependent_option(BUILD_TESTING
  "Build the unit tests when BUILD_TESTING is enabled and we are the root project" OFF
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

#################################
# Target Arch                   #
#################################
include(cmake/target_arch.cmake)
get_target_arch(TARGET_ARCH)

if(TARGET_ARCH STREQUAL "ARM")
  add_definitions(-DPTW32_ARCHARM -D_ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE=1)
elseif(TARGET_ARCH STREQUAL "ARM64")
  add_definitions(-DPTW32_ARCHARM64 -D_ARM_WINAPI_PARTITION_DESKTOP_SDK_AVAILABLE=1)
elseif(TARGET_ARCH STREQUAL "x86_64")
  add_definitions(-DPTW32_ARCHAMD64)
elseif(TARGET_ARCH STREQUAL "x86")
  add_definitions(-DPTW32_ARCHX86)
elseif(TARGET_ARCH STREQUAL "x64")
  add_definitions(-DPTW32_ARCHX64)
else()
  message(FATAL_ERROR "\"${TARGET_ARCH}\" not supported in version.rc")
endif()

#################################
# Defs                          #
#################################
add_definitions(-DPTW32_BUILD_INLINED)

if(UNIX)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -D ENABLE_ASSERTS=1")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D ENABLE_ASSERTS=1 ")
endif()

add_definitions(-DENABLE_OVERRIDE=1)
if(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /errorReport:none /nologo")
  # C++ Exceptions
  # (Note: If you are using Microsoft VC++6.0, the library needs to be built
  # with /EHa instead of /EHs or else cancellation won't work properly.)
  if(MSVC_VERSION EQUAL 1200)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
  endif()

  add_definitions(-DHAVE_CONFIG_H -DPTW32_RC_MSC)
endif()

# Update filename with proper version info
set(PTW32_VER ${PROJECT_VERSION_MAJOR}${EXTRAVERSION})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)

#################################
# Libraries                     #
#################################

if(BUILD_SHARED_LIBS)
  add_library(pthreadVCE3 SHARED pthread-EH.cpp ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
  target_compile_definitions(pthreadVCE3 PUBLIC PTW32_CLEANUP_CXX _WINDLL)

  add_library(pthreadVSE3 SHARED pthread-EH.cpp ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
  target_compile_definitions(pthreadVSE3 PUBLIC PTW32_CLEANUP_SEH _WINDLL)

  add_library(pthreadVC3 SHARED pthread-JMP.c ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
  target_compile_definitions(pthreadVC3 PUBLIC PTW32_CLEANUP_C _WINDLL)
else()
  add_library(pthreadVCE3 STATIC pthread-EH.cpp)
  target_compile_definitions(pthreadVCE3 PUBLIC PTW32_CLEANUP_CXX PTW32_STATIC_LIB)

  add_library(pthreadVSE3 STATIC pthread-EH.cpp)
  target_compile_definitions(pthreadVSE3 PUBLIC PTW32_CLEANUP_SEH PTW32_STATIC_LIB)

  add_library(pthreadVC3 STATIC pthread-JMP.c)
  target_compile_definitions(pthreadVC3 PUBLIC PTW32_CLEANUP_C PTW32_STATIC_LIB)
endif()

target_include_directories(pthreadVCE3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(pthreadVSE3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(pthreadVC3 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Add aliases for fetchcontent
add_library(${PROJECT_NAME}::pthreadVCE3 ALIAS pthreadVCE3)
add_library(${PROJECT_NAME}::pthreadVSE3 ALIAS pthreadVSE3)
add_library(${PROJECT_NAME}::pthreadVC3 ALIAS pthreadVC3)

#################################
# Install                       #
#################################

install(FILES _ptw32.h pthread.h sched.h semaphore.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS pthreadVCE3 pthreadVSE3 pthreadVC3
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install pdb files
if(BUILD_SHARED_LIBS)
  install(FILES $<TARGET_PDB_FILE:pthreadVCE3> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
  install(FILES $<TARGET_PDB_FILE:pthreadVSE3> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
  install(FILES $<TARGET_PDB_FILE:pthreadVC3> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()

# Install CMake config files
set(${PROJECT_NAME}_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION ${${PROJECT_NAME}_INSTALL_CONFIGDIR}
  NAMESPACE ${PROJECT_NAME}::
)

include(CMakePackageConfigHelpers)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in "# Generated by CMake @CMAKE_VERSION@ for @PROJECT_NAME@

@PACKAGE_INIT@

include(\"\$\{CMAKE_CURRENT_LIST_DIR\}/@PROJECT_NAME@-targets.cmake\")
")
configure_package_config_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake.in ${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CONFIGDIR}
)

write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
  DESTINATION ${${PROJECT_NAME}_INSTALL_CONFIGDIR}
)

#################################
# Test                          #
#################################
option(BUILD_TESTING "Enable Test code build" False)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
