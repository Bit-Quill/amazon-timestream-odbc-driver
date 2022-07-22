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

#include "ignite/odbc/cred_prov_class.h"

#include <ignite/odbc/common/utils.h>

namespace ignite {
namespace odbc {
CredProvClass::Type CredProvClass::FromString(const std::string& val, Type dflt) {
  std::string lowerVal = common::ToLower(val);

  if (lowerVal == "none")
    return CredProvClass::Type::NONE;

  if (lowerVal == "propertiesfilecredentialsprovider")
    return CredProvClass::Type::PROP_FILE_CRED_PROV;

  if (lowerVal == "instanceprofilecredentialsprovider")
    return CredProvClass::Type::INST_PROF_CRED_PROV;

  return dflt;
}

std::string CredProvClass::ToString(Type val) {
  switch (val) {
    case CredProvClass::Type::PROP_FILE_CRED_PROV:
      return "propertiesfilecredentialsprovider";

    case CredProvClass::Type::INST_PROF_CRED_PROV:
      return "instanceprofilecredentialsprovider";

    case CredProvClass::Type::NONE:
      return "none";

    default:
      return "unknown";
  }
}
}  // namespace odbc
}  // namespace ignite
