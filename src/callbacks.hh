#ifndef __CALLBACKS_HH__
#define __CALLBACKS_HH__

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <errno.h>



extern void HandleNewConnection( evutil_socket_t, short, void* );
extern void ErrorCB( struct bufferevent*, short, void* );
extern void ReadCB( struct bufferevent*, void* );


struct CBArg
{
  void *server;
  void *base;
};

#endif

