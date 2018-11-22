/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "mainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sock = std::make_unique<socketClient>(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    sock.reset();
    imageDataBuffer.empty();
}

void MainWindow::update(void* ptr, size_t size)
{
    convert_YUV422_to_RGB888( static_cast<uchar*>(ptr), static_cast<uint>(size), &imageDataBuffer[0]);
    QImage tmpQImg(&imageDataBuffer[0], ui->VidWidget->width(), ui->VidWidget->height(), QImage::Format_RGB888);
    ui->VidWidget->updateImage(tmpQImg);
}

void MainWindow::convert_YUV422_to_RGB888(uchar* pImg_IN, uint size, uchar* pImg_OUT)
{
    /*  YUV422 = 4 bytes per 2 pixels
        RGB = 3 bytes per pixel*/

    std::function<void(int, int, int, int*, int*, int*)> convert_YUV444_to_RGB888 =
            [this](int y_var, int u_var, int v_var, int* R_var, int* G_var, int* B_var)
    {
        /*YUV444 = 3 bytes per pixel*/

        int c_var, d_var, e_var;

        c_var = (y_var - 16);
        d_var = (u_var - 128);
        e_var = (v_var - 128);

        *R_var = clamp((298 * c_var + 409 * e_var + 128) >> 8);
        *G_var = clamp((298 * c_var - 100 * d_var - 208 * e_var + 128) >> 8);
        *B_var = clamp((298 * c_var + 516 * d_var + 128) >> 8);
    };

    int R_var, G_var, B_var;
    int y1_var, y2_var, u_var, v_var;
    uint RGB_arraysize;
    uint t = 0;
    uint i;

    RGB_arraysize = uint((size / 2) * 3);

    for ( i = 0; i < size ; )
    {

        y1_var = pImg_IN[i++];
        u_var = pImg_IN[i++];
        y2_var = pImg_IN[i++];
        v_var = pImg_IN[i++];

        // RGB 1
        convert_YUV444_to_RGB888(y1_var, u_var, v_var, &R_var, &G_var, &B_var);
        pImg_OUT[t++] = static_cast<uchar>(R_var);
        pImg_OUT[t++] = static_cast<uchar>(G_var);
        pImg_OUT[t++] = static_cast<uchar>(B_var);

        // RGB 2
        convert_YUV444_to_RGB888(y2_var, u_var, v_var, &R_var, &G_var, &B_var);
        pImg_OUT[t++] = static_cast<uchar>(R_var);
        pImg_OUT[t++] = static_cast<uchar>(G_var);
        pImg_OUT[t++] = static_cast<uchar>(B_var);

        //
        if(( i > size ) ||
           ( t > RGB_arraysize ))
        {
            break;
        }
    }
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_2_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
    qApp->quit();
}

void MainWindow::API_Init_ImageBuffer(size_t size)
{
    size = ((size / 2) * 3); // size is for YUV422, need to change to RGB888
    imageDataBuffer.resize(size);
}

int MainWindow::clamp(int var)
{
    if( var < 0 )
        return 0;
    else if (var > 255)
        return 255;
    else
        return var;
}
