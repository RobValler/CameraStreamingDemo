/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <stdio.h>	//printf
#include <string.h>	//strlen

#include "socketClient.h"
#include "mainWindow.h"
#include "comms/comm_client.h"
#include "example.pb.h"

socketClient::socketClient(MainWindow* ref): refMainWnd(ref)
{
    exit_request = false;
    updateFlag = false;
    t_socketRead = std::thread(&socketClient::f_socketRead, this);
    t_imageForward = std::thread(&socketClient::f_imageForward, this);
}

socketClient::~socketClient()
{
    exit_request = true;
    t_socketRead.join();
    t_imageForward.join();
    imageDataBuffer.empty();
}

void socketClient::f_socketRead()
{
    CCommClient sock_client(client_proto::EPT_TCTPIP, client_proto::EST_PROTO);
    camera_msg in;
    std::uint32_t size;

    if(!sock_client.connect("127.0.0.1"))
        return;

    std::uint32_t index = 0;

    while(true)
    {
        if( exit_request == true)
            break;

        int read_size;
        if(!sock_client.read(&in, read_size))
            break;

        size = in.data_size();
        imageDataBuffer = {};
        for(index = 0; index < size; ++index) {
            imageDataBuffer.at(index) = in.data(index);
        }
        if(index == size)
            updateFlag = true;
    }
}

void socketClient::f_imageForward()
{
    while(true)
    {
        if( exit_request == true)
            break;

        if(updateFlag){

            // send the image data to the image renderer
            refMainWnd->update((void*) &imageDataBuffer[0], (size_t) HS.ImgBufferSize);
            updateFlag = false;
        }
    }
}
