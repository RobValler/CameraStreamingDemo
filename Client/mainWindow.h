#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

void API_Update(void* ptr, size_t size);

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void update(void* ptr, size_t size);

private:

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;


protected:
    int convert_YUV444_to_RGB888(int y_var, int u_var, int v_var, int* R_var, int* G_var, int* B_var );
    int convert_YUV422_to_RGB888(uchar* pImg_IN, uint size, uchar* pImg_OUT);

};

#endif // MAINWINDOW_H
