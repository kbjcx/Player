#include "widget.h"
#include "QPushButton"
#include "QPoint"
#include <QtMath>

#include "video_player.h"

Widget::Widget(QWidget* parent) : QWidget(parent) {
    this->setWindowTitle("Save image");
    this->setFixedSize(1920, 1080);
    video_player_ = new VideoPlayer();
    
    void (VideoPlayer::*signal_get_frame_)(QImage) = &VideoPlayer::signal_get_frame;
    void (Widget::*slot_get_frame_)(QImage) = &Widget::slot_get_frame;
    connect(video_player_, signal_get_frame_, this, slot_get_frame_);
    video_player_->set_file_name("../Source/test.mp4");
    video_player_->start_play();
}

Widget::~Widget() noexcept = default;

void Widget::slot_get_frame(QImage img) {
    printf("get signal, image width %d \n", img.width());
    image_ = img;
    update();
//    repaint();
}

void Widget::paintEvent(QPaintEvent* event) {
    printf("start paint \n");
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); // 先画成黑色
    QImage img;
    printf("%d %d \n", image_.size().width(), image_.size().height());
    if(image_.size().width() <= 0)
        painter.drawImage(0,0,img);
        /*
            一定要加标记位判断，控件在绘制之前的size为NULL，
            所以scaled()返回值也为NULL，会提示nImg是空的
        */
    else
    {
        img = image_.scaled(width(),height());
        int x = this->width() - img.width();
        int y = this->height() - img.height();

        x /= 2;
        y /= 2;

//        painter.drawImage(0, 0, img); // 画出图像
        painter.drawImage(QPoint(x, y),img);
    }
//    image_ = image_.scaled(this->size(), Qt::KeepAspectRatio)
}
