#ifndef __POOLCLIENT_HH__
#define __POOLCLIENT_HH__

#include "client.hh"
#include "server.hh"


class PoolClient : public Client
{
 private:
  Server *server;

 protected:
  bool HandleREG( std::string );

 public:
  struct evbuffer *input, *output;

  PoolClient( Server*, struct bufferevent* );

  bool HandleMessage( std::string );
};

#endif

