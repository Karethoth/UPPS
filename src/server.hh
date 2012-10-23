#ifndef __SERVER_HH__
#define __SERVER_HH__

#include <vector>
#include <event2/event.h>

#include "client.hh"


class Server
{
 private:
  //std::vector<Pool>   poolList;
  std::vector<Client*> clientList;

 public:
  void Run();
};

#endif

