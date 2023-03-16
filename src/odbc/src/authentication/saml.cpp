
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

#include "ignite/odbc/log.h"
#include "ignite/odbc/authentication/saml.h"

#include <aws/sts/model/Credentials.h>

namespace ignite {
namespace odbc {

bool TimestreamSAMLCredentialsProvider::FetchCredentialsWithSAMLAssertion(
    Aws::STS::Model::AssumeRoleWithSAMLRequest& samlRequest,
    Aws::Auth::AWSCredentials& awsCredentials,
    std::string& errInfo) {
  LOG_DEBUG_MSG("FetchCredentialsWithSAMLAssertion is called");

  Aws::STS::Model::AssumeRoleWithSAMLOutcome outcome =
      stsClient_->AssumeRoleWithSAML(samlRequest);

  bool retval = false;
  if (outcome.IsSuccess()) {
    const Aws::STS::Model::Credentials& credentials =
        outcome.GetResult().GetCredentials();

    awsCredentials.SetAWSAccessKeyId(credentials.GetAccessKeyId());
    awsCredentials.SetAWSSecretKey(credentials.GetSecretAccessKey());
    awsCredentials.SetSessionToken(credentials.GetSessionToken());

    retval = true;
  } else {
    auto error = outcome.GetError();
    errInfo = "Failed to fetch credentials, ERROR: " + error.GetExceptionName() + ": "
                                          + error.GetMessage();
    LOG_ERROR_MSG(errInfo);
  }

  return retval;
}

bool TimestreamSAMLCredentialsProvider::GetAWSCredentials(
    Aws::Auth::AWSCredentials& credentials,
    std::string& errInfo) {
  LOG_DEBUG_MSG("GetAWSCredentials is called");

  std::string samlAsseration = GetSAMLAssertion(errInfo);
  bool retval = false;
  if (samlAsseration.empty()) {
    LOG_ERROR_MSG("Failed to get SAML asseration.");
  } else {
    Aws::STS::Model::AssumeRoleWithSAMLRequest samlRequest;
    samlRequest.WithRoleArn(config_.GetRoleArn().c_str())
        .WithSAMLAssertion(samlAsseration)
        .WithPrincipalArn(config_.GetIdPArn().c_str());

    retval = FetchCredentialsWithSAMLAssertion(samlRequest, credentials, errInfo);
  }

  return retval;
}

}  // namespace odbc
}  // namespace ignite
