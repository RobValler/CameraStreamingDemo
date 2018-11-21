/*
    C ECHO client example using sockets
*/
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h> // close socket

#include "mainWindow.h"

typedef struct _HandshakeStruct {
    int ImgBufferSize;
}handshake;



static void* socket_thread_func(void* ptr);


pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t socket_thread;

unsigned char* Imgbuffer;
bool exit_request = false;

int socket_init()
{
    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_create( &socket_thread, &attr, socket_thread_func, NULL );
    return 0;
}

static void* socket_thread_func(void* ptr)
{
    extern void API_Update(void* ptr, size_t size);
    extern void API_Init_ImageBuffer(size_t size);

    int read_count, read_total;
    handshake HS;
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
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return ptr;
    }

    puts("Connected\n");


    // handle the handshake
    if( recv(sock , &HS , sizeof(handshake) , 0) < 0)
    {
        puts("recv failed");
        return ptr;
    }

    if(HS.ImgBufferSize <= 0)
    {
        puts("Handshake failed");
        return ptr;
    }

    printf("buffer size = %d\n", HS.ImgBufferSize);

    if( send(sock , &HS , sizeof(handshake) , 0) < 0)
    {
        puts("Send failed");
        return ptr;
    }

    // pre loop init
    read_count = 0;
    Imgbuffer = (unsigned char*)malloc(HS.ImgBufferSize);
    API_Init_ImageBuffer(HS.ImgBufferSize);

    //keep communicating with server
    while(true)
    {

        // received image data
        read_count = recv(sock, Imgbuffer, HS.ImgBufferSize, MSG_WAITALL);
        if( read_count < 0)
        {
            puts("receive failed");
            //break;
        }
        else if( read_count == HS.ImgBufferSize )
        {
            read_count = 0;
            // update the image
            API_Update((void*) Imgbuffer, (size_t) HS.ImgBufferSize);
        }

        pthread_mutex_lock(&socket_mutex);
            if( exit_request == true)
            {
                break;
            }
        pthread_mutex_unlock(&socket_mutex);

    }

    close(sock);
    return ptr;
}

int socket_exit()
{
    pthread_mutex_lock(&socket_mutex);
        exit_request = true;
    pthread_mutex_unlock(&socket_mutex);

    pthread_join(socket_thread, NULL);

    free(Imgbuffer);
    return 0;
}
