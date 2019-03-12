

#ifdef __cplusplus
extern "C" {
#endif

#include "server.h"
#include "v4l2.h"

#ifdef __cplusplus
} //end extern "C"
#endif



#include "serv.h"


serv::serv()
{
    init_v4l2();
    socket_init();
}

serv::~serv()
{
    socket_exit();
    exit_v4l2();
}
