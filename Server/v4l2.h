/*****************************************************************
 * Copyright (C) 2017-2018 Robert Valler - All rights reserved.
 *
 * This file is part of the project: DevPlatformAppCMake.
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef V4L2_H_
#define V4L2_H_

int init_v4l2();
void exit_v4l2();
void v4l2_get_image(void* ptr);
unsigned long v4l2_get_image_size();

#endif /* V4L2_H_ */
