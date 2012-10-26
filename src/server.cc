#include "server.hh"
#include "callbacks.hh"
#include "sha1.hh"

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
using std::string;


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



bool Server::AddClient( Client *client )
{
  clientList.push_back( client );
  return true;
}



bool Server::AddPool( Pool *pool )
{
  poolList.push_back( pool );
  return true;
}



bool Server::MoveClientToPool( Client *client, Pool *pool )
{
  vector<Client*>::iterator cit;

  bool found = false;

  for( cit = clientList.begin(); cit != clientList.end(); ++cit )
  {
    if( (*cit) == client )
    {
      clientList.erase( cit );
      found = true;
      break;
    }
  }

  if( !found )
    return false;

  pool->AddClient( client );
  std::cout << "Moved client '" << client->id << "' to pool '" << pool->id << "'.\n";
}



Pool* Server::FindPool( string id )
{
  Pool *ret = NULL;

  vector<Pool*>::iterator pit;
  for( pit = poolList.begin(); pit != poolList.end(); ++pit )
  {
    std::cout << "Comparing:\n-" << id << " and\n-" << (*pit)->idHash << "\n";
    if( (*pit)->idHash.compare( id ) == 0 )
    {
      ret = *pit;
      break;
    }
  }

  return ret;
}



Pool* Server::FindSaltedPool( Client *client, string hash )
{
  Pool *ret = NULL;
  unsigned char hashBuf[20];
  char hex[41];

  vector<Pool*>::iterator pit;
  for( pit = poolList.begin(); pit != poolList.end(); ++pit )
  {
    string salted = (*pit)->id;
    salted.append( client->salt );

    sha1::calc( salted.c_str(), salted.length(), hashBuf );
    sha1::toHexString( hashBuf, hex );

    string hashed( hex );

    std::cout << "Comparing:\n-" << hash << " and\n-" << hashed << "\n";
    if( hashed.compare( hash ) == 0 )
    {
      ret = *pit;
      break;
    }
  }

  return ret;
}
