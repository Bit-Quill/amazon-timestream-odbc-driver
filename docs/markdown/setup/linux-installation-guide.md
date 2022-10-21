# Amazon Timestream ODBC Driver Setup on Linux 

## Prerequisites
In order to install the Timestream ODBC Driver, [unixODBC](http://www.unixodbc.org/) must be installed.

### Installing on Ubuntu 64 bit

```
sudo apt update
sudo apt install unixodbc
```

### Installing on Ubuntu 32 bit

```
sudo dpkg --add-architecture i386
sudo apt update
sudo apt install unixodbc:i386
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

#### Sample command for Debian 32 bit 
```
sudo dpkg -i AmazonTimestreamODBC_2.0.0_amd32.deb
```

#### Sample command for Amazon Linux2 64 bit
```
sudo rpm -i AmazonTimestreamODBC_2.0.0_amd64.rpm
```


3. Register the ODBC driver by running the following script.
#### Command for Linux 32 bit
```
sudo /usr/lib/timestream-odbc32/postinst_unix32.sh
```

#### Command for Linux 64 bit
```
sudo /usr/lib64/timestream-odbc64/postinst_unix64.sh
```

## Next Steps

- [Set up the DSN](linux-dsn-configuration.md)

## Uninstall Timestream ODBC Driver
There is no automatical way to uninstall it. It has to be done manually.
1. Remove "Amazon Timestream ODBC Driver" entry from /etc/odbcinst.ini
2. Remove the driver residence directory
#### Command for Linux 32 bit
```
sudo rm -rf /usr/lib/timestream-odbc32
```

#### Command for Linux 64 bit
```
sudo rm -rf /usr/lib64/timestream-odbc64
```

3. Remove the libraries installed to /usr/lib (for Linux 64-bit only)
```
sudo rm /usr/lib/libtimestream-odbc.so /usr/lib/libtimestream-odbc.so.2.0.0
```

## Notes
The Linux packages could only be used on Linux with equal or higher kernel version than the package is built on.
The following table list the package and the linux kernel version it is built on.

```
--------------------------------------------------------
|        Package           |    Linux Kernel Version   |
--------------------------------------------------------
| Linux 32 bit deb package |        5.15.0             |
--------------------------------------------------------
| Linux 64 bit deb package |        5.15.0             |
--------------------------------------------------------
| Linux 32 bit rpm package |        5.15.0             |
--------------------------------------------------------
| Linux 64 bit rpm package |        5.10.135           |
--------------------------------------------------------
```
