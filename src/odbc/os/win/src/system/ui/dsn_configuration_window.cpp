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

#include "ignite/odbc/system/ui/dsn_configuration_window.h"

#include <Shlwapi.h>
#include <Windowsx.h>
#include <commctrl.h>

#include "ignite/odbc/config/config_tools.h"
#include "ignite/odbc/cred_prov_class.h"
#include "ignite/odbc/log.h"
#include "ignite/odbc/log_level.h"
#include "ignite/odbc/idp_name.h"

// TODO Add ComboBox for AWS log level configuration
// https://bitquill.atlassian.net/browse/AT-1054

namespace ignite {
namespace odbc {
namespace system {
namespace ui {
DsnConfigurationWindow::DsnConfigurationWindow(Window* parent,
                                               config::Configuration& config)
    : CustomWindow(parent, L"IgniteConfigureDsn",
                   L"Configure Amazon Timestream DSN"),
      width(450),
      height(455),
      nameEdit(),
      nameLabel(),
      nameBalloon(),
      endpointEdit(),
      endpointLabel(),
      regionEdit(),
      regionLabel(),
      regionBalloon(),
      enableMetadataPrepStmtCheckbox(),
      tabs(),
      accessKeyIdEdit(),
      accessKeyIdLabel(),
      secretAccessKeyEdit(),
      secretAccessKeyLabel(),
      sessionTokenEdit(),
      sessionTokenLabel(),
      credProvClassComboBox(),
      credProvClassLabel(),
      cusCredFileEdit(),
      cusCredFileLabel(),
      cusCredFileBalloon(),
      idpNameComboBox(),
      idpNameLabel(),
      idpHostEdit(),
      idpHostLabel(),
      idpUserNameEdit(),
      idpUserNameLabel(),
      idpPasswordEdit(),
      idpPasswordLabel(),
      idpArnEdit(),
      idpArnLabel(),
      oktaAppIdEdit(),
      oktaAppIdLabel(),
      roleArnEdit(),
      roleArnLabel(),
      aadAppIdEdit(),
      aadAppIdLabel(),
      aadClientSecretEdit(),
      aadClientSecretLabel(),
      aadTenantEdit(),
      aadTenantLabel(),
      reqTimeoutEdit(),
      reqTimeoutLabel(),
      socketTimeoutEdit(),
      socketTimeoutLabel(),
      maxRetryCountClientEdit(),
      maxRetryCountClientLabel(),
      maxConnectionsEdit(),
      maxConnectionsLabel(),
      maxConnectionsBalloon(),
      logLevelComboBox(),
      logLevelLabel(),
      logPathEdit(),
      logPathLabel(),
      okButton(),
      cancelButton(),
      config(config),
      accepted(false),
      created(false),
      shownNameBalloon(false),
      shownRegBalloon(false),
      shownCusCredFileBalloon(false),
      shownMaxConBalloon(false),
      preSel(TabIndex::Type::BASIC_AUTH) {
  // No-op.
}

DsnConfigurationWindow::~DsnConfigurationWindow() {
  // No-op.
}

void DsnConfigurationWindow::Create() {
  // Finding out parent position.
  RECT parentRect;
  GetWindowRect(parent->GetHandle(), &parentRect);

  // Positioning window to the center of parent window.
  const int posX =
      parentRect.left + (parentRect.right - parentRect.left - width) / 2;
  const int posY =
      parentRect.top + (parentRect.bottom - parentRect.top - height) / 2;

  RECT desiredRect = {posX, posY, posX + width, posY + height};
  AdjustWindowRect(&desiredRect,
                   WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, FALSE);

  Window::Create(WS_OVERLAPPED | WS_SYSMENU, desiredRect.left, desiredRect.top,
                 desiredRect.right - desiredRect.left,
                 desiredRect.bottom - desiredRect.top, 0);

  if (!handle) {
    std::stringstream buf;

    buf << "Can not create window, error code: " << GetLastError();

    throw IgniteError(IgniteError::IGNITE_ERR_GENERIC, buf.str().c_str());
  }
}

void DsnConfigurationWindow::OnCreate() {
  int groupPosYLeft = MARGIN;
  int tabGroupPosY = groupPosYLeft;
  int groupSizeY = width - 2 * MARGIN;

  // create column group settings
  tabGroupPosY += +CreateBasicSettingsGroup(MARGIN, groupPosYLeft, groupSizeY);
  CreateBasicAuthSettingsGroup(MARGIN, tabGroupPosY, groupSizeY);
  CreateConnectionSettingsGroup(MARGIN, tabGroupPosY, groupSizeY);

  // Advance Authentication group is the tallest UI group, so the positions of
  // Ok button and Cancel button will be right below the end of Advance Auth
  groupPosYLeft +=
      tabGroupPosY
      + CreateAdvanceAuthSettingsGroup(MARGIN, tabGroupPosY, groupSizeY);
  CreateLogSettingsGroup(MARGIN, tabGroupPosY, groupSizeY);

  // Hide all tab window except Basic Authentication UI.
  // Basic Authentication UI tab is selected by default.
  ShowAdvanceAuth(false);
  ShowConnectionSettings(false);
  ShowLogSettings(false);

  int cancelPosX = width - MARGIN - BUTTON_WIDTH;
  int okPosX = cancelPosX - INTERVAL - BUTTON_WIDTH;

  okButton = CreateButton(okPosX, groupPosYLeft, BUTTON_WIDTH, BUTTON_HEIGHT,
                          L"Ok", ChildId::OK_BUTTON);
  cancelButton = CreateButton(cancelPosX, groupPosYLeft, BUTTON_WIDTH,
                              BUTTON_HEIGHT, L"Cancel", ChildId::CANCEL_BUTTON);

  // check whether the required fields are filled. If not, Ok button is
  // disabled.
  created = true;
  okButton->SetEnabled(nameEdit->HasText());
}

void DsnConfigurationWindow::ShowBasicAuth(bool visble) const {
  ShowWindow(accessKeyIdEdit->GetHandle(), visble);
  ShowWindow(accessKeyIdLabel->GetHandle(), visble);
  ShowWindow(secretAccessKeyEdit->GetHandle(), visble);
  ShowWindow(secretAccessKeyLabel->GetHandle(), visble);
  ShowWindow(sessionTokenEdit->GetHandle(), visble);
  ShowWindow(sessionTokenLabel->GetHandle(), visble);
  ShowWindow(credProvClassComboBox->GetHandle(), visble);
  ShowWindow(credProvClassLabel->GetHandle(), visble);
  ShowWindow(cusCredFileEdit->GetHandle(), visble);
  ShowWindow(cusCredFileLabel->GetHandle(), visble);
}

void DsnConfigurationWindow::ShowAdvanceAuth(bool visble) const {
  ShowWindow(idpNameComboBox->GetHandle(), visble);
  ShowWindow(idpNameLabel->GetHandle(), visble);
  if (visble) {
    // Show fields in Advance Authentication based on selection of idpName
    OnIdpNameChanged();
  } else {
    ShowWindow(idpHostEdit->GetHandle(), visble);
    ShowWindow(idpHostLabel->GetHandle(), visble);
    ShowWindow(idpUserNameEdit->GetHandle(), visble);
    ShowWindow(idpUserNameLabel->GetHandle(), visble);
    ShowWindow(idpPasswordEdit->GetHandle(), visble);
    ShowWindow(idpPasswordLabel->GetHandle(), visble);
    ShowWindow(idpArnEdit->GetHandle(), visble);
    ShowWindow(idpArnLabel->GetHandle(), visble);
    ShowWindow(oktaAppIdEdit->GetHandle(), visble);
    ShowWindow(oktaAppIdLabel->GetHandle(), visble);
    ShowWindow(roleArnEdit->GetHandle(), visble);
    ShowWindow(roleArnLabel->GetHandle(), visble);
    ShowWindow(aadAppIdEdit->GetHandle(), visble);
    ShowWindow(aadAppIdLabel->GetHandle(), visble);
    ShowWindow(aadClientSecretEdit->GetHandle(), visble);
    ShowWindow(aadClientSecretLabel->GetHandle(), visble);
    ShowWindow(aadTenantEdit->GetHandle(), visble);
    ShowWindow(aadTenantLabel->GetHandle(), visble);
  }
}

void DsnConfigurationWindow::ShowConnectionSettings(bool visble) const {
  ShowWindow(reqTimeoutEdit->GetHandle(), visble);
  ShowWindow(reqTimeoutLabel->GetHandle(), visble);
  ShowWindow(socketTimeoutEdit->GetHandle(), visble);
  ShowWindow(socketTimeoutLabel->GetHandle(), visble);
  ShowWindow(maxRetryCountClientEdit->GetHandle(), visble);
  ShowWindow(maxRetryCountClientLabel->GetHandle(), visble);
  ShowWindow(maxConnectionsEdit->GetHandle(), visble);
  ShowWindow(maxConnectionsLabel->GetHandle(), visble);
}

void DsnConfigurationWindow::ShowLogSettings(bool visble) const {
  ShowWindow(logLevelComboBox->GetHandle(), visble);
  ShowWindow(logLevelLabel->GetHandle(), visble);
  ShowWindow(logPathEdit->GetHandle(), visble);
  ShowWindow(logPathLabel->GetHandle(), visble);
}

void DsnConfigurationWindow::OnSelChanged(TabIndex::Type idx) {
  if (preSel == idx) {
    LOG_DEBUG_MSG(
        "Tab index unchanged but OnSelChanged is triggered. Tab window will "
        "not change");
    return;
  }

  // hide previous tab selection
  switch (preSel) {
    case TabIndex::Type::BASIC_AUTH:
      ShowBasicAuth(false);

      break;
    case TabIndex::Type::ADVANCE_AUTH:
      ShowAdvanceAuth(false);

      break;
    case TabIndex::Type::CONNECTION_SETTINGS:
      ShowConnectionSettings(false);

      break;
    case TabIndex::Type::LOG_SETTINGS:
      ShowLogSettings(false);

      break;
    default:
      // log wrong preSel value
      LOG_DEBUG_MSG("wrong preSel value: "
                    << preSel << ", this should not happen. Suggest to debug.");
  }

  // show current tab selection
  switch (idx) {
    case TabIndex::Type::BASIC_AUTH:
      ShowBasicAuth(true);

      break;
    case TabIndex::Type::ADVANCE_AUTH:
      ShowAdvanceAuth(true);

      break;
    case TabIndex::Type::CONNECTION_SETTINGS:
      ShowConnectionSettings(true);

      break;
    case TabIndex::Type::LOG_SETTINGS:
      ShowLogSettings(true);

      break;
    default:
      // log wrong index value
      LOG_DEBUG_MSG("wrong idx value: "
                    << idx << ", this should not happen. Suggest to debug.");
  }

  // Hide all balloons when tabs switch
  Edit_HideBalloonTip(nameEdit->GetHandle());
  shownNameBalloon = false;
  Edit_HideBalloonTip(regionEdit->GetHandle());
  shownRegBalloon = false;
  Edit_HideBalloonTip(cusCredFileEdit->GetHandle());
  shownCusCredFileBalloon = false;
  Edit_HideBalloonTip(maxConnectionsEdit->GetHandle());
  shownMaxConBalloon = false;

  // save the previously selected index
  preSel = idx;
}

void DsnConfigurationWindow::OnCredProvClassChanged() const {
  // get value of credProvClass
  std::wstring credProvClassWStr;
  credProvClassComboBox->GetText(credProvClassWStr);
  std::string credProvClassStr = utility::ToUtf8(credProvClassWStr);
  CredProvClass::Type credProvClass =
      CredProvClass::FromString(credProvClassStr, CredProvClass::Type::UNKNOWN);
  bool credProvClassNotSelected =
      credProvClass != CredProvClass::Type::PROP_FILE_CRED_PROV
      && credProvClass != CredProvClass::Type::INST_PROF_CRED_PROV;

  cusCredFileEdit->SetEnabled(credProvClass
                              == CredProvClass::Type::PROP_FILE_CRED_PROV);

  // when PropertiesFileCredentialsProvider or
  // Instanceprofilecredentialsprovider are selected as credentials provider
  // class, the access key ID, secret access key and session token fields will
  // be disabled
  accessKeyIdEdit->SetEnabled(credProvClassNotSelected);
  secretAccessKeyEdit->SetEnabled(credProvClassNotSelected);
  sessionTokenEdit->SetEnabled(credProvClassNotSelected);
}

void DsnConfigurationWindow::OnIdpNameChanged() const {
  // get value of idpName
  std::wstring idpNameWStr;
  idpNameComboBox->GetText(idpNameWStr);
  std::string idpNameStr = utility::ToUtf8(idpNameWStr);
  IdpName::Type idpName =
      IdpName::FromString(idpNameStr, IdpName::Type::UNKNOWN);
  // If idpName is not none/unknown, idpNameNEqNone is true.
  bool idpNameNEqNone =
      idpName != IdpName::Type::UNKNOWN && idpName != IdpName::Type::NONE;
  bool ipdNameOkta = idpName == IdpName::Type::OKTA;
  bool ipdNameAAD = idpName == IdpName::Type::AAD;

  // when Okta or AAD are selected for IdpName, the access key ID, secret
  // access key and session token fields will be disabled
  accessKeyIdEdit->SetEnabled(!idpNameNEqNone);
  secretAccessKeyEdit->SetEnabled(!idpNameNEqNone);
  sessionTokenEdit->SetEnabled(!idpNameNEqNone);

  // enable/disable generic advance authenication fields
  idpHostEdit->SetEnabled(idpNameNEqNone);
  idpUserNameEdit->SetEnabled(idpNameNEqNone);
  idpPasswordEdit->SetEnabled(idpNameNEqNone);
  idpArnEdit->SetEnabled(idpNameNEqNone);

  // hide/show generic advance authenication fields
  ShowWindow(idpHostLabel->GetHandle(), idpNameNEqNone);
  ShowWindow(idpHostEdit->GetHandle(), idpNameNEqNone);
  ShowWindow(idpUserNameEdit->GetHandle(), idpNameNEqNone);
  ShowWindow(idpUserNameLabel->GetHandle(), idpNameNEqNone);
  ShowWindow(idpPasswordEdit->GetHandle(), idpNameNEqNone);
  ShowWindow(idpPasswordLabel->GetHandle(), idpNameNEqNone);
  ShowWindow(idpArnEdit->GetHandle(), idpNameNEqNone);
  ShowWindow(idpArnLabel->GetHandle(), idpNameNEqNone);

  // enable/disable Okta-related fields
  oktaAppIdEdit->SetEnabled(ipdNameOkta);
  roleArnEdit->SetEnabled(ipdNameOkta);

  // hide/show Okta-related fields
  ShowWindow(oktaAppIdEdit->GetHandle(), ipdNameOkta);
  ShowWindow(oktaAppIdLabel->GetHandle(), ipdNameOkta);
  ShowWindow(roleArnEdit->GetHandle(), ipdNameOkta);
  ShowWindow(roleArnLabel->GetHandle(), ipdNameOkta);

  // enable/disable AAD-related fields
  aadAppIdEdit->SetEnabled(ipdNameAAD);
  aadClientSecretEdit->SetEnabled(ipdNameAAD);
  aadTenantEdit->SetEnabled(ipdNameAAD);

  // hide/show AAD-related fields
  ShowWindow(aadAppIdEdit->GetHandle(), ipdNameAAD);
  ShowWindow(aadAppIdLabel->GetHandle(), ipdNameAAD);
  ShowWindow(aadClientSecretEdit->GetHandle(), ipdNameAAD);
  ShowWindow(aadClientSecretLabel->GetHandle(), ipdNameAAD);
  ShowWindow(aadTenantEdit->GetHandle(), ipdNameAAD);
  ShowWindow(aadTenantLabel->GetHandle(), ipdNameAAD);
}

int DsnConfigurationWindow::CreateBasicSettingsGroup(int posX, int posY,
                                                     int sizeX) {
  enum { LABEL_WIDTH = 120 };

  int labelPosX = posX + INTERVAL;

  int tabSizeX = sizeX - 2 * INTERVAL;
  int editSizeX = sizeX - LABEL_WIDTH - 3 * INTERVAL;
  int editPosX = labelPosX + LABEL_WIDTH + INTERVAL;

  int rowPos = posY + INTERVAL;

  int checkBoxSize = sizeX - 2 * MARGIN;

  std::wstring wVal = utility::FromUtf8(config.GetDsn());
  nameLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                          L"Data Source Name*:", ChildId::NAME_LABEL);
  nameEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                        ChildId::NAME_EDIT);
  nameBalloon = CreateBalloon(L"Required Field",
                              L"DSN name is a required field.", TTI_ERROR);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetEndpoint());
  endpointLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                              L"Endpoint:", ChildId::ENDPOINT_LABEL);
  endpointEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                            ChildId::ENDPOINT_EDIT);
  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetRegion());
  regionLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                            L"Region:", ChildId::REGION_LABEL);
  regionEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                          ChildId::REGION_EDIT);
  regionBalloon = CreateBalloon(
      L"Required Field",
      L"Must enter the region if custom endpoint is provided.", TTI_WARNING);

  rowPos += INTERVAL + ROW_HEIGHT;

  enableMetadataPrepStmtCheckbox =
      CreateCheckBox(labelPosX, rowPos, checkBoxSize, ROW_HEIGHT,
                     L"Enable Metadata Prepared Statement",
                     ChildId::ENABLE_METADATA_PREPARED_STATEMENT_CHECKBOX,
                     config.IsEnableMetadataPreparedStatement());

  rowPos += INTERVAL + ROW_HEIGHT;

  tabs = CreateTab(labelPosX, rowPos, tabSizeX, ROW_HEIGHT, L"Tabs",
                   ChildId::TABS);

  tabs->AddTab(TabIndex::Type::BASIC_AUTH, L"Basic Authenication");
  tabs->AddTab(TabIndex::Type::ADVANCE_AUTH, L"Advance Authenication");
  tabs->AddTab(TabIndex::Type::CONNECTION_SETTINGS, L"Connection Options");
  tabs->AddTab(TabIndex::Type::LOG_SETTINGS, L"Log Settings");

  rowPos += INTERVAL + ROW_HEIGHT;

  return rowPos - posY;
}

int DsnConfigurationWindow::CreateBasicAuthSettingsGroup(int posX, int posY,
                                                         int sizeX) {
  enum { LABEL_WIDTH = 160 };

  int labelPosX = posX + INTERVAL;

  int editSizeX = sizeX - LABEL_WIDTH - 3 * INTERVAL;
  int editPosX = labelPosX + LABEL_WIDTH + INTERVAL;

  int rowPos = posY;

  int checkBoxSize = sizeX - 2 * MARGIN;

  std::wstring wVal = utility::FromUtf8(config.GetAccessKeyId());
  accessKeyIdLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT, L"Access Key ID:",
                  ChildId::ACCESS_KEY_ID_LABEL);
  accessKeyIdEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                               ChildId::ACCESS_KEY_ID_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetSecretKey());
  secretAccessKeyLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Secret Access Key:", ChildId::SECRET_ACCESS_KEY_LABEL);
  secretAccessKeyEdit =
      CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                 ChildId::SECRET_ACCESS_KEY_EDIT, ES_PASSWORD);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetSessionToken());
  sessionTokenLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT, L"Session Token:",
                  ChildId::SESSION_TOKEN_LABEL);
  sessionTokenEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                                ChildId::SESSION_TOKEN_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  credProvClassLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                                   L"AWS Credentials Provider Class:",
                                   ChildId::CRED_PROV_CLASS_LABEL);
  credProvClassComboBox =
      CreateComboBox(editPosX, rowPos, editSizeX, ROW_HEIGHT, L"",
                     ChildId::CRED_PROV_CLASS_COMBO_BOX);

  // the order of add string needs to match the definition of the
  // cred_prov_class.h file
  credProvClassComboBox->AddString(L"None");
  credProvClassComboBox->AddString(L"PropertiesFileCredentialsProvider");
  credProvClassComboBox->AddString(L"InstanceProfileCredentialsProvider");

  CredProvClass::Type className = config.GetCredProvClass();
  credProvClassComboBox->SetCBSelection(
      static_cast< int >(className));  // set default

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetCusCredFile());
  cusCredFileLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Custom Credentials File:", ChildId::CUS_CRED_FILE_LABEL);
  cusCredFileEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                               ChildId::CUS_CRED_FILE_EDIT);
  cusCredFileBalloon =
      CreateBalloon(L"Required Field",
                    L"Must enter path to custom credentials file if "
                    L"PropertiesFileCredentialsProvider is selected as "
                    L"credentials provider class.",
                    TTI_WARNING);

  rowPos += INTERVAL + ROW_HEIGHT;

  OnCredProvClassChanged();

  return rowPos - posY;
}

int DsnConfigurationWindow::CreateAdvanceAuthSettingsGroup(int posX, int posY,
                                                           int sizeX) {
  enum { LABEL_WIDTH = 120 };

  int labelPosX = posX + INTERVAL;

  int editSizeX = sizeX - LABEL_WIDTH - 3 * INTERVAL;
  int editPosX = labelPosX + LABEL_WIDTH + INTERVAL;

  int rowPos = posY;

  IdpName::Type idpName = config.GetIdpName();
  idpNameLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                             L"Idp Name:", ChildId::IDP_NAME_LABEL);
  idpNameComboBox = CreateComboBox(editPosX, rowPos, editSizeX, ROW_HEIGHT, L"",
                                   ChildId::IDP_NAME_COMBO_BOX);

  // the order of add string needs to match the definition of the idp name .h
  // file
  idpNameComboBox->AddString(L"None");
  idpNameComboBox->AddString(L"Okta");
  idpNameComboBox->AddString(L"AzureAD");

  idpNameComboBox->SetCBSelection(static_cast< int >(idpName));  // set default

  rowPos += INTERVAL + ROW_HEIGHT;

  std::wstring wVal = utility::FromUtf8(config.GetIdpHost());
  idpHostLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                             L"Idp Host:", ChildId::IDP_HOST_LABEL);
  idpHostEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                           ChildId::IDP_HOST_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetIdpUserName());
  idpUserNameLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT, L"Idp User Name:",
                  ChildId::IDP_USER_NAME_LABEL);
  idpUserNameEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                               ChildId::IDP_USER_NAME_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetIdpPassword());
  idpPasswordLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                                 L"Idp Password:", ChildId::IDP_PASSWORD_LABEL);
  idpPasswordEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                               ChildId::IDP_PASSWORD_EDIT, ES_PASSWORD);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetIdpArn());
  idpArnLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                            L"Idp ARN:", ChildId::IDP_ARN_LABEL);
  idpArnEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                          ChildId::IDP_ARN_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  int arnRowPos = rowPos;

  // Okta Only fields
  wVal = utility::FromUtf8(config.GetOktaAppId());
  oktaAppIdLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Okta Application ID:", ChildId::OKTA_APP_ID_LABEL);
  oktaAppIdEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                             ChildId::OKTA_APP_ID_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetRoleArn());
  roleArnLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                             L"Role ARN:", ChildId::ROLE_ARN_LABEL);
  roleArnEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                           ChildId::ROLE_ARN_EDIT);

  // retrieve y position value from Idp ARN field
  // so that AAD specific fields can be put right after Idp ARN field.
  rowPos = arnRowPos;

  // AAD specific fields
  wVal = utility::FromUtf8(config.GetAadAppId());
  aadAppIdLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"AAD Application ID:", ChildId::AAD_APP_ID_LABEL);
  aadAppIdEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                            ChildId::AAD_APP_ID_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetAadClientSecret());
  aadClientSecretLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"AAD Client Secret:", ChildId::AAD_CLIENT_SECRET_LABEL);
  aadClientSecretEdit =
      CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                 ChildId::AAD_CLIENT_SECRET_EDIT, ES_PASSWORD);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = utility::FromUtf8(config.GetAadTenant());
  aadTenantLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                               L"AAD Tenant:", ChildId::AAD_TENANT_LABEL);
  aadTenantEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                             ChildId::AAD_TENANT_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  OnIdpNameChanged();

  return rowPos - posY;
}

int DsnConfigurationWindow::CreateConnectionSettingsGroup(int posX, int posY,
                                                          int sizeX) {
  enum { LABEL_WIDTH = 120 };

  int labelPosX = posX + INTERVAL;

  int editSizeX = sizeX - LABEL_WIDTH - 3 * INTERVAL;
  int editPosX = labelPosX + LABEL_WIDTH + INTERVAL;

  int rowPos = posY;

  std::wstring wVal = std::to_wstring(config.GetReqTimeout());
  reqTimeoutLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Request Timeout:", ChildId::REQ_TIMEOUT_LABEL);
  reqTimeoutEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                              ChildId::REQ_TIMEOUT_EDIT, ES_NUMBER);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = std::to_wstring(config.GetSocketTimeout());
  socketTimeoutLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Socket Timeout:", ChildId::SOCKET_TIMEOUT_LABEL);
  socketTimeoutEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                                 ChildId::SOCKET_TIMEOUT_EDIT, ES_NUMBER);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = std::to_wstring(config.GetMaxRetryCount());
  maxRetryCountClientLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH,
                                         ROW_HEIGHT, L"Max retry count client:",
                                         ChildId::MAX_RETRY_COUNT_CLIENT_LABEL);
  maxRetryCountClientEdit =
      CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                 ChildId::MAX_RETRY_COUNT_CLIENT_EDIT, ES_NUMBER);

  rowPos += INTERVAL + ROW_HEIGHT;

  wVal = std::to_wstring(config.GetMaxConnections());
  maxConnectionsLabel =
      CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                  L"Max connections:", ChildId::MAX_CONNECTIONS_LABEL);
  maxConnectionsEdit = CreateEdit(editPosX, rowPos, editSizeX, ROW_HEIGHT, wVal,
                                  ChildId::MAX_CONNECTIONS_EDIT, ES_NUMBER);
  maxConnectionsBalloon = CreateBalloon(
      L"Positive Number Only",
      L"Number of connections must be a positive number.", TTI_ERROR);

  rowPos += INTERVAL + ROW_HEIGHT;

  return rowPos - posY;
}

int DsnConfigurationWindow::CreateLogSettingsGroup(int posX, int posY,
                                                   int sizeX) {
  enum { LABEL_WIDTH = 120 };

  int labelPosX = posX + INTERVAL;
  int pathSizeX = sizeX - 2 * INTERVAL;
  int comboSizeX = sizeX - LABEL_WIDTH - 3 * INTERVAL;
  int comboPosX = labelPosX + LABEL_WIDTH + INTERVAL;
  int editPosX = labelPosX;

  int rowPos = posY;

  LogLevel::Type logLevel = config.GetLogLevel();

  logLevelLabel = CreateLabel(labelPosX, rowPos, LABEL_WIDTH, ROW_HEIGHT,
                              L"Log Level:", ChildId::LOG_LEVEL_LABEL);
  logLevelComboBox = CreateComboBox(comboPosX, rowPos, comboSizeX, ROW_HEIGHT,
                                    L"", ChildId::LOG_LEVEL_COMBO_BOX);

  logLevelComboBox->AddString(L"Debug");
  logLevelComboBox->AddString(L"Info");
  logLevelComboBox->AddString(L"Error");
  logLevelComboBox->AddString(L"Off");

  logLevelComboBox->SetCBSelection(
      static_cast< int >(logLevel));  // set default

  rowPos += INTERVAL + ROW_HEIGHT;

  std::wstring wVal = utility::FromUtf8(config.GetLogPath());
  logPathLabel = CreateLabel(
      labelPosX, rowPos, pathSizeX, ROW_HEIGHT * 2,
      L"Log Path:\n(the log file name format is docdb_odbc_YYYYMMDD.log)",
      ChildId::LOG_PATH_LABEL);

  rowPos += INTERVAL * 2 + ROW_HEIGHT;

  logPathEdit = CreateEdit(editPosX, rowPos, pathSizeX, ROW_HEIGHT, wVal,
                           ChildId::LOG_PATH_EDIT);

  rowPos += INTERVAL + ROW_HEIGHT;

  std::wstring logLevelWStr;
  logLevelComboBox->GetText(logLevelWStr);
  if (LogLevel::FromString(utility::ToUtf8(logLevelWStr),
                           LogLevel::Type::UNKNOWN)
      == LogLevel::Type::OFF) {
    logPathEdit->SetEnabled(false);
  } else {
    logPathEdit->SetEnabled(true);
  }

  return rowPos - posY;
}

bool DsnConfigurationWindow::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_COMMAND: {
      switch (LOWORD(wParam)) {
        case ChildId::OK_BUTTON: {
          try {
            RetrieveParameters(config);

            accepted = true;

            PostMessage(GetHandle(), WM_CLOSE, 0, 0);
          } catch (IgniteError& err) {
            std::wstring errWText = utility::FromUtf8(err.GetText());
            MessageBox(NULL, errWText.c_str(), L"Error!",
                       MB_ICONEXCLAMATION | MB_OK);
          }

          break;
        }

        case IDCANCEL:
        case ChildId::CANCEL_BUTTON: {
          PostMessage(GetHandle(), WM_CLOSE, 0, 0);

          break;
        }

        case ChildId::NAME_EDIT: {
          // Check if window has been created.
          if (created) {
            okButton->SetEnabled(nameEdit->HasText());

            if (!shownNameBalloon && !nameEdit->HasText()) {
              Edit_ShowBalloonTip(nameEdit->GetHandle(), nameBalloon.get());
              shownNameBalloon = true;
            } else {
              Edit_HideBalloonTip(nameEdit->GetHandle());
              shownNameBalloon = false;
            }
          }
          break;
        }

        case ChildId::ENABLE_METADATA_PREPARED_STATEMENT_CHECKBOX: {
          enableMetadataPrepStmtCheckbox->SetChecked(
              !enableMetadataPrepStmtCheckbox->IsChecked());
          break;
        }

        case ChildId::REGION_EDIT: {
          if (created) {
            // The balloon will be triggered one time when user is on the Region
            // edit field

            if (!shownRegBalloon && !regionEdit->HasText()
                && endpointEdit->HasText()) {
              Edit_ShowBalloonTip(regionEdit->GetHandle(), regionBalloon.get());
              shownRegBalloon = true;
            } else {
              Edit_HideBalloonTip(regionEdit->GetHandle());
              shownRegBalloon = false;
            }
          }
          break;
        }

        case ChildId::CRED_PROV_CLASS_COMBO_BOX: {
          OnCredProvClassChanged();

          break;
        }

        case ChildId::CUS_CRED_FILE_EDIT: {
          if (created) {
            std::wstring credProvClassWStr;
            credProvClassComboBox->GetText(credProvClassWStr);
            std::string credProvClassStr = utility::ToUtf8(credProvClassWStr);
            CredProvClass::Type className = CredProvClass::FromString(
                credProvClassStr, CredProvClass::Type::UNKNOWN);

            // if PropertiesFileCredentialsProvider is selected but custom
            // credentials file field is empty, throw an error
            if (!shownCusCredFileBalloon && !cusCredFileEdit->HasText()
                && className == CredProvClass::Type::PROP_FILE_CRED_PROV) {
              Edit_ShowBalloonTip(cusCredFileEdit->GetHandle(),
                                  cusCredFileBalloon.get());
              shownCusCredFileBalloon = true;
            } else {
              Edit_HideBalloonTip(cusCredFileEdit->GetHandle());
              shownCusCredFileBalloon = false;
            }
          }
          break;
        }

        case ChildId::MAX_CONNECTIONS_EDIT: {
          if (created) {
            std::wstring maxConWStr;
            maxConnectionsEdit->GetText(maxConWStr);

            std::string maxConStr = utility::ToUtf8(maxConWStr);

            int16_t maxCon = common::LexicalCast< int16_t >(maxConStr);

            if (!shownMaxConBalloon && maxCon <= 0) {
              Edit_ShowBalloonTip(maxConnectionsEdit->GetHandle(),
                                  maxConnectionsBalloon.get());
              shownMaxConBalloon = true;
            } else {
              Edit_HideBalloonTip(maxConnectionsEdit->GetHandle());
              shownMaxConBalloon = false;
            }
          }
          break;
        }

        case ChildId::IDP_NAME_COMBO_BOX: {
          OnIdpNameChanged();

          break;
        }

        case ChildId::LOG_LEVEL_COMBO_BOX: {
          std::wstring logLevelWStr;
          logLevelComboBox->GetText(logLevelWStr);
          if (LogLevel::FromString(utility::ToUtf8(logLevelWStr),
                                   LogLevel::Type::UNKNOWN)
              == LogLevel::Type::OFF) {
            logPathEdit->SetEnabled(false);
          } else {
            logPathEdit->SetEnabled(true);
          }
          break;
        }

        default:
          return false;
      }

      break;
    }

    case WM_DESTROY: {
      PostQuitMessage(accepted ? Result::OK : Result::CANCEL);

      break;
    }

    case WM_NOTIFY: {
      switch (LOWORD(wParam)) {
        case ChildId::TABS: {
          LOG_DEBUG_MSG("current Tab selection index (without cast): "
                        << tabs->GetTabSelection());

          TabIndex::Type curSel =
              static_cast< TabIndex::Type >(tabs->GetTabSelection());

          LOG_DEBUG_MSG("current Tab selection index (with cast): " << curSel);

          OnSelChanged(curSel);
          break;
        }

        default:
          return false;
      }

      break;
    }

    default:
      return false;
  }

  return true;
}

void DsnConfigurationWindow::RetrieveParameters(
    config::Configuration& cfg) const {
  // Intentioanlly retrieve log parameters first
  RetrieveLogParameters(cfg);
  RetrieveBasicParameters(cfg);
  RetrieveBasicAuthParameters(cfg);
  RetrieveAdvanceAuthParameters(cfg);
  RetrieveConnectionParameters(cfg);
}

void DsnConfigurationWindow::RetrieveBasicParameters(
    config::Configuration& cfg) const {
  bool enableMetadataPrepStmt = enableMetadataPrepStmtCheckbox->IsChecked();

  std::wstring dsnWStr;
  std::wstring endpointWStr;
  std::wstring regionWStr;

  nameEdit->GetText(dsnWStr);
  endpointEdit->GetText(endpointWStr);
  regionEdit->GetText(regionWStr);

  std::string dsnStr = utility::ToUtf8(dsnWStr);
  common::StripSurroundingWhitespaces(dsnStr);
  std::string endpointStr = utility::ToUtf8(endpointWStr);
  std::string regionStr = utility::ToUtf8(regionWStr);

  cfg.SetDsn(dsnStr);
  cfg.SetEnableMetadataPreparedStatement(enableMetadataPrepStmt);
  cfg.SetEndpoint(endpointStr);
  cfg.SetRegion(regionStr);

  LOG_INFO_MSG("Retrieving arguments:");
  LOG_INFO_MSG("DSN:                             " << dsnStr);
  LOG_INFO_MSG("Enable Metadata Preprepard Statement: "
               << (enableMetadataPrepStmt ? "true" : "false"));
  LOG_INFO_MSG("Endpoint:    " << endpointStr);
  LOG_INFO_MSG("Region:      " << regionStr);
}

void DsnConfigurationWindow::RetrieveBasicAuthParameters(
    config::Configuration& cfg) const {
  if (!regionEdit->HasText() && endpointEdit->HasText())
    throw IgniteError(
        IgniteError::IGNITE_ERR_GENERIC,
        "[Region] Must enter the region if custom endpoint is provided.");

  // retrieve credentials provider class value
  std::wstring credProvClassWStr;
  credProvClassComboBox->GetText(credProvClassWStr);
  std::string credProvClassStr = utility::ToUtf8(credProvClassWStr);
  CredProvClass::Type className =
      CredProvClass::FromString(credProvClassStr, CredProvClass::Type::UNKNOWN);

  // if PropertiesFileCredentialsProvider is selected but custom credentials
  // file field is empty, throw an error
  if (!cusCredFileEdit->HasText()
      && className == CredProvClass::Type::PROP_FILE_CRED_PROV)
    throw IgniteError(IgniteError::IGNITE_ERR_GENERIC,
                      "[Custom Credentials File] Must enter path to custom "
                      "credentials file if "
                      "PropertiesFileCredentialsProvider is selected as "
                      "credentials provider class.");

  std::wstring accessKeyIdWStr;
  std::wstring secretKeyWStr;
  std::wstring sessionTokenWStr;
  std::wstring cusCredFileWStr;

  accessKeyIdEdit->GetText(accessKeyIdWStr);
  secretAccessKeyEdit->GetText(secretKeyWStr);
  sessionTokenEdit->GetText(sessionTokenWStr);
  cusCredFileEdit->GetText(cusCredFileWStr);

  std::string accessKeyIdStr = utility::ToUtf8(accessKeyIdWStr);
  std::string secretKeyStr = utility::ToUtf8(secretKeyWStr);
  std::string sessionTokenStr = utility::ToUtf8(sessionTokenWStr);
  std::string cusCredFileStr = utility::ToUtf8(cusCredFileWStr);

  cfg.SetAccessKeyId(accessKeyIdStr);
  cfg.SetSecretKey(secretKeyStr);
  cfg.SetSessionToken(sessionTokenStr);
  cfg.SetCredProvClass(className);
  cfg.SetCusCredFile(cusCredFileStr);

  LOG_INFO_MSG("Retrieving arguments:");
  LOG_INFO_MSG("Session Token:                   " << sessionTokenStr);
  LOG_INFO_MSG("AWS Credentials Provider Class: " << credProvClassStr);
  LOG_DEBUG_MSG(
      "CredProvClass::Type className: " << static_cast< int >(className));
  LOG_INFO_MSG("Custum Credentials File: " << cusCredFileStr);
  // username and password intentionally not logged for security reasons
}

void DsnConfigurationWindow::RetrieveAdvanceAuthParameters(
    config::Configuration& cfg) const {
  std::wstring idpNameWStr;
  std::wstring idpHostWStr;
  std::wstring idpUserNameWStr;
  std::wstring idpPasswordWStr;
  std::wstring idpArnWStr;
  std::wstring oktaAppIdWStr;
  std::wstring roleArnWStr;
  std::wstring aadAppIdWStr;
  std::wstring aadClientSecretWStr;
  std::wstring aadTenantWStr;

  idpNameComboBox->GetText(idpNameWStr);
  idpHostEdit->GetText(idpHostWStr);
  idpUserNameEdit->GetText(idpUserNameWStr);
  idpPasswordEdit->GetText(idpPasswordWStr);
  idpArnEdit->GetText(idpArnWStr);
  oktaAppIdEdit->GetText(oktaAppIdWStr);
  roleArnEdit->GetText(roleArnWStr);
  aadAppIdEdit->GetText(aadAppIdWStr);
  aadClientSecretEdit->GetText(aadClientSecretWStr);
  aadTenantEdit->GetText(aadTenantWStr);

  std::string idpNameStr = utility::ToUtf8(idpNameWStr);
  std::string idpHostStr = utility::ToUtf8(idpHostWStr);
  std::string idpUserNameStr = utility::ToUtf8(idpUserNameWStr);
  std::string idpPasswordStr = utility::ToUtf8(idpPasswordWStr);
  std::string idpArnStr = utility::ToUtf8(idpArnWStr);
  std::string oktaAppIdStr = utility::ToUtf8(oktaAppIdWStr);
  std::string roleArnStr = utility::ToUtf8(roleArnWStr);
  std::string aadAppIdStr = utility::ToUtf8(aadAppIdWStr);
  std::string aadClientSecretStr = utility::ToUtf8(aadClientSecretWStr);
  std::string aadTenantStr = utility::ToUtf8(aadTenantWStr);

  IdpName::Type idpName =
      IdpName::FromString(idpNameStr, IdpName::Type::UNKNOWN);

  cfg.SetIdpName(idpName);
  cfg.SetIdpHost(idpHostStr);
  cfg.SetIdpUserName(idpUserNameStr);
  cfg.SetIdpPassword(idpPasswordStr);
  cfg.SetIdpArn(idpArnStr);
  cfg.SetOktaAppId(oktaAppIdStr);
  cfg.SetRoleArn(roleArnStr);
  cfg.SetAadAppId(aadAppIdStr);
  cfg.SetAadClientSecret(aadClientSecretStr);
  cfg.SetAadTenant(aadTenantStr);

  LOG_INFO_MSG("Idp Name:    " << idpNameStr);
  LOG_DEBUG_MSG("IdpName::Type idpName: " << static_cast< int >(idpName));
  LOG_INFO_MSG("Idp Host:     " << idpHostStr);
  LOG_INFO_MSG("Idp User Name:     " << idpUserNameStr);
  LOG_INFO_MSG("Idp ARN:     " << idpArnStr);
  LOG_INFO_MSG("Okta Application ID:     " << oktaAppIdStr);
  LOG_INFO_MSG("Role ARN:     " << roleArnStr);
  LOG_INFO_MSG("Azure AD Application Id:     " << aadAppIdStr);
  LOG_INFO_MSG("Azure AD Tenant:     " << aadTenantStr);
  // Idp password and AAD client secret not logged intentionally
}

void DsnConfigurationWindow::RetrieveConnectionParameters(
    config::Configuration& cfg) const {
  std::wstring reqTimeoutWStr;
  std::wstring socketTimeoutWStr;
  std::wstring maxRetryCountWStr;
  std::wstring maxConWStr;

  reqTimeoutEdit->GetText(reqTimeoutWStr);
  socketTimeoutEdit->GetText(socketTimeoutWStr);
  maxRetryCountClientEdit->GetText(maxRetryCountWStr);
  maxConnectionsEdit->GetText(maxConWStr);

  std::string reqTimeoutStr = utility::ToUtf8(reqTimeoutWStr);
  std::string socketTimeoutStr = utility::ToUtf8(socketTimeoutWStr);
  std::string maxRetryCountStr = utility::ToUtf8(maxRetryCountWStr);
  std::string maxConStr = utility::ToUtf8(maxConWStr);

  int32_t reqTimeout = common::LexicalCast< int32_t >(reqTimeoutStr);
  int32_t socketTimeout = common::LexicalCast< int32_t >(socketTimeoutStr);
  int32_t maxRetryCount = common::LexicalCast< int32_t >(maxRetryCountStr);
  int32_t maxCon = common::LexicalCast< int32_t >(maxConStr);

  if (maxCon <= 0)
    throw IgniteError(
        IgniteError::IGNITE_ERR_GENERIC,
        "[Max Connections] Number of connections must be a positive number.");

  cfg.SetReqTimeout(reqTimeout);
  cfg.SetSocketTimeout(socketTimeout);
  cfg.SetMaxRetryCount(maxRetryCount);
  cfg.SetMaxConnections(maxCon);

  LOG_INFO_MSG("Request timeout (ms): " << reqTimeout);
  LOG_INFO_MSG("Socket timeout (ms):  " << socketTimeout);
  LOG_INFO_MSG("Max retry count:      " << maxRetryCount);
  LOG_INFO_MSG("Max connections:      " << maxCon);
}

// RetrieveLogParameters is a special case. We want to get the log level and
// path as soon as possible. If user set log level to OFF, then nothing should
// be logged. Therefore, the LOG_MSG calls are after log level and log path are
// set.
void DsnConfigurationWindow::RetrieveLogParameters(
    config::Configuration& cfg) const {
  std::wstring logLevelWStr;
  std::wstring logPathWStr;

  logLevelComboBox->GetText(logLevelWStr);
  logPathEdit->GetText(logPathWStr);

  std::string logLevelStr = utility::ToUtf8(logLevelWStr);
  std::string logPathStr = utility::ToUtf8(logPathWStr);

  LogLevel::Type logLevel =
      LogLevel::FromString(logLevelStr, LogLevel::Type::UNKNOWN);

  cfg.SetLogLevel(logLevel);
  cfg.SetLogPath(logPathStr);

  LOG_INFO_MSG("Log level:    " << logLevelStr);
  LOG_DEBUG_MSG("LogLevel::Type logLevel: " << static_cast< int >(logLevel));
  LOG_INFO_MSG("Log path:     " << logPathStr);
}
}  // namespace ui
}  // namespace system
}  // namespace odbc
}  // namespace ignite
