#ifndef __CLIENT_HH__
#define __CLIENT_HH__

#include <iostream>

enum eClientState
{
  ERROR,
  NOT_AUTHENTICATED,
  AUTHENTICATED,
  POOL_OWNER
};



class Client
{
 protected:
  void CreateSalt();
  void CreateIdentifier();

 public:
  eClientState state;

  std::string id;
  std::string idHash;
  std::string salt;

  Client();
};


#endif

