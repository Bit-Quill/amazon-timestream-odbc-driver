/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mock/mock_stsclient.h>
#include <mock/mock_timestream_service.h>
#include<aws/sts/model/AssumeRoleWithSAMLRequest.h>

namespace ignite {
namespace odbc {

Model::AssumeRoleWithSAMLOutcome MockSTSClient::AssumeRoleWithSAML(
    const Model::AssumeRoleWithSAMLRequest& request) const {
  // SAML assertion token that should match the token from AAD authentication
  std::string predefinedSAMLAssertion("PHNhbWxwOlJlc3BvbnNlIHhtbG5zOnNhbWxwPSJ1cm46b2FzaXM6bmFtZXM6dGM6U0FNTDoyLjA6cHJvdG9jb2wiPjxzYW1scDpTdGF0dXM-PHNhbWxwOlN0YXR1c0NvZGUgVmFsdWU9InVybjpvYXNpczpuYW1lczp0YzpTQU1MOjIuMDpzdGF0dXM6U3VjY2VzcyIvPjwvc2FtbHA6U3RhdHVzPnKK63nLQHHHrLE6JHo8L3NhbWxwOlJlc3BvbnNlPg==");

  // The outcome is determined by role arn.
  if ((request.GetRoleArn() == "arn:role:nocredentials")
      || (request.GetRoleArn() == "arn:role:checkSAMLAssertion"
             && request.GetSAMLAssertion() != predefinedSAMLAssertion)) {
    // no credentials in the result
    Model::AssumeRoleWithSAMLOutcome outcome;

    return outcome;
  } else {
    // a valid credentials in the result
    Model::AssumeRoleWithSAMLResult result;
    Model::Credentials credentials;
    const std::map< Aws::String, Aws::String >& credMap =
        MockTimestreamService::GetInstance()->GetCredentialMap();
    if (!credMap.empty()) {
      credentials.SetAccessKeyId(credMap.begin()->first);
      credentials.SetSecretAccessKey(credMap.begin()->second);
      credentials.SetSessionToken("");
    }

    result.SetCredentials(credentials);
    Model::AssumeRoleWithSAMLOutcome outcome(result);

    return outcome;
  }
}

}  // namespace odbc
}  // namespace ignite
