#include "videoWidget.h"

videoWidget::videoWidget(QWidget *parent) :
    QWidget(parent)
{
    setAutoFillBackground(false);
}

void videoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if(!image.isNull())
    {
        painter.drawImage(QPoint(0,0), image);
    }

}
