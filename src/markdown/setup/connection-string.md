# Connection String Syntax and Options
`DRIVER={Amazon Timestream};<option>=<value>;`

### Driver
`Driver:` Required: the driver for this ODBC driver.

### Driver Configuration Option

| Option | Description | Default |
|--------|-------------|---------------|
| `ENABLE_METADATA_PREPARED_STATEMENT` | Enables Timestream ODBC driver to return metadata for PreparedStatements, but this will incur an additional cost with Timestream when retrieving the metadata. | `false` |

### Connection Options

| Option | Description | Default |
|--------|-------------|---------------|
| `REQUEST_TIMEOUT` | The time in milliseconds the AWS SDK will wait for a query request before timing out. Non-positive value disables request timeout. | `0`
| `SOCKET_TIMEOUT` | The time in milliseconds the AWS SDK will wait for data to be transferred over an open connection before timing out. Value must be non-negative. A value of 0 disables socket timeout.| `50000`
| `MAX_RETRY_COUNT` | The maximum number of retry attempts for retryable errors with 5XX error codes in the SDK. The value must be non-negative.| `0`
| `MAX_CONNECTIONS` | The maximum number of allowed concurrently opened HTTP connections to the Timestream service. The value must be positive.| `50`

### Endpoint Configuration Options

| Option | Description | Default |
|--------|-------------|---------------|
| `ENDPOINT` | The endpoint for the Timestream service. | `NONE`
| `REGION` | The signing region for the Timestream service endpoint. The signing region can be specified without an endpoint, but must be specified if a custom endpoint is given.| `NONE` |

### Basic Authentication Options

| Option | Description | Default |
|--------|-------------|---------------|
| `ACCESS_KEY_ID` | The AWS user access key id. | `NONE` |
| `SECRET_KEY` | The AWS user secret access key. | `NONE` |
| `SESSION_TOKEN` | The temporary session token required to access a database with multi-factor authentication (MFA) enabled. | `NONE` |

### Credentials Provider Options

| Option | Description | Default |
|--------|-------------|---------------|
| `AWS_CREDENTIALS_PROVIDER_CLASS` | One of `PropertiesFileCredentialsProvider` or `InstanceProfileCredentialsProvider` to use for authentication. | `NONE`
| `CUSTOM_CREDENTIALS_FILE` | The path to a properties file containing AWS security credentials `ACCESS_KEY_ID` and`SECRET_KEY`. This is only required if `AWS_CREDENTIALS_PROVIDER_CLASS` is specified as `PropertiesFileCredentialsProvider`.| `NONE` |

### SAML-Based Authentication Options for Okta

| Option | Description | Default |
|--------|-------------|---------------|
| `IDP_NAME` | The Identity Provider (IdP) name to use for SAML-based authentication. One of `Okta` or `AzureAD`. | `NONE`
| `IDP_HOST` | The hostname of the specified IdP. | `NONE`
| `IDP_USER_NAME` | The user name for the specified IdP account. | `NONE`
| `IDP_PASSWORD` | The password for the specified IdP account. | `NONE`
| `OKTA_APP_ID` | The unique Okta-provided ID associated with the Timestream application. A place to find the AppId is in the entityID field provided in the application metadata. An example entityID=`"http://www.okta.com//<IdpAppID>"`| `NONE`
| `ROLE_ARN` | The Amazon Resource Name (ARN) of the role that the caller is assuming. | `NONE`
| `IDP_ARN` | The Amazon Resource Name (ARN) of the SAML provider in IAM that describes the IdP. | `NONE`

### SAML-Based Authentication Options for Azure Active Directory

| Option | Description | Default |
|--------|-------------|---------------|
| `IDP_NAME` | The Identity Provider (IdP) name to use for SAML-based authentication. One of `Okta` or `AzureAD` | `NONE`
| `IDP_USER_NAME` | The user name for the specified IdP account. | `NONE`
| `IDP_PASSWORD` | The password for the specified IdP account. | `NONE`
| `AAD_APP_ID` | The unique id of the registered application on Azure AD. | `NONE`
| `AAD_CLIENT_SECRET` | The client secret associated with the registered application on Azure AD used to authorize fetching tokens. | `NONE`
| `AAD_TENANT` | The Azure AD Tenant ID. | `NONE`
| `IDP_ARN` | The Amazon Resource Name (ARN) of the SAML provider in IAM that describes the IdP. | `NONE`


## Examples

### Connecting to an Amazon Timestream Cluster

```
DRIVER={Amazon Timestream};ACCESS_KEY_ID=accessKeyId;SECRET_KEY=secretKey;SESSION_TOKEN=sessionToken;REGION=us-east-2;
```

#### Notes

1. AccessKeyId is `accessKeyId` and SecretKey is `secretKey`.
2. Session token is `sessionToken`. Example of session token: 
```
"IQoJb3JpZ2luX2VjEBMaCXVzLXdlc3QtMiJHMEUCIARrHq+wiFok7WeX29yXaaoK4kyq1ytlXMp2j5S+DQ7IAiEAq1DNnhCJD31FN6et4pOjJ/RTkibtqr99n1cMZs+ejzoq7wEITBADGgw1MTgwNDc0OTkyMDkiDNZL+IGSOKLyaDZIKSrMAejzccsRaVIpnkpEvi20gGgnyZ9jtHntA3U7pt4qUbeNDn5rVgjh/FPPx+zfPGYxdbP5ASEl9PXRcaYHznaLGqODv3umRUC5B4Vz3s7hgxgcJstydFwlIAOpaiMZbQ/qXCVauoT2VVr9cD2z/gn/8jk7Ue+BnBxYO41arKCzKoPxCL1DVbc9/eWdKOgxZZGKBkp7bD3uMJzcoJdRaBRLdiXR930PzZNMrbcVf5munnHNjXhFwa7SdYBRvX1cpMvgtb7XbqkBt3G5RE7NDjD/3rGWBjqYAd8JqapgYongooSXlNKM7FwZpvKa6YK3863p/05SckxWaQ2EtlQe29R4dCFwZRxPWoiOAM9sOREKX60b+kFOdssuEFagDu150+UAnRAYmjlMUjJYtoqLVpQ0fcij12OF+6nqX+3nDnjgq2HqfnZ8wAU/Y5mV5uhgbLRyJICfOQoNbU7PxaQ2NtxQclWBwwmSiNFgpgTdMVln"
```
3. Region is `us-east-2`. 
