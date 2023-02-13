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
project(timestream-populate-data)

set(TARGET ${PROJECT_NAME})

find_package(AWSSDK REQUIRED COMPONENTS core sts timestream-query timestream-write)
if (UNIX AND NOT APPLE)
    find_package(ZLIB REQUIRED)
endif()

find_package(Threads REQUIRED)

include_directories(include ../../odbc/include ../integration-test/include ${AWSSDK_INCLUDE_DIRS})
if (WIN32)
    include_directories(../../odbc/os/win/include)

    # ensure ourself build aws sdk library could be found
    set_target_properties(aws-cpp-sdk-core aws-cpp-sdk-sts aws-cpp-sdk-timestream-query aws-cpp-sdk-timestream-write PROPERTIES
      IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/../build/aws-sdk/install/bin
    )
    set_target_properties(AWS::aws-crt-cpp AWS::aws-c-cal AWS::aws-c-auth AWS::aws-c-common AWS::aws-c-io AWS::aws-checksums AWS::aws-c-event-stream AWS::aws-c-s3  AWS::aws-c-http AWS::aws-c-mqtt AWS::aws-c-compression PROPERTIES
      IMPORTED_IMPLIB ${CMAKE_SOURCE_DIR}/../build/aws-sdk/install/lib
    )

else ()
    # TODO: Ensure MacOS is portable. https://bitquill.atlassian.net/browse/AD-525
    include_directories(../../odbc/os/linux/include)
endif()

set(SOURCES 
         timestream_data_generator.cpp 
	 timestream_writer.cpp)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} ${AWSSDK_LINK_LIBRARIES})
