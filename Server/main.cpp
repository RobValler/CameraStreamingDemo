/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include "v4l2.h"
#include <stdio.h>
#include <signal.h>
//#include "server.h"

#ifdef __cplusplus
}
#endif

#include <vector>
#include <iostream>
#include <atomic>

#include "example.pb.h"
#include "comms/comm_server.h"


namespace {
    std::atomic<bool> isInterrupted;
}

void my_handler(int)
{
    isInterrupted = true;
    std::cout << "program aborted" << std::endl;
}

//int main(int argc, char *argv[])
int main()
{      
    CCommServer server(server_proto::EPT_TCTPIP, server_proto::EST_PROTO);
    camera_msg out;
    std::vector<char> message;
    std::uint32_t size = 0;
    std::uint32_t messages_sent = 0;

    // interrupt handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL); // CTRL+C
    isInterrupted = false;


	init_v4l2();
    //socket_init();

    server.init();

    while(!isInterrupted)
    {
        size = v4l2_get_image_size();
        message.resize(size);
        v4l2_get_image(&message[0]);

        for(std::uint32_t index = 0; index < size; ++index) {
            out.add_data(message.at(index));
        }

        if(!server.write(&out)) {
            //std::cout << "messages was not sent = " << messages_sent << std::endl;
            continue;
        }
        else
        {
            std::cout << "messages sent = " << messages_sent << std::endl;
        }

        out.clear_data();
        messages_sent++;
    }

    std::cout << "messages sent = " << messages_sent << std::endl;

    /* The program exits after a keypress*/
   // getchar();

    //socket_exit();
	exit_v4l2();

	return 0;
}
