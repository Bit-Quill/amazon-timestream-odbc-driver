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

#ifndef _IGNITE_ODBC_MOCK_HTTPCLIENT
#define _IGNITE_ODBC_MOCK_HTTPCLIENT

#include <aws/core/Aws.h>
#include "aws/core/http/HttpClient.h"

using namespace Aws::Http;

namespace ignite {
namespace odbc {
/**
 * Mock HttpClient so we could control the HttpResponse
 */
class MockHttpClient : public HttpClient {
 public:
  /**
   * Constructor.
   */
  MockHttpClient() = default;

  /**
   * Destructor.
   */
  ~MockHttpClient() = default;

  /**
   * Handle HttpRequest to generate a mocked HttpResponse
   *
   * @param request The http request.
   * @param readLimiter Read limiter.
   * @param writeLimiter Write limiter.
   * @return Mocked HttpResponse.
   */
  virtual std::shared_ptr< HttpResponse > MakeRequest(
      const std::shared_ptr< HttpRequest >& request,
      Aws::Utils::RateLimits::RateLimiterInterface* readLimiter = nullptr,
      Aws::Utils::RateLimits::RateLimiterInterface* writeLimiter =
          nullptr) const;

 private:
  /**
   * Handle session token request
   *
   * @param request The http request.
   * @param response The generated HttpResponse with session token
   */
  void HandleSessionTokenRequest(
      const std::shared_ptr< HttpRequest >& request,
      std::shared_ptr< HttpResponse >& response) const;

  /**
   * Handle Azure AD access token request
   *
   * @param request The http request.
   * @param response The generated HttpResponse with access token
   */
  void HandleAADAccessTokenRequest(
      const std::shared_ptr< HttpRequest >& request,
      std::shared_ptr< HttpResponse >& response) const;

  /**
   * Handle SAML Get request to get SAMLResponse
   *
   * @param path The http request uri path.
   * @param request The http SAML get request
   * @param response The http response with SAMLResponse
   */
  void HandleSAMLAssertion(const Aws::String& path,
                           const std::shared_ptr< HttpRequest >& request,
                           std::shared_ptr< HttpResponse >& response) const;
};
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_MOCK_HTTPCLIENT
