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

#include "performance_helper.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

std::string sqltcharToStr(const SQLTCHAR* sqltchar) {
    if (sizeof(SQLTCHAR) == 2) {
        std::u16string temp((const char16_t*) sqltchar);
        return std::wstring_convert< std::codecvt_utf8_utf16< char16_t >,
            char16_t >{}.to_bytes(temp);
    } else if (sizeof(SQLTCHAR) == 4) {
        std::u32string temp((const char32_t*) sqltchar);
        return std::wstring_convert< std::codecvt_utf8< char32_t >,
             char32_t >{}.to_bytes(temp);
    } else {
        return std::string((const char*) sqltchar);
    }
}

void logDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle, SQLRETURN ret,
                       SQLTCHAR* msgReturn, const SQLSMALLINT size) {
    if (!handle) {
        std::cout << "logDiagnostics failed: handle is null\n";
        return;
    }

    if (ret == SQL_SUCCESS_WITH_INFO) {
        std::cout << "SQL_SUCCESS_WITH_INFO: ";
    } else if (ret == SQL_ERROR) {
        std::cout << "SQL_ERROR: ";
    } else if (ret == SQL_NO_DATA) {
        std::cout << "SQL_NO_DATA: ";
    } else {
        return;
    }

    SQLRETURN diagRet;
    SQLTCHAR sqlState[6] = {};
    SQLINTEGER errorCode;
    SQLTCHAR diagMessage[SQL_MAX_MESSAGE_LENGTH] = {};
    SQLSMALLINT messageLen;

    SQLSMALLINT recordNumber = 0;
    do {
        recordNumber++;
        diagRet = SQLGetDiagRec(
            handleType, handle, recordNumber, sqlState, &errorCode,
            msgReturn == nullptr ? diagMessage : msgReturn,
            msgReturn == nullptr ? HELPER_SIZEOF(diagMessage) : size, &messageLen);
        std::string diagStr = sqltcharToStr((msgReturn == nullptr) ? diagMessage : msgReturn);
        std::string stateStr = sqltcharToStr(sqlState);
        if (diagRet == SQL_INVALID_HANDLE) {
            std::cout << "Invalid handle\n";
        } else if (SQL_SUCCEEDED(diagRet)) {
            std::cout << "SQLState: " + stateStr << ": " << diagStr << std::endl;
        }
    } while (diagRet == SQL_SUCCESS);

    if (diagRet == SQL_NO_DATA && recordNumber == 1) {
        std::cout << "No error information" << std::endl;
    }
}

