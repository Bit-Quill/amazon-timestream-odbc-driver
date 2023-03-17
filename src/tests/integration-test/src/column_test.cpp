/*
 * Copyright <2022> Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#include "ignite/odbc/column.h"

#include "ignite/odbc/impl/binary/binary_common.h"
#include "ignite/odbc/impl/binary/binary_writer_impl.h"

#include <boost/test/unit_test.hpp>

#include "ignite/odbc/system/odbc_constants.h"

using namespace ignite::odbc::impl::binary;
using namespace ignite::odbc::impl::interop;
using namespace ignite::odbc::app;
using namespace ignite::odbc;

BOOST_AUTO_TEST_SUITE(ColumnTestSuite)

BOOST_AUTO_TEST_CASE(TestColumnDefaultConstruction, *disabled()) {
  Column column;

  BOOST_REQUIRE(!column.IsValid());
}

BOOST_AUTO_TEST_CASE(TestColumnShort, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  int16_t data = 42;

  writer.WriteInt8(IGNITE_TYPE_SHORT);
  writer.WriteInt16(data);

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column(reader);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), 2);

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 2);

  short shortBuf = 0;
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_SIGNED_SHORT,
                               &shortBuf, sizeof(shortBuf), &reslen);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), 2);

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(shortBuf, data);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), 2);

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);
}

BOOST_AUTO_TEST_CASE(TestColumnString, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  std::string data("Some test data.");

  writer.WriteString(data.data(), static_cast< int32_t >(data.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column(reader);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), data.size());

  char strBuf[1024] = {};
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_CHAR, &strBuf,
                               sizeof(strBuf), &reslen);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(std::string(strBuf), data);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);
}

BOOST_AUTO_TEST_CASE(TestColumnStringSeveral, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  std::string data("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

  writer.WriteString(data.data(), static_cast< int32_t >(data.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column(reader);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), data.size());

  std::string res;

  std::string strBuf(data.size() / 3 + 2, 0);
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_CHAR, &strBuf[0],
                               strBuf.size(), &reslen);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_VARLEN_DATA_TRUNCATED);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(),
                      data.size() - (strBuf.size() - 1));

  res.append(strBuf.c_str());

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_VARLEN_DATA_TRUNCATED);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(),
                      data.size() - 2 * (strBuf.size() - 1));

  res.append(strBuf.c_str());

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  res.append(strBuf.c_str());

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(res, data);
}

BOOST_AUTO_TEST_CASE(TestColumnMultiString, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  std::string data1("Some test data.");
  std::string data2("Other TEST DATA.");

  writer.WriteString(data1.data(), static_cast< int32_t >(data1.size()));
  writer.WriteString(data2.data(), static_cast< int32_t >(data2.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column1(reader);

  inStream.Position(column1.GetEndPosition());

  Column column2(reader);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), data1.size());

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data2.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), data2.size());

  char strBuf[1024] = {};
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_CHAR, &strBuf,
                               sizeof(strBuf), &reslen);

  BOOST_REQUIRE_EQUAL(column1.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(std::string(strBuf), data1);

  BOOST_REQUIRE_EQUAL(column1.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(column2.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data2.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(std::string(strBuf), data2);

  BOOST_REQUIRE_EQUAL(column2.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data2.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), 0);
}

BOOST_AUTO_TEST_CASE(TestColumnByteArray, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  const int8_t bytes[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  std::vector< int8_t > data(bytes, bytes + sizeof(bytes) / sizeof(bytes[0]));
  writer.WriteInt8Array(&data[0], static_cast< int32_t >(data.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column(reader);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), data.size());

  std::vector< int8_t > buf(data.size());
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_BINARY, &buf[0],
                               buf.size(), &reslen);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL_COLLECTIONS(buf.begin(), buf.end(), data.begin(),
                                  data.end());

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);
}

BOOST_AUTO_TEST_CASE(TestColumnByteArrayHalfBuffer, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  const int8_t bytes[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  std::vector< int8_t > data(bytes, bytes + sizeof(bytes) / sizeof(bytes[0]));
  BOOST_REQUIRE_EQUAL(0, data.size() % 2);

  writer.WriteInt8Array(&data[0], static_cast< int32_t >(data.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column(reader);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), data.size());

  std::vector< int8_t > buf(data.size() / 2);
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_BINARY, &buf[0],
                               buf.size(), &reslen);

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), data.size() - buf.size());

  std::vector< int8_t > result;
  result.reserve(data.size());
  std::copy(buf.begin(), buf.end(), std::back_inserter(result));

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  std::copy(buf.begin(), buf.end(), std::back_inserter(result));

  BOOST_REQUIRE_EQUAL(column.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column.IsValid());

  BOOST_REQUIRE_EQUAL(column.GetSize(), data.size());

  BOOST_REQUIRE_EQUAL(column.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL_COLLECTIONS(result.begin(), result.end(), data.begin(),
                                  data.end());
}

BOOST_AUTO_TEST_CASE(TestColumnByteArrayTwoColumns, *disabled()) {
  InteropUnpooledMemory mem(4096);
  InteropOutputStream outStream(&mem);
  BinaryWriterImpl writer(&outStream, 0);

  const int8_t bytes1[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
  const int8_t bytes2[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
  std::vector< int8_t > data1(bytes1,
                              bytes1 + sizeof(bytes1) / sizeof(bytes1[0]));
  std::vector< int8_t > data2(bytes2,
                              bytes2 + sizeof(bytes2) / sizeof(bytes2[0]));
  writer.WriteInt8Array(&data1[0], static_cast< int32_t >(data1.size()));
  writer.WriteInt8Array(&data2[0], static_cast< int32_t >(data2.size()));

  outStream.Synchronize();

  InteropInputStream inStream(&mem);
  BinaryReaderImpl reader(&inStream);

  Column column1(reader);
  inStream.Position(column1.GetEndPosition());
  Column column2(reader);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), data1.size());

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data2.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), data2.size());

  int8_t buf[1024] = {};
  SqlLen reslen = 0;

  ApplicationDataBuffer appBuf(type_traits::OdbcNativeType::AI_BINARY, &buf,
                               sizeof(buf), &reslen);

  BOOST_REQUIRE_EQUAL(column1.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + reslen, data1.begin(),
                                  data1.end());

  BOOST_REQUIRE_EQUAL(column1.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column1.IsValid());

  BOOST_REQUIRE_EQUAL(column1.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column1.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL(column2.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_SUCCESS);

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), 0);

  BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + reslen, data2.begin(),
                                  data2.end());

  BOOST_REQUIRE_EQUAL(column2.ReadToBuffer(reader, appBuf),
                      app::ConversionResult::AI_NO_DATA);

  BOOST_REQUIRE(column2.IsValid());

  BOOST_REQUIRE_EQUAL(column2.GetSize(), data1.size());

  BOOST_REQUIRE_EQUAL(column2.GetUnreadDataLength(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
