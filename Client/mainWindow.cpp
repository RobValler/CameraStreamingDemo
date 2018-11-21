#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "socketClient.h"

// include all C compiled object here
#if defined(__cplusplus)
extern "C"
{
#endif

//#include "socket_client.h"

#if defined(__cplusplus)
}
#endif

void API_Update(void* ptr, size_t size);
void API_Init_ImageBuffer(size_t size);
static int clamp(int var);

uchar* ImageBuffer;
MainWindow* pMainObj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pMainObj = this;

    socket_init();

}

MainWindow::~MainWindow()
{
    delete ui;
    socket_exit();
}

void MainWindow::update(void* ptr, size_t size)
{
    convert_YUV422_to_RGB888((uchar*)ptr, (uint)size, &ImageBuffer[0]);

    QImage tmpQImg(ImageBuffer, ui->VidWidget->width(), ui->VidWidget->height(), QImage::Format_RGB888);

    ui->VidWidget->image = tmpQImg;
    ui->VidWidget->update();
}

int MainWindow::convert_YUV444_to_RGB888(int y_var, int u_var, int v_var, int* R_var, int* G_var, int* B_var )
{
    /*YUV444 = 3 bytes per pixel*/

    int c_var, d_var, e_var;

    c_var = (y_var - 16);
    d_var = (u_var - 128);
    e_var = (v_var - 128);

    *R_var = clamp((298 * c_var + 409 * e_var + 128) >> 8);
    *G_var = clamp((298 * c_var - 100 * d_var - 208 * e_var + 128) >> 8);
    *B_var = clamp((298 * c_var + 516 * d_var + 128) >> 8);

}

int MainWindow::convert_YUV422_to_RGB888(uchar* pImg_IN, uint size, uchar* pImg_OUT)
{
    /*  YUV422 = 4 bytes per 2 pixels
        RGB = 3 bytes per pixel*/

    int R_var, G_var, B_var;
    int y1_var, y2_var, u_var, v_var;
    uint RGB_arraysize;
    int i, t;

    i = 0;
    t = 0;
    RGB_arraysize = uint((size / 2) * 3);

    for ( i = 0; i < size ; )
    {

        y1_var = pImg_IN[i++];
        u_var = pImg_IN[i++];
        y2_var = pImg_IN[i++];
        v_var = pImg_IN[i++];

        // RGB 1
        convert_YUV444_to_RGB888(y1_var, u_var, v_var, &R_var, &G_var, &B_var);
        pImg_OUT[t++] = R_var;
        pImg_OUT[t++] = G_var;
        pImg_OUT[t++] = B_var;

        // RGB 2
        convert_YUV444_to_RGB888(y2_var, u_var, v_var, &R_var, &G_var, &B_var);
        pImg_OUT[t++] = R_var;
        pImg_OUT[t++] = G_var;
        pImg_OUT[t++] = B_var;

        //
        if(( i > size ) ||
           ( t > RGB_arraysize ))
        {
            break;
        }

    }


    return 0;
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

void API_Update(void* ptr, size_t size)
{
    pMainObj->update(ptr, size);
}

void API_Init_ImageBuffer(size_t size)
{
    size = ((size / 2) * 3); // size is for YUV422, need to change to RGB888
    ImageBuffer = (uchar*)malloc(size);
}

int clamp(int var)
{
    if( var < 0 )
        return 0;
    else if (var > 255)
        return 255;
    else
        return var;
}
