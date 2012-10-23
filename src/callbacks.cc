#include "callbacks.hh"

#include "server.hh"
#include "poolUser.hh"

void ErrorCB( struct bufferevent*, short, void* );
void ReadCB( struct bufferevent*, void* );



void HandleNewConnection( evutil_socket_t listener, short event, void *arg )
{
  CBArg *cbArg = (struct CBArg*)arg;

  Server *server = (Server*)cbArg->server;

  struct event_base *base = (struct event_base*)cbArg->base;
  struct sockaddr_storage ss;

  socklen_t slen = sizeof(ss);

  int fd = accept( listener, (struct sockaddr*)&ss, &slen );
  if( fd < 0 )
  {
    perror( "accept" );
  }
  else if( fd > FD_SETSIZE )
  {
      close( fd );
  }
  else
  {
    struct bufferevent *bev;
    evutil_make_socket_nonblocking( fd );
    bev = bufferevent_socket_new( base, fd, BEV_OPT_CLOSE_ON_FREE );
    bufferevent_setcb( bev, ReadCB, NULL, ErrorCB, NULL );
    bufferevent_setwatermark( bev, EV_READ, 0, 16000 );
    bufferevent_enable( bev, EV_READ|EV_WRITE );
  }
}



void ErrorCB( struct bufferevent *bev, short error, void *ctx )
{
  if( error & BEV_EVENT_EOF )
  {
    /* connection has been closed, do any clean up here */
    /* ... */
  } else if (error & BEV_EVENT_ERROR) {
    /* check errno to see what error occurred */
    /* ... */
  } else if (error & BEV_EVENT_TIMEOUT) {
    /* must be a timeout event handle, handle it */
    /* ... */
  }

  bufferevent_free( bev );
}



void ReadCB( struct bufferevent *bev, void *ctx )
{
  struct evbuffer *input, *output;
  char *line;
  size_t n;
  int i;

  input  = bufferevent_get_input( bev );
  output = bufferevent_get_output( bev );

  while( (line = evbuffer_readln( input, &n, EVBUFFER_EOL_LF )) )
  {
    evbuffer_add( output, line, n );
    evbuffer_add( output, "\n", 1 );
    free( line );
  }

  if( evbuffer_get_length( input ) >= 16000 )
  {
    /* Too long; just process what there is and go on so that the buffer
     * doesn't grow infinitely long. */
    char buf[1024];
    while( evbuffer_get_length( input ) )
    {
      int n = evbuffer_remove( input, buf, sizeof(buf) );
      evbuffer_add(output, buf, n);
    }
    evbuffer_add(output, "\n", 1);
  }
}

