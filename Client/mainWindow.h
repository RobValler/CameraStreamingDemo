/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "socketClient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void update(void* ptr, size_t size);
    void API_Init_ImageBuffer(size_t size);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:

    void convert_YUV422_to_RGB888(uchar* pImg_IN, uint size, uchar* pImg_OUT);
    int clamp(int var);

    Ui::MainWindow *ui;
    std::unique_ptr<socketClient> sock;
    std::vector<uchar> imageDataBuffer;
};

#endif // MAINWINDOW_H
