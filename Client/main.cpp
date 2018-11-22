/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <QApplication>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
