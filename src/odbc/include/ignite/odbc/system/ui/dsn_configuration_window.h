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

#ifndef _IGNITE_ODBC_SYSTEM_UI_DSN_CONFIGURATION_WINDOW
#define _IGNITE_ODBC_SYSTEM_UI_DSN_CONFIGURATION_WINDOW

#include "ignite/odbc/config/configuration.h"
#include "ignite/odbc/system/ui/custom_window.h"

namespace ignite {
namespace odbc {
namespace system {
namespace ui {
/**
 * DSN configuration window class.
 */
class DsnConfigurationWindow : public CustomWindow {
  /**
   * Children windows ids.
   */
  struct ChildId {
    enum Type {
      BASIC_AUTH_SETTINGS_GROUP_BOX = 100,
      ADVANCE_AUTH_SETTINGS_GROUP_BOX,
      LOG_SETTINGS_GROUP_BOX,
      CRED_PROVIDER_GROUP_BOX,
      ENDPOINT_OPTIONS_GROUP_BOX,
      CONNECTION_SETTINGS_GROUP_BOX,
      NAME_EDIT,
      NAME_LABEL,
      IDP_NAME_LABEL,
      IDP_NAME_COMBO_BOX,
      IDP_HOST_EDIT,
      IDP_HOST_LABEL,
      IDP_USER_NAME_EDIT,
      IDP_USER_NAME_LABEL,
      IDP_PASSWORD_EDIT,
      IDP_PASSWORD_LABEL,
      IDP_ARN_EDIT,
      IDP_ARN_LABEL,
      OKTA_APP_ID_EDIT,
      OKTA_APP_ID_LABEL,
      ROLE_ARN_EDIT,
      ROLE_ARN_LABEL,
      AAD_APP_ID_EDIT,
      AAD_APP_ID_LABEL,
      AAD_CLIENT_SECRET_EDIT,
      AAD_CLIENT_SECRET_LABEL,
      AAD_TENANT_EDIT,
      AAD_TENANT_LABEL,
      LOG_LEVEL_COMBO_BOX,
      LOG_LEVEL_LABEL,
      LOG_PATH_EDIT,
      LOG_PATH_LABEL,
      REQ_TIMEOUT_EDIT,
      REQ_TIMEOUT_LABEL,
      SOCKET_TIMEOUT_EDIT,
      SOCKET_TIMEOUT_LABEL,
      MAX_RETRY_COUNT_CLIENT_EDIT,
      MAX_RETRY_COUNT_CLIENT_LABEL,
      MAX_CONNECTIONS_EDIT,
      MAX_CONNECTIONS_LABEL,
      ENDPOINT_EDIT,
      ENDPOINT_LABEL,
      REGION_EDIT,
      REGION_LABEL,
      CRED_PROV_CLASS_COMBO_BOX,
      CRED_PROV_CLASS_LABEL,
      CUS_CRED_FILE_EDIT,
      CUS_CRED_FILE_LABEL,
      DRIVER_LABEL,
      DRIVER_EDIT,
      ACCESS_KEY_ID_LABEL,
      ACCESS_KEY_ID_EDIT,
      SECRET_ACCESS_KEY_LABEL,
      SECRET_ACCESS_KEY_EDIT,
      SESSION_TOKEN_LABEL,
      SESSION_TOKEN_EDIT,
      ENABLE_METADATA_PREPARED_STATEMENT_CHECKBOX,
      OK_BUTTON,
      CANCEL_BUTTON
    };
  };

  // Window margin size.
  enum { MARGIN = 10 };

  // Standard interval between UI elements.
  enum { INTERVAL = 10 };

  // Standard row height.
  enum { ROW_HEIGHT = 20 };

  // Standard button width.
  enum { BUTTON_WIDTH = 80 };

  // Standard button height.
  enum { BUTTON_HEIGHT = 25 };

 public:
  /**
   * Constructor.
   *
   * @param parent Parent window handle.
   */
  explicit DsnConfigurationWindow(Window* parent,
                                  config::Configuration& config);

  /**
   * Destructor.
   */
  virtual ~DsnConfigurationWindow();

  /**
   * Create window in the center of the parent window.
   */
  void Create();

  /**
   * @copedoc ignite::odbc::system::ui::CustomWindow::OnCreate
   */
  virtual void OnCreate();

  /**
   * @copedoc ignite::odbc::system::ui::CustomWindow::OnMessage
   */
  virtual bool OnMessage(UINT msg, WPARAM wParam, LPARAM lParam);

 private:
  IGNITE_NO_COPY_ASSIGNMENT(DsnConfigurationWindow)

  /**
   * Disable / Enable the cusCredFileEdit field
   * based on value of credProvClass.
   * If credProvClass equals "Properties File Credentials Provider,"
   * cusCredFileEdit field is enabled, else it is diabled.
   */
  void DsnConfigurationWindow::EnableCusCredFileField() const;

  /**
   * Disable / Enable the fields in advance authentication options UI group 
   * based on value of IdpName.
   * IdpName field is always enabled.
   * When "None" is selected for IdpName, all fields are disabled.
   * When "Okta" is selected for IdpName, AzureAD-specific fields are disabled,
   * and the rest are enabled. 
   * When "AzureAD" is selected for IdpName, Okta-specific fields are disabled, 
   * and the rest are enabled.
   */
  void DsnConfigurationWindow::EnableAdvanceAuthFields() const;

  /**
   * Retrieves current values from the children and stores
   * them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the basic authentication UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveBasicAuthParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the advance authentication options UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveAdvanceAuthParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the log configuration UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveLogParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the AWS Credentials Provider Options UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveCredentialsProvidersParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the Connection UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveConnectionParameters(config::Configuration& cfg) const;

  /**
   * Retrieves current values from the Endpoint Configuration UI group and
   * stores them to the specified configuration.
   *
   * @param cfg Configuration.
   */
  void RetrieveEndpointConfigParameters(config::Configuration& cfg) const;

  /**
   * Create basic authentication settings group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateBasicAuthSettingsGroup(int posX, int posY, int sizeX);

  /**
   * Create advance authentication options group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateAdvanceAuthSettingsGroup(int posX, int posY, int sizeX);

  /**
   * Create logging configuration settings group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateLogSettingsGroup(int posX, int posY, int sizeX);

  /**
   * Create AWS Credentials Provider Options settings group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateCredentialsProvidersGroup(int posX, int posY, int sizeX);

  /**
   * Create Connection settings group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateConnctionSettingsGroup(int posX, int posY, int sizeX);

  /**
   * Create endpoint configuration options group box.
   *
   * @param posX X position.
   * @param posY Y position.
   * @param sizeX Width.
   * @return Size by Y.
   */
  int CreateEndpointConfigOptionsGroup(int posX, int posY, int sizeX);

  /** Window width. */
  int width;

  /** Window height. */
  int height;

  /** Connection settings group box. */
  std::unique_ptr< Window > basicAuthSettingsGroupBox;

  /** SSH settings group box. */
  std::unique_ptr< Window > advanceAuthSettingsGroupBox;

  /** Log settings group box. */
  std::unique_ptr< Window > logSettingsGroupBox;

  /** AWS credentials provider options group box. */
  std::unique_ptr< Window > credProviderSettingsGroupBox;

  /** Connection Options group box. */
  std::unique_ptr< Window > connctionSettingsGroupBox;

  /** Endpoint Configuration Options group box. */
  std::unique_ptr< Window > endpointConfigOptionsGroupBox;

  /** DSN name field label. */
  std::unique_ptr< Window > nameLabel;

  /** DSN name edit field. */
  std::unique_ptr< Window > nameEdit;

  /** DSN name balloon. */
  std::unique_ptr< EDITBALLOONTIP > nameBalloon;

  /** Request timeout field label. */
  std::unique_ptr< Window > reqTimeoutLabel;

  /** Request timeout edit. */
  std::unique_ptr< Window > reqTimeoutEdit;

  /** Socket timeout field label. */
  std::unique_ptr< Window > socketTimeoutLabel;

  /** Socket timeout edit. */
  std::unique_ptr< Window > socketTimeoutEdit;

  /** Max retry count client field label. */
  std::unique_ptr< Window > maxRetryCountClientLabel;

  /** Max retry count client edit. */
  std::unique_ptr< Window > maxRetryCountClientEdit;

  /** Max connections field label. */
  std::unique_ptr< Window > maxConnectionsLabel;

  /** Max connections edit. */
  std::unique_ptr< Window > maxConnectionsEdit;

  /** Max connections balloon. */
  std::unique_ptr< EDITBALLOONTIP > maxConnectionsBalloon;

  /** Endpoint label. */
  std::unique_ptr< Window > endpointLabel;

  /** Endpoint edit. */
  std::unique_ptr< Window > endpointEdit;

  /** Region label. */
  std::unique_ptr< Window > regionLabel;

  /** Region edit. */
  std::unique_ptr< Window > regionEdit;

  /** Region balloon. */
  std::unique_ptr< EDITBALLOONTIP > regionBalloon;

  /** Idp Name ComboBox **/
  std::unique_ptr< Window > idpNameComboBox;

  /** Idp Name label. */
  std::unique_ptr< Window > idpNameLabel;

  /** Idp Host edit. */
  std::unique_ptr< Window > idpHostEdit;

  /** Idp Host label. */
  std::unique_ptr< Window > idpHostLabel;

  /** Idp User Name edit. */
  std::unique_ptr< Window > idpUserNameEdit;

  /** Idp User Name label. */
  std::unique_ptr< Window > idpUserNameLabel;

  /** Idp Password edit. */
  std::unique_ptr< Window > idpPasswordEdit;

  /** Idp Password label. */
  std::unique_ptr< Window > idpPasswordLabel;

  /** Idp ARN edit. */
  std::unique_ptr< Window > idpArnEdit;

  /** Idp ARN label. */
  std::unique_ptr< Window > idpArnLabel;

  /** Okta Application Id edit. */
  std::unique_ptr< Window > oktaAppIdEdit;

  /** Okta Application Id label. */
  std::unique_ptr< Window > oktaAppIdLabel;

  /** Role ARN edit. */
  std::unique_ptr< Window > roleArnEdit;

  /** Role ARN label. */
  std::unique_ptr< Window > roleArnLabel;

  /** AAD Application Id edit. */
  std::unique_ptr< Window > aadAppIdEdit;

  /** AAD Application Id label. */
  std::unique_ptr< Window > aadAppIdLabel;

  /** AAD Client Secret edit. */
  std::unique_ptr< Window > aadClientSecretEdit;

  /** AAD Client Secret label. */
  std::unique_ptr< Window > aadClientSecretLabel;

  /** AAD Tenant edit. */
  std::unique_ptr< Window > aadTenantEdit;

  /** AAD Tenant label. */
  std::unique_ptr< Window > aadTenantLabel;

  /** Log Level ComboBox **/
  std::unique_ptr< Window > logLevelComboBox;

  /** Log Level label. */
  std::unique_ptr< Window > logLevelLabel;

  /** Log Path edit. */
  std::unique_ptr< Window > logPathEdit;

  /** Log Path label. */
  std::unique_ptr< Window > logPathLabel;

  /** Ok button. */
  std::unique_ptr< Window > okButton;

  /** Cancel button. */
  std::unique_ptr< Window > cancelButton;

  /** AWS credentials provider class label. */
  std::unique_ptr< Window > credProvClassLabel;

  /** AWS credentials provider class comboBox. */
  std::unique_ptr< Window > credProvClassComboBox;

  /** Custom credentials file label. */
  std::unique_ptr< Window > cusCredFileLabel;

  /** Custom credentials file edit. */
  std::unique_ptr< Window > cusCredFileEdit;

  /** Access key Id label. */
  std::unique_ptr< Window > accessKeyIdLabel;

  /** Access key Id edit. */
  std::unique_ptr< Window > accessKeyIdEdit;

  /** Secret Access Key label. */
  std::unique_ptr< Window > secretAccessKeyLabel;

  /** Secret Access Key edit. */
  std::unique_ptr< Window > secretAccessKeyEdit;

  /** Session Token label. */
  std::unique_ptr< Window > sessionTokenLabel;

  /** Session Token edit. */
  std::unique_ptr< Window > sessionTokenEdit;

  /** Enable Metadata Prepared Statement checkbox. */
  std::unique_ptr< Window > enableMetadataPrepStmtCheckbox;

  /** Configuration. */
  config::Configuration& config;

  /** Flag indicating whether OK option was selected. */
  bool accepted;

  /** Flag indicating whether the configuration window has been created. */
  bool created;

  /** Flag indicating whether the DSN name balloon has been shown. */
  bool shownNameBalloon;

  /** Flag indicating whether the max connections balloon has been shown. */
  bool shownMaxConBalloon;

  /** Flag indicating whether the region balloon has been shown. */
  bool shownRegBalloon;
};
}  // namespace ui
}  // namespace system
}  // namespace odbc
}  // namespace ignite

#endif  //_IGNITE_ODBC_SYSTEM_UI_DSN_CONFIGURATION_WINDOW
