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

#include "ignite/odbc/idp_name.h"

#include <ignite/odbc/common/utils.h>

namespace ignite {
namespace odbc {
IdpName::Type IdpName::FromString(const std::string& val, Type dflt) {
  std::string lowerVal = common::ToLower(val);

  common::StripSurroundingWhitespaces(lowerVal);

  if (lowerVal == "none")
    return IdpName::Type::NONE;

  if (lowerVal == "okta")
    return IdpName::Type::OKTA;

  if (lowerVal == "azuread")
    return IdpName::Type::AAD;

  return dflt;
}

std::string IdpName::ToString(Type val) {
  switch (val) {
    case IdpName::Type::OKTA:
      return "okta";

    case IdpName::Type::AAD:
      return "azuread";

    case IdpName::Type::NONE:
      return "none";

    default:
      return "unknown";
  }
}
}  // namespace odbc
}  // namespace ignite
