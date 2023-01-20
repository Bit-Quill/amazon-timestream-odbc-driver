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

#ifndef _IGNITE_ODBC_SAML
#define _IGNITE_ODBC_SAML

#include "ignite/odbc/config/configuration.h"

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/sts/model/AssumeRoleWithSAMLRequest.h>
#include <aws/sts/STSClient.h>

namespace ignite {
namespace odbc {

class IGNITE_IMPORT_EXPORT TimestreamSAMLCredentialsProvider {
 public:
  /**
   * Constructor.
   *
   * @param config Configuration object reference
   * @param stsClient Shared pointer to STSClient
   */
  TimestreamSAMLCredentialsProvider(const config::Configuration& config,
                                    std::shared_ptr< Aws::STS::STSClient > stsClient)
      : config_(config), stsClient_(stsClient) {
    // No-op.
  }

  /**
   * Get AWSCredentials.
   *
   * @param credentials Credentials returned from AWS.
   * @param errInfo Error message when there is a failure
   * @return @c true on success and @c false otherwise.
   */
  bool GetAWSCredentials(Aws::Auth::AWSCredentials& credentials, std::string& errInfo);

  /**
   * Get SAML asseration which is a base64 encoded SAML authentication response provided by the IdP
   * 
   * @param errInfo Error message when there is a failure
   * @return SAML assertion value.
   */
  virtual std::string GetSAMLAssertion(std::string& errInfo) = 0;

 protected:
  /**
   * Get AWSCredentials based on SAML assertion
   *
   * @param samlRequest AssumeRoleWithSAMLRequest.
   * @param credentials Credentials returned from AWS.
   * @param errInfo Error message when there is a failure
   * @return @c true on success and @c false otherwise.
   */
  bool FetchCredentialsWithSAMLAssertion(
      Aws::STS::Model::AssumeRoleWithSAMLRequest& samlRequest,
      Aws::Auth::AWSCredentials& credentials,
      std::string& errInfo);

  /** Configuration object */
  config::Configuration config_;

  /** STSClient pointer */
  std::shared_ptr< Aws::STS::STSClient > stsClient_;
};

}  // namespace odbc
}  // namespace ignite

#endif //_IGNITE_ODBC_SAML
