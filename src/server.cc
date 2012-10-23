#include "server.hh"
#include "callbacks.hh"

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

using std::vector;


void Server::Run()
{
  evutil_socket_t listener;
  struct sockaddr_in sin;
  struct event_base *base;
  struct event *listener_event;

  base = event_base_new();
  if( !base )
    return; /*XXXerr*/

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons( 40000 );

  listener = socket( AF_INET, SOCK_STREAM, 0 );
  evutil_make_socket_nonblocking( listener );

#ifndef WIN32
  int one = 1;
  setsockopt( listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one) );
#endif

  if( bind( listener, (struct sockaddr*)&sin, sizeof(sin) ) < 0 )
  {
    perror("bind");
    return;
  }

  if( listen( listener, 16 ) < 0 )
  {
    perror( "listen" );
    return;
  }

  struct CBArg arg;
  arg.server = this;
  arg.base = base;

  listener_event = event_new( base, listener, EV_READ|EV_PERSIST, HandleNewConnection, (void*)&arg );

  /*XXX check it */
  event_add( listener_event, NULL );

  event_base_dispatch( base );
}

