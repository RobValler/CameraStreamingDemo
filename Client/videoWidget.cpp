/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

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
