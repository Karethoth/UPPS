#ifndef __POOL_HH__
#define __POOL_HH__


#include <iostream>
#include <vector>
#include "client.hh"


class Pool
{
 private:
  std::vector<Client*> clientList;

 protected:
  void CreateSalt();
  void CreateIdentifier();
  void CreateOwnerKey();

 public:
  std::string id;
  std::string idHash;
  std::string salt;
  std::string ownerKey;
  std::string ownerHash;

  Pool();

  bool AddClient( Client* );
  std::vector<Client*>* GetClientList();
};

#endif

