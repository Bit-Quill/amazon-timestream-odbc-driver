# Amazon Timestream ODBC Driver

## Overview

The ODBC driver for the Amazon Timestream serverless time series database provides an 
SQL-relational interface for developers and BI tool users.

Full source code is provided. Users should build the library for intended platform.

For build instructions please refer to markdown/setup/developer-guide.md.

For details on ODBC driver installation and usage please refer to rest of this document.

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License

This project is licensed under the Apache-2.0 License.

## Documentation

See the [product documentation](docs/markdown/index.md) for more detailed information about this driver, such as setup and configuration.

## Setup and Usage

To setup and use the Timestream ODBC driver, follow [these directions](docs/markdown/setup/setup.md).

## Connection String Syntax

```
DRIVER={Amazon Timestream ODBC Driver};DSN={Timestream DSN};<option>=<value>;
```

For more information about connecting to an Amazon Timestream database using this ODBC driver, see
the [connection string documentation](docs/markdown/setup/connection-string.md) for more details.

## Driver Info
| Platform | Amazon Timestream ODBC Driver File 
|----------|-----------------------------------|
| Windows | timestream.odbc.dll
| MacOS | libtimestream-odbc.dylib
| Linux | libtimestream-odbc.so
