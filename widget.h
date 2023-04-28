
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDesktopWidget>
namespace Ui {
    class Widget;
}

class Widget : public QWidget
{
Q_OBJECT

public:
    explicit Widget(QWidget *parent = NULL);
    ~Widget();

private slots:


private:

};

#endif // WIDGET_H
