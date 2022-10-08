# Amazon Timestream ODBC Driver Setup on Linux 

## Prerequisites
In order to install the Timestream ODBC Driver, [unixODBC](http://www.unixodbc.org/) must be installed.

### Installing on Ubuntu 64 bit

```
sudo apt update
sudo apt install unixodbc
```

### Installing on Amazon Linux 2 64 bit

```
sudo yum update
sudo yum install unixODBC
```

### Installing on Linux via Docker 64 bit

```
[sudo] apt update
[sudo] apt install unixodbc unixodbc-dev
```

## Install Timestream ODBC Driver
1. Download the Timestream ODBC installer deb package according to your Linux distro and architecture.
2. Install the package

#### Sample command for Debian 64 bit 
```
sudo dpkg -i AmazonTimestreamODBC_2.0.0_amd64.deb
```

#### Sample command for Amazon Linux2 64 bit
```
sudo rpm -i AmazonTimestreamODBC_2.0.0_amd64.rpm
```


3. Register the ODBC driver by running the following script.
```
sudo /usr/lib64/timestream-odbc64/postinst_unix.sh
```

## Next Steps

- [Set up the DSN](linux-dsn-configuration.md)

## Uninstall Timestream ODBC Driver
There is no automatical way to uninstall it. It has to be done manually.
1. Remove "Amazon Timestream ODBC Driver" entry from /etc/odbcinst.ini
2. Remove the driver residence directory /usr/lib64/timestream-odbc64
```
sudo rm -rf /Library/ODBC/timestream-odbc
```

3. Remove the libraries installed to /usr/lib
```
sudo rm /usr/lib/libtimestream-odbc.so /usr/lib/libtimestream-odbc.so.2.0.0
```

