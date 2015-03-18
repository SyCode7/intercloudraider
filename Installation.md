## Download ##

The source code of the InterCloud RAIDer can be downloaded by following the instructions in the [Source tab](https://code.google.com/p/intercloudraider/source/checkout).

## Prerequisites ##

InterCloud RAIDer makes use of some free lightweight libraries in its implementation.

### Qt ###

The implementation of InterCloud RAIDer is developed under the [Qt/C++](http://qt.digia.com/) framework. If you want to make any modification and improve the original code, or run InterCloud RAIDer by using the original source code, please download [Qt4.8](http://qt-project.org/downloads). The current version of InterCloudRAIDer does not support Qt5.0+.

### PolarSSL ###

The Provable Data Possession (PDP) mechanism of InterCloud RAIDer is using [PolarSSL](https://polarssl.org/) in its algorithms. Since each cipher and hashing algorithm in PolarSSL (AES, MD5, SHA-1, etc.) is self-contained and can be easily reused as a standalone module, we used a modified version of PolarSSL which only including the implementation of [AES](https://polarssl.org/aes-source-code) and a version of random number generator, i.e., [CTR-DRBG](https://polarssl.org/ctr-drbg-source-code).

The modified version (based on [version 1.2.8](https://polarssl.org/download-archive)) including the following files:
  1. `aes.h` and `aes.c`
  1. `ctr_drbg.h` and `ctr_drbg.c`
  1. `entropy.h` and `entropy.c`
  1. `entropy_poll.h` and `entropy_poll.c`
  1. `sha4.h` and `sha4.c`

If there is any linking error that is related to `<wincrypt.h>`, which is used in `entropy_poll.c`, when running InterCloud RAIDer on your platform, the following library is built for the implementation of `<wincrypt.h>` by using Qt:
  1. `advapi32_export.h`
  1. `advapi32.lib`
  1. `advapi32.dll`

### QJson ###

[QJson](http://qjson.sourceforge.net/) is a Qt-based library that maps [JSON](http://www.json.org/) data to [QVariant](http://qt-project.org/doc/qt-5.0/qtcore/qvariant.html) objects and vice versa. Since the network module of InterCloud RAIDer only need to parse the JSON object that is receiving from cloud services when making network requests, we used a modified version of QJson library that is able to use the [parsing feature](http://qjson.sourceforge.net/usage).

The modified version including the following files:
  1. `parser.h`
  1. `qjson_export.h`
  1. `qjson0.lib`
  1. `qjson0.dll`

The QJson library (`qjson_export.h`, `qjson0.lib` and `qjson0.dll`) is built dynamically by using Qt framework.

## Compiling and Running by Source Code ##

### Step1 : Compiling ###

After downloading the [Qt SDK of version 4.8 or above](http://qt-project.org/downloads), please follow the following steps to build InterCloud RAIDer:
  1. Run the file `InterCloudRAIDer.pro` in the downloaded package of InterCloud RAIDer by using [Qt Creator](http://qt-project.org/doc/qtcreator-2.8/). The project **InterCloudRAIDer** is now loaded to your Qt Creator.
  1. In the sidebar on the left, select **Projects**, and modify your build directory to a correct path under the group box entitled **General**.
  1. Press Ctrl + B to build.

### Step2 : Running ###

After building the InterCloud RAIDer, press Ctrl + R to run.

## Code Structure ##

The important classes and packages in the code are:
  1. `CoreEngines` - contains main classes for executing the core features of InterCloud RAIDer, e.g., **data deduplication**, **erasure coding** and **PDP**.
  1. `DBContr` - contains classes for manipulating the local database of InterCloud RAIDer. The database is written in [SQLite3](http://www.sqlite.org/) by using the [internal library of Qt](http://qt-project.org/doc/qt-4.8/qtsql.html).
  1. `NetworkContr` - contains classes for handling all network requests making to cloud services, i.e., [authorization protocol](ManualPage#Authorization_Protocol.md), uploading, downloading and deleting.
  1. `GUI` - contains classes for managing the graphical user interface of InterCloud RAIDer.