
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>

#include "video_player.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;
    
protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void slot_get_frame(QImage img);

private:
    VideoPlayer* video_player_;
    QImage image_;
};

#endif // WIDGET_H
