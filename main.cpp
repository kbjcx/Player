#include <QApplication>
#include <QPushButton>
#include "widget.h"
#include <QDebug>
#include <QTextCodec>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    
    Widget w;
    w.show();
    
    return QApplication::exec();
}
