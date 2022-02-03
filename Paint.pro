QT       += core gui printsupport svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    Paint.cpp \
    scribblearea.cpp

HEADERS += \
    Paint.h \
    scribblearea.h

TRANSLATIONS += \
    Paint_de.ts

RESOURCES += \
    ressources.qrc

CONFIG += lrelease
CONFIG += embed_translations

!unix: RC_ICONS += Paint.ico
android: include(Qt-Color-Widgets/color_widgets.pri)
android: RESOURCES += themes.qrc
android: TRANSLATIONS += qtbase_de.qm

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
