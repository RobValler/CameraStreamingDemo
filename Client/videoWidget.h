/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QPainter>

class videoWidget : public QWidget
{
Q_OBJECT
public:
    explicit videoWidget(QWidget *parent = nullptr);

    // todo: fix using Qt SLOT
    void updateImage(QImage& i){
        update();
        image = i;
    }

private:
    void paintEvent(QPaintEvent *event);
    QImage image;
};

#endif // VIDEOWIDGET_H
