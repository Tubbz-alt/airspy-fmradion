# CMake definitions for SoftFM

cmake_minimum_required(VERSION 3.0.2)
project(airspy-fmradion)

# Workaround for the compilation warning
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(Threads)
find_package(PkgConfig)
find_package(Volk)

# Require ALSA only for Linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(ALSA REQUIRED)
endif()

# Find Airspy library.
pkg_check_modules(PKG_AIRSPY libairspy)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_path(AIRSPY_INCLUDE_DIR airspy.h
             HINT /usr/local/include/libairspy ${PKG_AIRSPY_INCLUDE_DIRS})
    find_library(AIRSPY_LIBRARY libairspy.a
             HINT /usr/local/lib ${PKG_AIRSPY_LIBRARY_DIRS})
    set(AIRSPY_INCLUDE_OPTION "-I/usr/local/include")
else()
    find_path(AIRSPY_INCLUDE_DIR airspy.h
             HINT ${PKG_AIRSPY_INCLUDE_DIRS})
    find_library(AIRSPY_LIBRARY libairspy.a
             HINT ${PKG_AIRSPY_LIBRARY_DIRS})
    set(AIRSPY_INCLUDE_OPTION "")
endif()
message(STATUS "libairspy: ${AIRSPY_INCLUDE_DIR}, ${AIRSPY_LIBRARY}")

# Find Airspy HF library.
pkg_check_modules(PKG_AIRSPYHF libairspyhf)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_path(AIRSPYHF_INCLUDE_DIR airspyhf.h
        HINT /usr/local/include/libairspyhf ${PKG_AIRSPYHF_INCLUDE_DIRS})
    find_library(AIRSPYHF_LIBRARY libairspyhf.a
        HINT /usr/local/lib ${PKG_AIRSPYHF_LIBRARY_DIRS})
    set(AIRSPYHF_INCLUDE_OPTION "-I/usr/local/include")
else()
    find_path(AIRSPYHF_INCLUDE_DIR airspyhf.h
             HINT ${PKG_AIRSPYHF_INCLUDE_DIRS})
    find_library(AIRSPYHF_LIBRARY libairspyhf.a
             HINT ${PKG_AIRSPYHF_LIBRARY_DIRS})
    set(AIRSPYHF_INCLUDE_OPTION "")
endif()
message(STATUS "libairspyhf: ${AIRSPYHF_INCLUDE_DIR}, ${AIRSPYHF_LIBRARY}")

# Find RTL-SDR library.
pkg_check_modules(PKG_RTLSDR librtlsdr)
find_path(RTLSDR_INCLUDE_DIR rtl-sdr.h
          HINT ${PKG_RTLSDR_INCLUDE_DIRS})
find_library(RTLSDR_LIBRARY librtlsdr.a
             HINT ${PKG_RTLSDR_LIBRARY_DIRS})
message(STATUS "librtlsdr: ${RTLSDR_INCLUDE_DIR}, ${RTLSDR_LIBRARY}")

# Find libusb
# See https://github.com/texane/stlink/blob/master/cmake/modules/FindLibUSB.cmake
pkg_check_modules(PKG_LIBUSB libusb-1.0)
find_path(LIBUSB_INCLUDE_DIR libusb.h
    HINTS /usr /usr/local /opt ${PKG_LIBUSB_INCLUDE_DIRS}
    PATH_SUFFIXES libusb-1.0)
set(LIBUSB_NAME usb-1.0)
find_library(LIBUSB_LIBRARY ${LIBUSB_NAME}
             HINTS /usr /usr/local /opt ${PKG_LIBUSB_LIBRARY_DIRS})
message(STATUS "libusb: ${LIBUSB_INCLUDE_DIR}, ${LIBUSB_LIBRARY}")

# Find libsoxr
pkg_check_modules(PKG_LIBSOXR libsoxr)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_path(LIBSOXR_INCLUDE_DIR soxr.h
        HINTS /usr/local/include ${PKG_LIBSOXR_INCLUDE_DIRS})
    find_library(LIBSOXR_LIBRARY libsoxr.dylib
        HINTS /usr/local/lib ${PKG_LIBSOXR_LIBRARY_DIRS})
else()
    find_path(LIBSOXR_INCLUDE_DIR soxr.h
        HINTS ${PKG_LIBSOXR_INCLUDE_DIRS})
    find_library(LIBSOXR_LIBRARY libsoxr.so
        HINTS ${PKG_LIBSOXR_LIBRARY_DIRS})
endif()
message(STATUS "libsoxr: ${LIBSOXR_INCLUDE_DIR}, ${LIBSOXR_LIBRARY}")

# Find sndfile library.
FIND_LIBRARY(SNDFILE_LIBRARY sndfile)

FIND_PATH(SNDFILE_INCLUDE_DIR sndfile.h
        HINT ${PKG_SNDFILE_INCLUDE_DIRS})
MESSAGE(STATUS "Sndfile: ${SNDFILE_LIBRARY}, ${SNDFILE_INCLUDE_DIR}")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ALSA_OPTION "-DUSE_ALSA")
else ()
    set(ALSA_OPTION "")
endif()

if(RTLSDR_INCLUDE_DIR AND RTLSDR_LIBRARY)
    message(STATUS "Found librtlsdr: ${RTLSDR_INCLUDE_DIR}, ${RTLSDR_LIBRARY}")
else()
    message(WARNING "Can not find Osmocom RTL-SDR library")
    message("Try again with environment variable PKG_CONFIG_PATH")
    message("or with -DRTLSDR_INCLUDE_DIR=/path/rtlsdr/include")
    message("        -DRTLSDR_LIBRARY=/path/rtlsdr/lib/librtlsdr.a")
endif()

set(RTLSDR_INCLUDE_DIRS ${RTLSDR_INCLUDE_DIR} ${LIBUSB_INCLUDE_DIR})
set(RTLSDR_LIBRARIES    ${RTLSDR_LIBRARY} ${LIBUSB_LIBRARY})

set(AIRSPY_INCLUDE_DIRS ${AIRSPY_INCLUDE_DIR} ${LIBUSB_INCLUDE_DIR})
set(AIRSPY_LIBRARIES    ${AIRSPY_LIBRARY} ${LIBUSB_LIBRARY})
set(AIRSPYHF_INCLUDE_DIRS ${AIRSPYHF_INCLUDE_DIR} ${LIBUSB_INCLUDE_DIR})
set(AIRSPYHF_LIBRARIES    ${AIRSPYHF_LIBRARY} ${LIBUSB_LIBRARY})

# Optimization flags and options.
##
# Enable speed-based optimization
# Do not apply -ffast-math; it enables -menable-no-nans
# which cancels detection functions of multipath filter abnornality!
set(OPTIMIZATION_FLAGS "-O3 -ftree-vectorize -march=native")
##
# Use conservative options when failed to run
#set(OPTIMIZATION_FLAGS "-O2")
##
# For vectorization analysis (in Clang only)
#set(OPTIMIZATION_FLAGS "-O3 -ftree-vectorize -march=native -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize")
##
# For clang profiling with optimization
#set(OPTIMIZATION_FLAGS "-O3 -ftree-vectorize -march=native -g -fprofile-instr-generate -fcoverage-mapping")
#SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-instr-generate")
##

# Common compiler flags and options.
##
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 ${OPTIMIZATION_FLAGS} ${ALSA_OPTION} ${AIRSPY_INCLUDE_OPTION} ${AIRSPYHF_INCLUDE_OPTION} ${EXTRA_FLAGS}")

set(sfmbase_SOURCES
    sfmbase/AfAgc.cpp
    sfmbase/AirspyHFSource.cpp
    sfmbase/AirspySource.cpp
    sfmbase/AmDecode.cpp
    sfmbase/AudioResampler.cpp
    sfmbase/AudioOutput.cpp
    sfmbase/ConfigParser.cpp
    sfmbase/FileSource.cpp
    sfmbase/Filter.cpp
    sfmbase/FilterParameters.cpp
    sfmbase/FmDecode.cpp
    sfmbase/IfAgc.cpp
    sfmbase/IfResampler.cpp
    sfmbase/MultipathFilter.cpp
    sfmbase/NbfmDecode.cpp
    sfmbase/PhaseDiscriminator.cpp
    sfmbase/RtlSdrSource.cpp
)

set(sfmbase_HEADERS
    include/AfAgc.h
    include/AirspyHFSource.h
    include/AirspySource.h
    include/AmDecode.h
    include/AudioResampler.h
    include/AudioOutput.h
    include/ConfigParser.h
    include/DataBuffer.h
    include/FileSource.h
    include/Filter.h
    include/FilterParameters.h
    include/FmDecode.h
    include/FourthConverterIQ.h
    include/IfAgc.h
    include/IfResampler.h
    include/MovingAverage.h
    include/MultipathFilter.h
    include/NbfmDecode.h
    include/PhaseDiscriminator.h
    include/RtlSdrSource.h
    include/Source.h
    include/SoftFM.h
    include/Utility.h
)

# Base sources

set(sfmbase_SOURCES
    ${sfmbase_SOURCES}
    ${sfmbase_HEADERS}
)

# Libraries

add_library(sfmbase STATIC
    ${sfmbase_SOURCES}
)

# Executable

add_executable(airspy-fmradion
	main.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${ALSA_INCLUDE_DIRS}
    ${EXTRA_INCLUDES}
)

target_link_libraries(airspy-fmradion
    sfmbase
    ${CMAKE_THREAD_LIBS_INIT}
    ${ALSA_LIBRARIES}
    ${VOLK_LIBRARIES}
    ${EXTRA_LIBS}
)

target_link_libraries(sfmbase
    ${LIBSOXR_LIBRARY}
    ${SNDFILE_LIBRARY}
    ${AIRSPY_LIBRARIES}
    ${AIRSPYHF_LIBRARIES}
    ${RTLSDR_LIBRARIES}
)

# Installation

install(TARGETS airspy-fmradion DESTINATION bin)
install(TARGETS sfmbase DESTINATION lib)

# End of CMakeLists.txt
