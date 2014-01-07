#-------------------------------------------------
#
# Project created by QtCreator 2012-11-19T13:21:59
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql
QT       += script

TARGET = HSRC
TEMPLATE = app


SOURCES += main.cpp\
    PolarSSL/sha4.c \
    PolarSSL/entropy_poll.c \
    PolarSSL/entropy.c \
    PolarSSL/ctr_drbg.c \
    PolarSSL/aes.c \
    GUI/settingdialog.cpp \
    GUI/serviceprovidersdialog.cpp \
    GUI/operationsdialog.cpp \
    GUI/mylabel2.cpp \
    GUI/mylabel.cpp \
    GUI/mainwindow.cpp \
    GUI/logpreview.cpp \
    GUI/hsrcencodingdialog.cpp \
    GUI/hsrcdecodingdialog.cpp \
    GUI/folderbrowserdialog.cpp \
    GUI/filebrowserdialog.cpp \
    GUI/getcodedialog.cpp \
    DBContr/databasecontrollersetting.cpp \
    DBContr/databasecontrollermetadata.cpp \
    DBContr/databasecontrollerengines.cpp \
    CoreEngines/enginepdp.cpp \
    CoreEngines/enginedelete.cpp \
    CoreEngines/enginededuplication.cpp \
    CoreEngines/enginecryptographichashing.cpp \
    CoreEngines/enginecoding.cpp \
    NetworkContr/sessionubuntuone.cpp \
    NetworkContr/sessionskydrive.cpp \
    NetworkContr/sessiongoogledrive.cpp \
    NetworkContr/sessiondropbox.cpp \
    NetworkContr/networkcontrollerparallel.cpp \

HEADERS  += qjson_export.h \
    parser.h \
    advapi32_export.h \
    PolarSSL/sha4.h \
    PolarSSL/entropy_poll.h \
    PolarSSL/entropy.h \
    PolarSSL/ctr_drbg.h \
    PolarSSL/aes.h \
    GUI/mainwindow.h \
    GUI/hsrcencodingdialog.h \
    GUI/hsrcdecodingdialog.h \
    GUI/mylabel2.h \
    GUI/mylabel.h \
    GUI/settingdialog.h \
    GUI/serviceprovidersdialog.h \
    GUI/operationsdialog.h \
    GUI/mylabel2.h \
    GUI/mylabel.h \
    GUI/mainwindow.h \
    GUI/logpreview.h \
    GUI/hsrcencodingdialog.h \
    GUI/hsrcdecodingdialog.h \
    GUI/filebrowserdialog.h \
    GUI/folderbrowserdialog.h \
    GUI/getcodedialog.h \
    DBContr/databasecontrollersetting.h \
    DBContr/databasecontrollermetadata.h \
    DBContr/databasecontrollerengines.h \
    CoreEngines/enginepdp.h \
    CoreEngines/enginedelete.h \
    CoreEngines/enginededuplication.h \
    CoreEngines/enginecryptographichashing.h \
    CoreEngines/enginecoding.h \
    NetworkContr/sessionubuntuone.h \
    NetworkContr/sessionskydrive.h \
    NetworkContr/sessiongoogledrive.h \
    NetworkContr/sessiondropbox.h \
    NetworkContr/networkcontrollerparallel.h \

FORMS    += Forms/mainwindow.ui \
    Forms/operationsdialog.ui \
    Forms/hsrcencodingdialog.ui \
    Forms/hsrcdecodingdialog.ui \
    Forms/filebrowserdialog.ui \
    Forms/getcodedialog.ui \
    Forms/folderbrowserdialog.ui \
    Forms/settingdialog.ui \
    Forms/serviceprovidersdialog.ui \
    Forms/logpreview.ui \
    Forms/keysecretdialog.ui

LIBS += .\qjson0.lib
LIBS += .\advapi32.lib
