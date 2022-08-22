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
project(timestream-odbc-unit-tests)

set(TARGET ${PROJECT_NAME})

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost 1.53 REQUIRED COMPONENTS unit_test_framework chrono thread system regex)

find_package(ODBC REQUIRED)
find_package(AWSSDK REQUIRED COMPONENTS core sts timestream-query)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
if (UNIX AND NOT APPLE)
    find_package(ZLIB REQUIRED)
endif()
include(UseJava)

find_package(Threads REQUIRED)

include_directories(SYSTEM ${ODBC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${JNI_INCLUDE_DIRS})
include_directories(include ../../odbc/include ../integration-test/include)
if (WIN32)
    include_directories(../../odbc/os/win/include)
else ()
    # TODO: Ensure MacOS is portable. https://bitquill.atlassian.net/browse/AD-525
    include_directories(../../odbc/os/linux/include)
endif()

set(SOURCES 
         src/column_meta_test.cpp
         src/configuration_test.cpp
         src/log_test.cpp
         src/unit_connection_string_parser_test.cpp
         src/unit_connection_test.cpp
         src/utility_test.cpp
         src/odbc_unit_test_suite.cpp
         src/mock/mock_environment.cpp
         src/mock/mock_connection.cpp
         src/mock/mock_statement.cpp
         src/mock/mock_timestream_query_client.cpp
         src/mock/mock_timestream_service.cpp
        )

add_executable(${TARGET} ${SOURCES})
add_dependencies(timestream-odbc-unit-tests timestream-odbc)

target_link_libraries(${TARGET} ${ODBC_LIBRARIES})
target_link_libraries(${TARGET} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_LIBRARIES} ${JNI_LIBRARIES})
target_link_libraries(${TARGET} ${AWSSDK_LINK_LIBRARIES})
if (WIN32)
    target_link_libraries(${TARGET} ${CMAKE_BINARY_DIR}/odbc/${CMAKE_BUILD_TYPE}/timestream.odbc.lib)
elseif(APPLE)
    target_link_libraries(${TARGET} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtimestream-odbc.dylib)
else()
    target_link_libraries(${TARGET} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtimestream-odbc.so)
endif()

if (${CODE_COVERAGE}) 
    target_code_coverage(${TARGET} AUTO)
endif()

add_definitions(-DUNICODE=1)
if (WIN32)
    add_definitions(-DTARGET_MODULE_FULL_NAME="$<TARGET_FILE_NAME:${TARGET}>")
    if (MSVC_VERSION GREATER_EQUAL 1900)
        target_link_libraries(${TARGET} legacy_stdio_definitions odbccp32 shlwapi)
    endif()
elseif(APPLE)
    add_definitions(-DBOOST_TEST_DYN_LINK)
    target_link_libraries(${TARGET} iodbcinst ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
else()
    add_definitions(-DBOOST_TEST_DYN_LINK)
    target_link_libraries(${TARGET} odbcinst ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

set(TEST_TARGET IgniteOdbcUnitTest)

add_test(NAME ${TEST_TARGET} COMMAND ${TARGET} --catch_system_errors=no --log_level=all)

set_tests_properties(${TEST_TARGET} PROPERTIES ENVIRONMENT IGNITE_NATIVE_TEST_ODBC_CONFIG_PATH=${PROJECT_SOURCE_DIR}/config)
