/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <linux/videodev2.h>

#include "config.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

enum io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR
};

typedef struct _buffer {
        void   *start;
        size_t  length;
}buffer;

static void*            v4l2_thread_func(void* ptr);
static pthread_t        v4l2_thread;
static pthread_mutex_t  v4l2_mutex = PTHREAD_MUTEX_INITIALIZER;
static enum io_method   io = IO_METHOD_MMAP;
static int              fd = -1;
static buffer           *buffers;
static unsigned int     n_buffers;
static int              force_format;
static unsigned long    ImgBufferSize;
static bool             exit_flag;

//image buffer
static unsigned char    *pImgBuffer;

static int errno_exit(const char *s)
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

static int xioctl(int fh, unsigned long request, void *arg)
{
    int r;

    do {
            r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

static void process_image(const void *p, size_t size)
{

	// write to the image buffer
	pthread_mutex_lock(&v4l2_mutex);
		memcpy(pImgBuffer, p, size);
		//pImgData = (unsigned char *)p;
	pthread_mutex_unlock(&v4l2_mutex);
}

static int read_frame(void)
{
        struct v4l2_buffer buf;
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("read");
                        }
                }

                process_image(buffers[0].start, buffers[0].length);
                break;

        case IO_METHOD_MMAP:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                            errno_exit("VIDIOC_DQBUF EAGAIN");
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                assert(buf.index < n_buffers);

                //process_image(buffers[buf.index].start, buf.bytesused);
                process_image(buffers[buf.index].start, buffers[buf.index].length);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;

        case IO_METHOD_USERPTR:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                for (i = 0; i < n_buffers; ++i)
                        if (buf.m.userptr == (unsigned long)buffers[i].start
                            && buf.length == buffers[i].length)
                                break;

                assert(i < n_buffers);

                process_image((void *)buf.m.userptr, buf.bytesused);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;
        }

        return 1;
}

static void mainloop(void)
{

	while(!exit_flag)
	{
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* Timeout. */
		tv.tv_sec = 5;
		tv.tv_usec = 0;

		r = select(fd + 1, &fds, NULL, NULL, &tv);

		if (-1 == r)
		{
			if (EINTR == errno)
			continue;
			errno_exit("select");
		}

		if (0 == r)
		{
			fprintf(stderr, "select timeout\n");
			//exit(EXIT_FAILURE);
			exit_flag = true;
		}

		if (!read_frame())
			break;
		/* EAGAIN - continue select loop. */

	}

}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                        errno_exit("VIDIOC_STREAMOFF");
                break;
        }
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_MMAP;
                        buf.index = i;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_USERPTR;
                        buf.index = i;
                        buf.m.userptr = (unsigned long)buffers[i].start;
                        buf.length = (__u32)buffers[i].length;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;
        }
}

static void uninit_device(void)
{
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                free(buffers[0].start);
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                        if (-1 == munmap(buffers[i].start, buffers[i].length))
                                errno_exit("munmap");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                        free(buffers[i].start);
                break;
        }

        free(buffers);
}

void init_read(unsigned int buffer_size)
{


        buffers = (buffer*)(calloc(1, sizeof(*buffers)));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        buffers[0].length = buffer_size;
        buffers[0].start = malloc(buffer_size);

        if (!buffers[0].start) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }
}

static void init_mmap(void)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 4;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "memory mapping\n", videoDeviceID);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                         videoDeviceID);
                exit(EXIT_FAILURE);
        }

        buffers = (buffer*)(calloc(req.count, sizeof(*buffers)));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }

}

static void init_userp(unsigned int buffer_size)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count  = 4;
        req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "user pointer i/o\n", videoDeviceID);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        buffers = (buffer*)(calloc(4, sizeof(*buffers)));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = malloc(buffer_size);

                if (!buffers[n_buffers].start) {
                        fprintf(stderr, "Out of memory\n");
                        exit(EXIT_FAILURE);
                }
        }
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        unsigned int min;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 videoDeviceID);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         videoDeviceID);
                exit(EXIT_FAILURE);
        }

        switch (io) {
        case IO_METHOD_READ:
                if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                        fprintf(stderr, "%s does not support read i/o\n",
                                 videoDeviceID);
                        exit(EXIT_FAILURE);
                }
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                        fprintf(stderr, "%s does not support streaming i/o\n",
                                 videoDeviceID);
                        exit(EXIT_FAILURE);
                }
                break;
        }


        /* Select video input, video standard and tune here. */


        CLEAR(cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {
                /* Errors ignored. */
        }


        CLEAR(fmt);

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (force_format)
        {

                if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
                        errno_exit("VIDIOC_G_FMT");

                 fmt.fmt.pix.width       = 640;
                 fmt.fmt.pix.height      = 360;
                 fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
                 //fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
                 fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

                 if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                         errno_exit("VIDIOC_S_FMT");


                /* Note VIDIOC_S_FMT may change width and height. */
        } else {
                /* Preserve original settings as set by v4l2-ctl for example */
                if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
                        errno_exit("VIDIOC_G_FMT");
        }

        ImgBufferSize = fmt.fmt.pix.sizeimage;
        pImgBuffer = (unsigned char*)malloc(ImgBufferSize);

        /* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

        printf("size of the image is = %d\n", fmt.fmt.pix.sizeimage);
        printf("driver is sending image at %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

        switch (io) {
        case IO_METHOD_READ:
                init_read(fmt.fmt.pix.sizeimage);
                break;

        case IO_METHOD_MMAP:
                init_mmap();
                break;

        case IO_METHOD_USERPTR:
                init_userp(fmt.fmt.pix.sizeimage);
                break;
        }
}

static void close_device(void)
{
        if (-1 == close(fd))
                errno_exit("close");

        fd = -1;
}

static void open_device(void)
{
        struct stat st;

        if (-1 == stat(videoDeviceID, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                         videoDeviceID, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", videoDeviceID);
                exit(EXIT_FAILURE);
        }

        fd = open(videoDeviceID, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                         videoDeviceID, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}


int init_v4l2()
{
    /* THREADS */
    pthread_attr_t attr;

    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_create( &v4l2_thread, &attr, v4l2_thread_func, NULL );

    return 0;
}


void* v4l2_thread_func(void* ptr)
{
    exit_flag = false;
    force_format = 1;

    printf("Starting video thread!\n");
    open_device();
    init_device();
    start_capturing();
    mainloop();
    stop_capturing();
    uninit_device();
    close_device();

    free(pImgBuffer);
    printf("video thread finished!\n");

    return ptr;
}

void exit_v4l2()
{
    exit_flag = true;
    pthread_join(v4l2_thread, NULL);
}

void v4l2_get_image(void* ptr)
{
	// copy the image buffer to the message buffer
	pthread_mutex_lock(&v4l2_mutex);
    memcpy(ptr, pImgBuffer, ImgBufferSize);
	pthread_mutex_unlock(&v4l2_mutex);
}

unsigned long v4l2_get_image_size()
{
	return ImgBufferSize;
}
