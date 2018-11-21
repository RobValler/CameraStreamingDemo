/*
 * v4l2.h
 *
 *  Created on: Feb 5, 2013
 *      Author: robert
 */

#ifndef V4L2_H_
#define V4L2_H_

int init_v4l2();
void exit_v4l2();
void v4l2_get_image(void* ptr);
int v4l2_get_image_size();


#endif /* V4L2_H_ */
