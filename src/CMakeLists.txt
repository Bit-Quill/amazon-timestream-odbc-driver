#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to You under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.20)

# Update version number in ODBC_DRIVER_VERSION.txt file.
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/ODBC_DRIVER_VERSION.txt" ODBC_DRIVER_VERSION)
string(STRIP ${ODBC_DRIVER_VERSION} ODBC_DRIVER_VERSION)
project(Timestream-ODBC VERSION ${ODBC_DRIVER_VERSION})

if ((UNIX AND (CMAKE_SIZEOF_VOID_P EQUAL 4)) OR APPLE)
    set(s2n_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/s2n/cmake")
    set(aws-cpp-sdk-timestream-query_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/cmake/aws-cpp-sdk-timestream-query")
    set(aws-cpp-sdk-sts_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/cmake/aws-cpp-sdk-sts")
    set(aws-cpp-sdk-core_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/cmake/aws-cpp-sdk-core")
    set(aws-crt-cpp_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-crt-cpp/cmake")
    set(aws-c-http_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-http/cmake")
    set(aws-c-io_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-io/cmake")
    set(aws-c-common_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-common/cmake")
    set(aws-c-cal_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-cal/cmake")
    set(aws-c-compression_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-compression/cmake")
    set(aws-c-auth_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-auth/cmake")
    set(aws-c-mqtt_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-mqtt/cmake")
    set(aws-checksums_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-checksums/cmake")
    set(aws-c-event-stream_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-event-stream/cmake")
    set(aws-c-s3_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/aws-c-s3/cmake")
    set(AWSSDK_DIR "${CMAKE_SOURCE_DIR}/aws-sdk-cpp/install/lib/cmake/AWSSDK")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/modules/code-coverage.cmake")
if (NOT "$ENV{BOOST_ROOT}" STREQUAL "")
    set(BOOST_ROOT "$ENV{BOOST_ROOT}")
endif()
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIGNITE_IMPL -DIGNITE_FRIEND -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)

if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

add_definitions(-DUNICODE=1)
if (WIN32)

    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../build/odbc/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../build/odbc/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../build/odbc/bin")
endif()

if (MSVC)
    add_compile_options(/source-charset:utf-8 /execution-charset:utf-8)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

option (WITH_ODBC OFF)
option (WITH_ODBC_MSI OFF)
option (WITH_THIN_CLIENT OFF)
option (WITH_TESTS OFF)
option (WARNINGS_AS_ERRORS OFF)

if (${WARNINGS_AS_ERRORS})
    if (MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Wall -Wextra -Werror -Wno-variadic-macros)
    endif()
endif()

if (${WITH_ODBC})
    add_subdirectory(odbc)
endif()

if (${WITH_TESTS})
    enable_testing()

    if (EXISTS ${CMAKE_SOURCE_DIR}/tests)
        add_subdirectory(tests)
    endif()
endif()

