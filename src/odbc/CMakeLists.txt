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

project(timestream-odbc)

set(TARGET ${PROJECT_NAME})

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(ODBC REQUIRED)
find_package(AWSSDK REQUIRED COMPONENTS core sts timestream-query)
find_package(Boost REQUIRED)
if (UNIX AND NOT APPLE)
    find_package(ZLIB REQUIRED)
endif()

if (${CODE_COVERAGE}) 
    add_code_coverage()    
endif()

include_directories(SYSTEM ${ODBC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

include_directories(include)


set(SOURCES src/app/application_data_buffer.cpp
        src/app/parameter.cpp
        src/app/parameter_set.cpp
        src/auth_type.cpp
	src/binary/binary_containers.cpp
	src/binary/binary_raw_writer.cpp
	src/binary/binary_writer.cpp
	src/binary/binary_reader.cpp
	src/binary/binary_type.cpp
	src/binary/binary_raw_reader.cpp
        src/common_types.cpp
        src/common/big_integer.cpp
        src/common/bits.cpp
        src/common/concurrent.cpp
        src/common/decimal.cpp
        src/ignite_error.cpp
        src/common/utils.cpp
        src/config/config_tools.cpp
        src/config/configuration.cpp
        src/config/connection_info.cpp
        src/config/connection_string_parser.cpp
	src/impl/binary/binary_type_manager.cpp
	src/impl/binary/binary_type_impl.cpp
	src/impl/binary/binary_utils.cpp
	src/impl/binary/binary_reader_impl.cpp
	src/impl/binary/binary_type_handler.cpp
	src/impl/binary/binary_writer_impl.cpp
	src/impl/binary/binary_schema.cpp
	src/impl/binary/binary_type_snapshot.cpp
	src/impl/binary/binary_object_header.cpp
	src/impl/binary/binary_object_impl.cpp
	src/impl/binary/binary_field_meta.cpp
	src/impl/interop/interop_input_stream.cpp
	src/impl/interop/interop_memory.cpp
	src/impl/interop/interop_output_stream.cpp
	src/impl/handle_registry.cpp
        src/connection.cpp
        src/cursor.cpp
        src/diagnostic/diagnosable_adapter.cpp
        src/diagnostic/diagnostic_record.cpp
        src/diagnostic/diagnostic_record_storage.cpp
        src/environment.cpp
        src/meta/column_meta.cpp
        src/meta/foreign_key_meta.cpp
        src/meta/primary_key_meta.cpp
        src/meta/table_meta.cpp
        src/odbc.cpp
        src/entry_points.cpp
        src/dsn_config.cpp
        src/query/column_metadata_query.cpp
        src/query/data_query.cpp
        src/query/batch_query.cpp
        src/query/foreign_keys_query.cpp
        src/query/primary_keys_query.cpp
        src/query/table_metadata_query.cpp
        src/query/type_info_query.cpp
        src/query/special_columns_query.cpp
        src/sql/sql_parser.cpp
        src/sql/sql_lexer.cpp
        src/sql/sql_set_streaming_command.cpp
        src/sql/sql_utils.cpp
        src/streaming/streaming_batch.cpp
        src/streaming/streaming_context.cpp
        src/ssl_mode.cpp
        src/protocol_version.cpp
        src/result_page.cpp
        src/row.cpp
        src/nested_tx_mode.cpp
        src/message.cpp
        src/column.cpp
        src/statement.cpp
        src/type_traits.cpp
        src/utility.cpp
        src/log.cpp
        src/log_level.cpp
        src/date.cpp
        src/guid.cpp
        src/time.cpp
        src/timestamp.cpp
        src/ts_error.cpp)

if (WIN32)
    set(OS_INCLUDE os/win/include)

    list(APPEND SOURCES 
            os/win/src/system_dsn.cpp
            os/win/src/system/ui/custom_window.cpp
            os/win/src/system/ui/dsn_configuration_window.cpp
            os/win/src/system/ui/window.cpp
            os/win/src/common/concurrent_os.cpp
            os/win/src/common/platform_utils.cpp
            os/win/src/common/dynamic_load_os.cpp
            module.def
    )
else()
    set(OS_INCLUDE os/linux/include)

    list(APPEND SOURCES 
            os/linux/src/common/concurrent_os.cpp
            os/linux/src/common/platform_utils.cpp
            os/linux/src/common/dynamic_load_os.cpp
    )
endif ()

include_directories(${OS_INCLUDE})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${TARGET} SHARED ${SOURCES} version.rc)

set_target_properties(${TARGET} PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})

target_link_libraries(${TARGET} ${ODBC_LIBRARIES})
target_link_libraries(${TARGET} ${AWSSDK_LINK_LIBRARIES})

add_definitions(-DUNICODE=1)
if (WIN32)
    target_link_libraries(${TARGET} odbccp32 shlwapi)

    add_definitions(-DTARGET_MODULE_FULL_NAME="$<TARGET_FILE_NAME:${TARGET}>")

    if (MSVC_VERSION GREATER_EQUAL 1900)
        target_link_libraries(${TARGET} legacy_stdio_definitions)
    endif()
                
    set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "timestream.odbc")
else()
    target_link_libraries(${TARGET})
endif()

if (WIN32 AND ${WITH_ODBC_MSI})
    find_program(WIX_CANDLE candle)
    if(NOT WIX_CANDLE)
        message(FATAL_ERROR "WIX candle.exe not found! Have you installed WIX Toolset or forgotten to add it to Path?")
    endif()

    find_program(WIX_LIGHT light)
    if(NOT WIX_LIGHT)
        message(FATAL_ERROR "WIX light.exe not found! Have you installed WIX Toolset or forgotten to add it to Path?")
    endif()

    set(WIX_ODBC_LIBRARY_PATH ".\\timestream.odbc.dll")
    set(WIX_ODBC_SDK_CORE_PATH ".\\aws-cpp-sdk-core.dll")
    set(WIX_ODBC_SDK_TS_QUERY_PATH ".\\aws-cpp-sdk-timestream-query.dll")
    set(WIX_ODBC_AWS_C_AUTH ".\\aws-c-auth.dll")
    set(WIX_ODBC_AWS_C_CAL ".\\aws-c-cal.dll")
    set(WIX_ODBC_AWS_C_COMMON ".\\aws-c-common.dll")
    set(WIX_ODBC_AWS_C_COMPRESSION ".\\aws-c-compression.dll")
    set(WIX_ODBC_AWS_C_EVENT_STREAM ".\\aws-c-event-stream.dll")
    set(WIX_ODBC_AWS_CHECKSUMS ".\\aws-checksums.dll")
    set(WIX_ODBC_AWS_C_HTTP ".\\aws-c-http.dll")
    set(WIX_ODBC_AWS_C_IO ".\\aws-c-io.dll")
    set(WIX_ODBC_AWS_C_MQTT ".\\aws-c-mqtt.dll")
    set(WIX_ODBC_AWS_CRT_CPP ".\\aws-crt-cpp.dll")
    set(WIX_ODBC_AWS_C_S3 ".\\aws-c-s3.dll")

    set(WIX_PROJECT_NAME "Amazon Timestream ODBC driver")
    set(WIX_MANUFACTURER "Amazon Web Services")
    set(WIX_COPYRIGHT_COMMENT "Apache, Apache Ignite, the Apache feather and the Apache Ignite logo are trademarks of The Apache Software Foundation.")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(WIX_BIT_SUFFIX "64-bit")
        set(WIX_PRODUCT_ID "942F102A-BCF5-46BE-A568-8A0A544F6456")
        set(WIX_UPGRADE_CODE "F17BF88F-9571-47FE-A642-5B0ECA26DFBD")
        set(WIX_COMPONENT_ID_ODBC_LIBRARY "AA8B007D-C9B9-4F1D-9C9F-E9384401E214")
        set(WIX_COMPONENT_ID_SDK_CORE "9AAED0BC-281E-454A-8DDB-76787A46573D")
        set(WIX_COMPONENT_ID_SDK_TS_QUERY "268BB95C-696C-46BC-89F9-7129380A3825")

        set(WIX_COMPONENT_ID_AWS_C_AUTH "ADCC5700-9051-449B-9C7E-9C765458BE3E")
        set(WIX_COMPONENT_ID_AWS_C_CAL "577B1441-D084-4100-9A39-317701DA8908")
        set(WIX_COMPONENT_ID_AWS_C_COMMON "8413E31F-8FA1-4E58-AF19-128EF8F71E8A")
        set(WIX_COMPONENT_ID_AWS_C_COMPRESSION "7E16BE95-F650-498D-918E-0A53186D6D44")
        set(WIX_COMPONENT_ID_AWS_C_EVENT_STREAM "2955CF86-507C-41E7-A681-A9FBC8CE0AE0")
        set(WIX_COMPONENT_ID_AWS_CHECKSUMS "2D9C91D3-1D14-40E0-91A7-E9F1F6BCCB0D")
        set(WIX_COMPONENT_ID_AWS_C_HTTP "0E0EB1A2-3150-4978-86DD-210CE49BBF73")
        set(WIX_COMPONENT_ID_AWS_C_IO "BD661102-356F-426E-B5C2-47A9A2C23E13")
        set(WIX_COMPONENT_ID_AWS_C_MQTT "21D27FB0-7C0A-4E80-881D-DEA2DBD085FC")
        set(WIX_COMPONENT_ID_AWS_CRT_CPP "475234A5-AF23-4E7C-80AA-066B30522C58")
        set(WIX_COMPONENT_ID_AWS_C_S3 "E35D33AB-3C50-469E-8A60-4E4B266D03A9")

        set(WIX_PROGRAM_FILES_FOLDER_ID "Program Files (x86)")
        set(WIX_COMPONENT_IS_WIN64 "yes")
        set(WIX_PACKAGE_PLATFORM "x64")
        set(WIX_INSTALLER_PREFIX ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/timestream-odbc-amd64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(WIX_BIT_SUFFIX "32-bit")
        set(WIX_PRODUCT_ID "9FAACB44-B1D9-4E9B-B22E-44FB20C083E2")
        set(WIX_UPGRADE_CODE "7BDC7F9E-BB0D-4288-B404-96A78AB0B69D")
        set(WIX_COMPONENT_ID_ODBC_LIBRARY "F0B4778F-43BD-447C-B882-E70D9B16AEB6")
        set(WIX_COMPONENT_ID_SDK_CORE "1CD3936E-F8CF-42B9-90F9-7F33BC3E1CC1")
        set(WIX_COMPONENT_ID_SDK_TS_QUERY "4E88F45B-7623-4A80-9985-0AC2F81F371E")
        set(WIX_COMPONENT_ID_AWS_C_AUTH "5376FD5A-D940-4FAC-B666-F9F37BBCA80E")
        set(WIX_COMPONENT_ID_AWS_C_CAL "875C5826-F555-4929-BFA5-819096E96437")
        set(WIX_COMPONENT_ID_AWS_C_COMMON "0DDAC317-5752-4391-9D5C-0B855F964DF9")
        set(WIX_COMPONENT_ID_AWS_C_COMPRESSION "34DE86DB-D1D5-4FE1-B13B-3C0BAD06C784")
        set(WIX_COMPONENT_ID_AWS_C_EVENT_STREAM "325A20AC-E459-4ABC-BFC5-E1680951EEE4")
        set(WIX_COMPONENT_ID_AWS_CHECKSUMS "7A70A03F-7128-4AAC-9556-0FDB536FEFED")
        set(WIX_COMPONENT_ID_AWS_C_HTTP "53D36BAF-F663-4799-8B75-BD8C643BB451")
        set(WIX_COMPONENT_ID_AWS_C_IO "922C1B88-9A36-4CE5-A770-EB10F7EF1FD9")
        set(WIX_COMPONENT_ID_AWS_C_MQTT "8F43AF91-BB7B-489D-9707-292497BBB21E")
        set(WIX_COMPONENT_ID_AWS_CRT_CPP "DB6DCE34-B717-43C1-A53C-EC36227B1B01")
        set(WIX_COMPONENT_ID_AWS_C_S3 "E382CA06-AEE1-48D3-95D4-D712A832AC4D")
        set(WIX_PROGRAM_FILES_FOLDER_ID "ProgramFilesFolder")
        set(WIX_COMPONENT_IS_WIN64 "no")
        set(WIX_PACKAGE_PLATFORM "x86")
        set(WIX_INSTALLER_PREFIX ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/timestream-odbc-x86)
    endif()

    set(WIX_PRODUCT_NAME "${WIX_PROJECT_NAME} ODBC ${WIX_BIT_SUFFIX} Driver")

    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/install/timestream-odbc.wxs.in"
        "${WIX_INSTALLER_PREFIX}.wxs"
        @ONLY
    )

    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${WIX_CANDLE} ${WIX_INSTALLER_PREFIX}.wxs -out ${WIX_INSTALLER_PREFIX}.wxobj
        COMMAND ${WIX_LIGHT} -ext WixUIExtension ${WIX_INSTALLER_PREFIX}.wxobj -out ${WIX_INSTALLER_PREFIX}.msi
    )
endif()

target_include_directories(${TARGET} INTERFACE include)

install(TARGETS ${TARGET} 
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

if (WIN32 AND ${WITH_ODBC_MSI})
   install(FILES ${WIX_INSTALLER_PREFIX}.msi DESTINATION bin)
endif()

if (WIN32)
    if (MSVC_VERSION GREATER_EQUAL 1900)
        target_link_libraries(${TARGET} legacy_stdio_definitions odbccp32 shlwapi)
    endif()
elseif(APPLE)
    target_link_libraries(${TARGET} iodbcinst)
else()
    target_link_libraries(${TARGET} odbcinst)
endif()

set(VERSIONINFO ${CMAKE_PROJECT_VERSION_MAJOR},${CMAKE_PROJECT_VERSION_MINOR},${CMAKE_PROJECT_VERSION_PATCH})
set(VERSIONINFO_STR ${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH})
configure_file("version.rc.in" "version.rc" @ONLY)
