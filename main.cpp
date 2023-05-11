#include <QApplication>
#include <QPushButton>
#include "widget.h"
#include <QDebug>

extern "C"{
    #include <libavcodec//avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/pixfmt.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}

#include "save_img.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    int version =avformat_version();
    qDebug() << "version:" << version << endl;
    
   
    
    return QApplication::exec();
}
