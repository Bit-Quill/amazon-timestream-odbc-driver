# Amazon Timestream ODBC Driver Setup on Linux 

## Install Timestream ODBC Driver
1. Download the Timestream ODBC installer deb package according to your Linux distro and architecture.
2. Install the package

### Sample command for Debian 64 bit 
```
sudo dpkg -i AmazonTimestreamODBC_2.0.0_amd64.deb
```

3. Register the ODBC driver by running the following script.
```
sudo /usr/lib64/timestream-odbc64/postinst_unix.sh
```

## Next Steps

- [Set up the DSN](linux-dsn-configuration.md)
