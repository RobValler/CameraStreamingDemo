/*
	C socket server example
*/


#include<stdio.h>
#include "server.h"
#include "v4l2.h"

#include "v4l2.h"

int main(int argc , char *argv[])
{

	init_v4l2();
	socket_init();

	getchar();

	socket_exit();
	exit_v4l2();


	return 0;
}
