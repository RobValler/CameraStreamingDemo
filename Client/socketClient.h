/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <thread>
#include <atomic>
#include <vector>

class MainWindow;

class socketClient
{
public:
    struct handshake {
        int ImgBufferSize;
    };

    socketClient(MainWindow* ref);
    ~socketClient();

private:
    void f_socketRead();
    void f_imageForward();
    std::thread t_socketRead;
    std::thread t_imageForward;
    std::atomic<bool> exit_request;
    std::atomic<bool> updateFlag;
    MainWindow* refMainWnd;
    std::vector<unsigned char> imageDataBuffer;
    handshake HS;
};


#endif // SOCKETCLIENT_H
