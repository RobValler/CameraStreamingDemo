/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include<stdio.h>
#include "server.h"
#include "v4l2.h"

int main(int argc, char *argv[])
{
	init_v4l2();
	socket_init();

    /* The program exits after a keypress*/
    getchar();

	socket_exit();
	exit_v4l2();

	return 0;
}
