#ifndef __POOLCLIENT_HH__
#define __POOLCLIENT_HH__

#include "client.hh"
#include "server.hh"


class PoolClient : public Client
{
 private:
  Server *server;
  Pool   *pool;

 protected:
  bool HandleREG( std::string );
  bool HandleLIST();
  bool HandleNEWPOOL();

 public:
  struct evbuffer *input, *output;

  PoolClient( Server*, struct bufferevent* );
  void SetPool( Pool* );

  bool HandleMessage( std::string );
};

#endif

