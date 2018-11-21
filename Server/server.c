/*
 * server.c
 *
 *  Created on: Feb 5, 2013
 *      Author: robert
 */

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
//#include <x86_64-linux-gnu/bits/pthreadtypes.h>   // for declaration of pthread_t, pthread_attr_t
#include <stdbool.h>

#include "v4l2.h"

static void* socket_thread_func(void* ptr);

typedef struct _HandshakeStruct {
	int ImgBufferSize;
}handshake;

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t socket_thread;

bool exit_request = false;

int state;

enum states {
	STATE_INIT,
	STATE_LISTEN,
	STATE_HANDSHAKE,
	STATE_PRE_RUNNING,
	STATE_RUNNING,
	STATE_EXIT
};





int socket_init()
{
    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_create( &socket_thread, &attr, socket_thread_func, NULL );
    return 0;
}

int socket_exit()
{
	state = STATE_EXIT;

//    pthread_join(socket_thread, NULL);

    return 0;
}


static void* socket_thread_func(void* ptr)
{
	int write_count;
	int bind_err_count, accept_err_count;
	int socket_desc , client_sock , c;
	struct sockaddr_in server , client;
	unsigned char* client_message = NULL;
	bool active;

	handshake HS_request;
	handshake HS_result;

	// init stuff
	bind_err_count = 0;
	accept_err_count = 0;
	state = STATE_INIT;
	active = true;

	while(active)
	{
		// loopiness

		switch(state)
		{
			case STATE_INIT:
			{
				//Create socket
				socket_desc = socket(AF_INET , SOCK_STREAM , 0);
				if (socket_desc == -1)
				{
					printf("Could not create socket");
					state = STATE_EXIT;
					break;
				}
				printf("Socket created\n");

				//Prepare the sockaddr_in structure
				server.sin_family = AF_INET;
				server.sin_addr.s_addr = INADDR_ANY;
				server.sin_port = htons( 8888 );

				//Bind
				if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
				{
					//print the error message
					perror("bind failed. Error");

					if(bind_err_count > 5)
					{
						perror("Too many bind errors, exiting!");
						state = STATE_EXIT;
					}

				}
				else
				{
					puts("bind done");
					state = STATE_LISTEN;
				}

				break;
			}
			case STATE_LISTEN:
			{
				//Listen
				listen(socket_desc , 3);

				//Accept and incoming connection
				puts("Waiting for incoming connections...");
				c = sizeof(struct sockaddr_in);

				//accept connection from an incoming client
				client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
				if (client_sock < 0)
				{
					perror("accept failed");
					state = STATE_EXIT;
				}
				else
				{
					puts("Connection accepted");
					state = STATE_HANDSHAKE;
				}

				break;
			}
			case STATE_HANDSHAKE:
			{

				// handle the handshaking
				sleep(2);

				HS_request.ImgBufferSize = v4l2_get_image_size();
				write(client_sock , &HS_request , sizeof(handshake));

				// wait for the OK message
				recv(client_sock , &HS_result , sizeof(handshake) , 0);

				if(HS_request.ImgBufferSize != HS_result.ImgBufferSize)
				{
					puts("Handshaking failed! Exiting.");
					state = STATE_EXIT;
				}
				else
				{
					puts("Handshaking OK !");
					state = STATE_PRE_RUNNING;
				}

				break;
			}
			case STATE_PRE_RUNNING:
			{
				// pre loop init
				client_message = (unsigned char*)malloc(HS_result.ImgBufferSize);
				write_count = 0;
				state = STATE_RUNNING;
				break;
			}
			case STATE_RUNNING:
			{

				if( write_count == 0 )
				{
					v4l2_get_image(client_message);
				}

				write_count = write(client_sock , (void*)client_message , HS_result.ImgBufferSize);
				if( write_count == HS_result.ImgBufferSize )
				{
					write_count = 0;
				}
				else if( write_count < 0 )
				{
					puts("Error writing data");
					free(client_message);
					state = STATE_LISTEN;
					break;
				}

				break;
			}
			case STATE_EXIT:
			default:
			{

				fprintf(stderr, "Socket thread finished!\n");

				// clean up
				if(client_message)
					free(client_message);

				close(client_sock);

				active = false;
				break;
			}
		} //end switch
	} // end while

	return ptr;
}


