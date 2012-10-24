#include "poolClient.hh"

#include <event2/bufferevent.h>
#include <event2/buffer.h>

using std::string;


PoolClient::PoolClient( Server *srv, struct bufferevent *bev ) : Client()
{
  server = srv;
  input  =  bufferevent_get_input( bev );
  output =  bufferevent_get_output( bev );

  bufferevent_setwatermark( bev, EV_READ, 0, 16000 );
  bufferevent_enable( bev, EV_READ|EV_WRITE );

  
  string ridMsg( "RID:" );
  ridMsg.append( id );
  ridMsg.append( "\n" );

  evbuffer_add( output, ridMsg.c_str(), ridMsg.length() );
}



bool PoolClient::HandleMessage( string msg )
{
  string cmd, data;
  size_t cmdEnd = msg.find( ":" );

  if( cmdEnd == string::npos )
    return false;

  cmd = msg.substr( 0, msg.find( ":" ) );
  data = msg.substr( cmd.length()+1, msg.length()-cmd.length() );

  if( data[data.length()-1] == '\r' )
    data = data.substr( 0, data.length()-1 );


  if( state == NOT_AUTHENTICATED &&
      cmd.compare( string( "REG" ) ) == 0 )
  {
    HandleREG( data );
  }

  return true;
}



bool PoolClient::HandleREG( string data )
{
  Pool *pool;

  if( data.length() == 40 )
  {
    if( (pool = server->FindPool( data )) )
    {
      state = AUTHENTICATED;
      server->MoveClientToPool( this, pool );
      evbuffer_add( output, "REGRESP:POOLED\n", 15 );
      return true;
    }
  }
  else if( data.length() == 80 )
  {
    if( (pool = server->FindPool( data.substr(0,40) )) )
    {
      if( pool->ownerHash.compare( data.substr(40,40) ) == 0 )
      {
        state = POOLOWNER;
        server->MoveClientToPool( this, pool );
        evbuffer_add( output, "REGRESP:POOLOWNER\n", 15 );
        return true;
      }
    }
  }

  evbuffer_add( output, "REGRESP:FAILED\n", 15 );
  return false;
}

