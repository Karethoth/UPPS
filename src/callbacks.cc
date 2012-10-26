#include "callbacks.hh"

#include "server.hh"
#include "poolClient.hh"
#include "sha1.hh"

using std::string;


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

    PoolClient *client = new PoolClient( server, bev );

    bufferevent_setcb( bev, ReadCB, NULL, ErrorCB, (void*)client );

    server->AddClient( client );
  }
}



void ErrorCB( struct bufferevent *bev, short error, void *ctx )
{
  PoolClient *client = (PoolClient*)ctx;
  std::cout << "Error by '" << client->id << "'\n";
  client->state = ERROR;
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
  PoolClient *client = (PoolClient*)ctx;
  std::cout << "Received data from '" << client->id << "'\n";

  char *line;
  size_t n;
  int i;

  unsigned char hash[20];
  char hex[41];
  int len;

  while( (line = evbuffer_readln( client->input, &n, EVBUFFER_EOL_LF )) )
  {
    //len = strlen( line )-1;
    //sha1::calc( line, len, hash );
    //sha1::toHexString( hash, hex );
    //evbuffer_add( client->output, "\n", 1 );
    //evbuffer_add( client->output, hex, 40 );
    //evbuffer_add( client->output, "\n", 1 );

    client->HandleMessage( string( line ) );

    free( line );
  }

  if( evbuffer_get_length( client->input ) >= 16000 )
  {
    /* Too long; just process what there is and go on so that the buffer
     * doesn't grow infinitely long. */
    char buf[1024];
    while( evbuffer_get_length( client->input ) )
    {
      int n = evbuffer_remove( client->input, buf, sizeof(buf) );
      evbuffer_add( client->output, buf, n );
    }
    evbuffer_add( client->output, "\n", 1 );
  }
}

