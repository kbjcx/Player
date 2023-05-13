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

INCLUDEPATH += E:/CPP/ffmpeg4.4.4/include

LIBS += E:/CPP/ffmpeg4.4.4/lib/avcodec.lib \
        E:/CPP/ffmpeg4.4.4/lib/avdevice.lib \
        E:/CPP/ffmpeg4.4.4/lib/avfilter.lib \
        E:/CPP/ffmpeg4.4.4/lib/avformat.lib \
        E:/CPP/ffmpeg4.4.4/lib/avutil.lib \
        E:/CPP/ffmpeg4.4.4/lib/postproc.lib \
        E:/CPP/ffmpeg4.4.4/lib/swresample.lib \
        E:/CPP/ffmpeg4.4.4/lib/swscale.lib

# Default rules for deployment.
qnx: target.path = ./
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
