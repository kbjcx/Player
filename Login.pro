QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    widget.cpp\
    video_player.cpp

HEADERS += \
    widget.h\
    video_player.h

INCLUDEPATH += D:/CPP/ffmpeg4.4.1/include

LIBS += D:/CPP/ffmpeg4.4.1/lib/avcodec.lib \
        D:/CPP/ffmpeg4.4.1/lib/avdevice.lib \
        D:/CPP/ffmpeg4.4.1/lib/avfilter.lib \
        D:/CPP/ffmpeg4.4.1/lib/avformat.lib \
        D:/CPP/ffmpeg4.4.1/lib/avutil.lib \
        D:/CPP/ffmpeg4.4.1/lib/postproc.lib \
        D:/CPP/ffmpeg4.4.1/lib/swresample.lib \
        D:/CPP/ffmpeg4.4.1/lib/swscale.lib

# Default rules for deployment.
qnx: target.path = ./
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
