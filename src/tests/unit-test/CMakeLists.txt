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
         ../../odbc/src/app/application_data_buffer.cpp
         ../../odbc/src/binary/binary_containers.cpp
         ../../odbc/src/binary/binary_raw_writer.cpp
         ../../odbc/src/binary/binary_writer.cpp
         ../../odbc/src/binary/binary_reader.cpp
         ../../odbc/src/binary/binary_type.cpp
         ../../odbc/src/binary/binary_raw_reader.cpp
         ../../odbc/src/cluster/cluster_group.cpp
         ../../odbc/src/cluster/cluster_node.cpp
         ../../odbc/src/cluster/ignite_cluster.cpp
         ../../odbc/src/common_types.cpp
         ../../odbc/src/common/big_integer.cpp
         ../../odbc/src/common/bits.cpp
         ../../odbc/src/app/application_data_buffer.cpp
         ../../odbc/src/app/parameter.cpp
         ../../odbc/src/app/parameter_set.cpp
         ../../odbc/src/column.cpp
         ../../odbc/src/common/big_integer.cpp
         ../../odbc/src/common/bits.cpp
         ../../odbc/src/common/concurrent.cpp
         ../../odbc/src/common/decimal.cpp
         ../../odbc/src/common/utils.cpp
         ../../odbc/src/common_types.cpp
         ../../odbc/src/config/configuration.cpp
         ../../odbc/src/config/config_tools.cpp
         ../../odbc/src/config/connection_info.cpp
         ../../odbc/src/config/connection_string_parser.cpp
         ../../odbc/src/impl/binary/binary_type_manager.cpp
         ../../odbc/src/impl/binary/binary_type_impl.cpp
         ../../odbc/src/impl/binary/binary_utils.cpp
         ../../odbc/src/impl/binary/binary_reader_impl.cpp
         ../../odbc/src/impl/binary/binary_type_handler.cpp
         ../../odbc/src/impl/binary/binary_writer_impl.cpp
         ../../odbc/src/impl/binary/binary_schema.cpp
         ../../odbc/src/impl/binary/binary_type_snapshot.cpp
         ../../odbc/src/impl/binary/binary_object_header.cpp
         ../../odbc/src/impl/binary/binary_object_impl.cpp
         ../../odbc/src/impl/binary/binary_field_meta.cpp
         ../../odbc/src/impl/binary/binary_type_updater_impl.cpp
         ../../odbc/src/impl/cache/cache_affinity_impl.cpp
         ../../odbc/src/impl/cache/cache_impl.cpp
         ../../odbc/src/impl/cache/query/query_batch.cpp
         ../../odbc/src/impl/cache/query/query_impl.cpp
         ../../odbc/src/impl/cache/query/continuous/continuous_query_handle_impl.cpp
         ../../odbc/src/impl/cluster/cluster_group_impl.cpp
         ../../odbc/src/impl/cluster/cluster_node_impl.cpp
         ../../odbc/src/impl/cluster/ignite_cluster_impl.cpp
         ../../odbc/src/impl/compute/cancelable_impl.cpp
         ../../odbc/src/impl/compute/compute_impl.cpp
         ../../odbc/src/impl/interop/interop_input_stream.cpp
         ../../odbc/src/impl/interop/interop_memory.cpp
         ../../odbc/src/impl/interop/interop_output_stream.cpp
         ../../odbc/src/impl/interop/interop_target.cpp
         ../../odbc/src/impl/interop/interop_external_memory.cpp
         ../../odbc/src/impl/handle_registry.cpp
         ../../odbc/src/impl/ignite_binding_impl.cpp
         ../../odbc/src/impl/ignite_environment.cpp
         ../../odbc/src/impl/ignite_impl.cpp
         ../../odbc/src/auth_type.cpp
         ../../odbc/src/connection.cpp
         ../../odbc/src/cursor.cpp
         ../../odbc/src/diagnostic/diagnosable_adapter.cpp
         ../../odbc/src/diagnostic/diagnostic_record_storage.cpp
         ../../odbc/src/diagnostic/diagnostic_record.cpp
         ../../odbc/src/dsn_config.cpp
         ../../odbc/src/environment.cpp
         ../../odbc/src/ignite_error.cpp
         ../../odbc/src/jni/java.cpp
         ../../odbc/src/log.cpp
         ../../odbc/src/message.cpp
         ../../odbc/src/meta/column_meta.cpp
         ../../odbc/src/meta/foreign_key_meta.cpp
         ../../odbc/src/meta/primary_key_meta.cpp
         ../../odbc/src/meta/table_meta.cpp
         ../../odbc/src/nested_tx_mode.cpp
         ../../odbc/src/protocol_version.cpp
         ../../odbc/src/query/batch_query.cpp
         ../../odbc/src/query/column_metadata_query.cpp
         ../../odbc/src/query/data_query.cpp
         ../../odbc/src/query/foreign_keys_query.cpp
         ../../odbc/src/query/primary_keys_query.cpp
         ../../odbc/src/query/special_columns_query.cpp
         ../../odbc/src/query/type_info_query.cpp
         ../../odbc/src/query/table_metadata_query.cpp
         ../../odbc/src/sql/sql_lexer.cpp
         ../../odbc/src/sql/sql_parser.cpp
         ../../odbc/src/sql/sql_set_streaming_command.cpp
         ../../odbc/src/sql/sql_utils.cpp
         ../../odbc/src/log_level.cpp
         ../../odbc/src/result_page.cpp
         ../../odbc/src/row.cpp
         ../../odbc/src/statement.cpp
         ../../odbc/src/streaming/streaming_batch.cpp
         ../../odbc/src/streaming/streaming_context.cpp
         ../../odbc/src/type_traits.cpp
         ../../odbc/src/utility.cpp
         ../../odbc/src/date.cpp
         ../../odbc/src/guid.cpp
         ../../odbc/src/time.cpp
         ../../odbc/src/timestamp.cpp
         ../../odbc/src/ignite.cpp
         ../../odbc/src/ts_error.cpp
        )

if (WIN32)
    list(APPEND SOURCES
        ../../odbc/os/win/src/common/concurrent_os.cpp
        ../../odbc/os/win/src/common/platform_utils.cpp
        ../../odbc/src/jni/os/win/utils.cpp
        ../../odbc/os/win/src/system_dsn.cpp
        ../../odbc/os/win/src/system/ui/custom_window.cpp
        ../../odbc/os/win/src/system/ui/dsn_configuration_window.cpp
        ../../odbc/os/win/src/system/ui/window.cpp
        ../../odbc/os/win/src/common/dynamic_load_os.cpp
    )
else()
    # TODO: Ensure MacOS is portable. https://bitquill.atlassian.net/browse/AD-525
    list(APPEND SOURCES
        ../../odbc/os/linux/src/common/concurrent_os.cpp
        ../../odbc/os/linux/src/common/platform_utils.cpp
	../../odbc/os/linux/src/common/dynamic_load_os.cpp
        ../../odbc/src/jni/os/linux/utils.cpp
    )
endif()

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET} ${ODBC_LIBRARIES})
target_link_libraries(${TARGET} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_LIBRARIES} ${JNI_LIBRARIES})
target_link_libraries(${TARGET} ${AWSSDK_LINK_LIBRARIES})
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
