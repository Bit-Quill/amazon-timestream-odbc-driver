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

#ifndef _IGNITE_ODBC_AUTHENTICATION_AAD
#define _IGNITE_ODBC_AUTHENTICATION_AAD

#include "ignite/odbc/authentication/saml.h"

namespace ignite {
namespace odbc {

class IGNITE_IMPORT_EXPORT TimestreamAADCredentialsProvider
    : public TimestreamSAMLCredentialsProvider {
 public:
  /**
   * Constructor.
   *
   * @param config Configuration object reference
   * @param httpClient Shared pointer to httpClient
   * @param stsClient Shared pointer to STSClient
   */
  TimestreamAADCredentialsProvider(const config::Configuration& config,
      std::shared_ptr< Aws::Http::HttpClient > httpClient,
      std::shared_ptr< Aws::STS::STSClient > stsClient)
      : TimestreamSAMLCredentialsProvider(config, httpClient, stsClient) {
    //No-op.
  }

 protected:
  /*
   * Get SAML assertion response. Called by SAMLCredentialsProvider 
   * @param errInfo Error message when there is a failure
   */
  virtual std::string GetSAMLAssertion(std::string& errInfo);

 private:
  /*
   * Get access token using values from config. Called by GetSAMLAssertion
   * Empty string is returned if unable to obtain access token
   * @param errInfo Error message when there is a failure
   */
  std::string GetAccessToken(std::string& errInfo);
};

}  // namespace odbc
}  // namespace ignite

#endif //_IGNITE_ODBC_AUTHENTICATION_AAD
