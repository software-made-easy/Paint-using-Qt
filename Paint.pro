QT       += core gui printsupport svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    Paint.cpp \
    paintWidget.cpp \
    # textedit.cpp

HEADERS += \
    Paint.h \
    paintWidget.h \
    # textedit.h

TRANSLATIONS += \
    Paint_de.ts

RESOURCES += \
    ressources.qrc

CONFIG += lrelease
CONFIG += embed_translations

android: include(Qt-Color-Widgets/color_widgets.pri)
!unix || android {
    RESOURCES += themes.qrc
    RC_ICONS = Paint.ico
    ICON = Paint.icns
}


VERSION = 0.6.5
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_TARGET_COMPANY = "Software-made-easy"
QMAKE_TARGET_PRODUCT="Paint"
QMAKE_TARGET_DESCRIPTION="Paint is a simple tool for draw images."

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
