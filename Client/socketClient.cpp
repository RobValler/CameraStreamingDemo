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
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h> // close socket

#include "mainWindow.h"
#include "socketClient.h"

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
    int read_count;

    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    //server.sin_addr.s_addr = inet_addr("192.168.178.36");  // WiFi
    //server.sin_addr.s_addr = inet_addr("192.168.178.21");   // Ethernet
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return;
    }

    puts("Connected\n");


    // handle the handshake
    if( recv(sock , &HS , sizeof(handshake) , 0) < 0)
    {
        puts("recv failed");
        return;
    }

    if(HS.ImgBufferSize <= 0)
    {
        puts("Handshake failed");
        return;
    }

    printf("buffer size = %d\n", HS.ImgBufferSize);

    if( send(sock , &HS , sizeof(handshake) , 0) < 0)
    {
        puts("Send failed");
        return;
    }

    // pre loop init
    read_count = 0;
    imageDataBuffer.resize(HS.ImgBufferSize);
    refMainWnd->API_Init_ImageBuffer(HS.ImgBufferSize);


    //keep communicating with server
    while(true)
    {
        if( exit_request == true)
            break;

        // received image data from the socket
        read_count = recv(sock, &imageDataBuffer[0], HS.ImgBufferSize, MSG_WAITALL);

        if( read_count < 0) {
            puts("receive failed");
            //break;
        } else if( read_count == HS.ImgBufferSize ) {
            read_count = 0;

            //refMainWnd->update((void*) &imageDataBuffer[0], (size_t) HS.ImgBufferSize);
            updateFlag = true;
        }
    }
    close(sock);
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


