QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = lab3

INCLUDEPATH += src

# промежуточные файлы сборки не засоряют корень репозитория
OBJECTS_DIR = build
MOC_DIR = build

SOURCES += \
    src/main.cpp \
    src/account.cpp \
    src/passwordrules.cpp \
    src/md2.cpp \
    src/rc2.cpp \
    src/cryptofile.cpp \
    src/passphrasedialog.cpp \
    src/logindialog.cpp \
    src/changepassdialog.cpp \
    src/newuserdialog.cpp \
    src/usersdialog.cpp \
    src/mainwindow.cpp

HEADERS += \
    src/account.h \
    src/passwordrules.h \
    src/md2.h \
    src/rc2.h \
    src/cryptofile.h \
    src/passphrasedialog.h \
    src/logindialog.h \
    src/changepassdialog.h \
    src/newuserdialog.h \
    src/usersdialog.h \
    src/mainwindow.h
