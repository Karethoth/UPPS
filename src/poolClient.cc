#include "poolClient.hh"
#include "sha1.hh"

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <sstream>

using std::string;
using std::vector;


PoolClient::PoolClient( Server *srv, struct bufferevent *bev ) : Client()
{
  server = srv;
  pool   = NULL;

  input  =  bufferevent_get_input( bev );
  output =  bufferevent_get_output( bev );

  bufferevent_setwatermark( bev, EV_READ, 0, 16000 );
  bufferevent_enable( bev, EV_READ|EV_WRITE );

  string ridMsg( "RSALT:" );
  ridMsg.append( salt );
  ridMsg.append( "\n" );

  evbuffer_add( output, ridMsg.c_str(), ridMsg.length() );
}



void PoolClient::SetPool( Pool *p )
{
  pool = p;
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
  else if( state == NOT_AUTHENTICATED &&
           cmd.compare( string( "NEWPOOL" ) ) == 0 )
  {
    HandleNEWPOOL();
  }
  else if( state == POOL_OWNER &&
           cmd.compare( string( "LIST" ) ) == 0 )
  {
    HandleLIST();
  }
  else
  {
    evbuffer_add( output, "ERROR:NOT_A_COMMAND\n", 20 );
  }

  return true;
}



bool PoolClient::SendPackage( struct sPackage *pkg )
{
  std::ostringstream pkgStream;

  pkgStream.write( (char*)&pkg->requestID, 4 );
  pkgStream.write( (char*)&pkg->ip, 4 );
  pkgStream.write( (char*)&pkg->port, 2 );
  pkgStream.write( (char*)&pkg->dataLen, 2 );

  vector<char>::iterator it;
  for( it = pkg->data->begin(); it != pkg->data->end(); ++it )
  {
    pkgStream.put( *it );
  }
  pkgStream.put( '\n' );

  string pkgString( pkgStream.str() );

  evbuffer_add( output, "PKG:\n", 5 );
  evbuffer_add( output, pkgString.c_str(), pkgString.length() );
}



bool PoolClient::HandleREG( string data )
{
  Pool *pool;

  if( data.length() == 40 )
  {
    if( (pool = server->FindSaltedPool( this, data )) )
    {
      state = AUTHENTICATED;
      server->MoveClientToPool( this, pool );
      SetPool( pool );

      evbuffer_add( output, "REGRESP:POOLED\n", 15 );

      vector<char> *data = new vector<char>();
      data->push_back( 'A' );
      data->push_back( 'A' );
      data->push_back( 'A' );
      data->push_back( 'A' );
      data->push_back( '\0' );
      struct sPackage pkg;
      pkg.requestID = 0x00000001;
      pkg.ip        = 0x7F000001;
      pkg.port      = 0x0050;
      pkg.dataLen   = 0x0005;
      pkg.data      = data;
      SendPackage( &pkg );

      return true;
    }
  }
  else if( data.length() == 80 )
  {
    if( (pool = server->FindSaltedPool( this, data.substr(0,40) )) )
    {
      if( pool->ownerHash.compare( data.substr(40,40) ) == 0 )
      {
        state = POOL_OWNER;
        server->MoveClientToPool( this, pool );
        SetPool( pool );
        evbuffer_add( output, "REGRESP:POOLOWNER\n", 18 );
        return true;
      }
    }
  }

  evbuffer_add( output, "REGRESP:FAILED\n", 15 );
  return false;
}



bool PoolClient::HandleLIST()
{
  vector<Client*> *clients = pool->GetClientList();
  string msg( "LISTRESP:" );

  vector<Client*>::iterator cit;
  for( cit = clients->begin(); cit != clients->end(); ++cit )
  {
    if( (*cit)->state == POOL_OWNER )
      continue;
    
    msg.append( (*cit)->id );
    msg.append( ";" );
  }
  msg.append( "\n" );

  evbuffer_add( output, msg.c_str(), msg.length() );
}



bool PoolClient::HandleNEWPOOL()
{
  Pool *newPool = new Pool();
  server->AddPool( newPool );

  string msg( "POOLINFO:" );
  msg.append( newPool->id );
  msg.append( ";" );
  msg.append( newPool->ownerKey );
  msg.append( "\n" );

  evbuffer_add( output, msg.c_str(), msg.length() );

  unsigned char poolHash[20];
  char poolHex[41];

  string saltedPool( newPool->id );
  saltedPool.append( salt );

  sha1::calc( saltedPool.c_str(), saltedPool.length(), poolHash );
  sha1::toHexString( poolHash, poolHex );

  string data( poolHex );
  data.append( newPool->ownerHash );

  HandleREG( data );
}

