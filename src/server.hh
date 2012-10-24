#ifndef __SERVER_HH__
#define __SERVER_HH__

#include <vector>
#include <event2/event.h>

#include "client.hh"
#include "pool.hh"


struct sPackage
{
  std::string    poolClientID;
  unsigned int   requestID;
  unsigned int   ip;
  unsigned short port;
  unsigned short dataLen;
  unsigned char *data;
};



class Server
{
 private:
  std::vector<Pool*>   poolList;
  std::vector<Client*> clientList;

 public:
  void Run();

  bool AddClient( Client* );
  bool AddPool( Pool* );
  bool MoveClientToPool( Client*, Pool* );

  Pool* FindPool( std::string );
};

#endif

