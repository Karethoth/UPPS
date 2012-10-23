#ifndef __POOLUSER_HH__
#define __POOLUSER_HH__

#include "client.hh"
#include "server.hh"


class PoolUser : public Client
{
 private:
  Server *server;

};

#endif

