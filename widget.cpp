#include "widget.h"
#include "QPushButton"

Widget::Widget(QWidget* parent) : QWidget(parent) {
    this->setWindowTitle("Save image");
    this->setFixedSize(400, 400);
    
    QPushButton* save_img = new QPushButton(this);
    save_img->setText("保存图片");
}

Widget::~Widget() noexcept {

}
