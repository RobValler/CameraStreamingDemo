#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QPainter>

class videoWidget : public QWidget
{
Q_OBJECT
public:
    explicit videoWidget(QWidget *parent = nullptr);
    QImage image;

protected:
    void paintEvent(QPaintEvent *event);


};

#endif // VIDEOWIDGET_H
