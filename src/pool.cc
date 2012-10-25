#include "pool.hh"
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "sha1.hh"

using std::string;
using std::vector;


Pool::Pool()
{
  CreateSalt();
  CreateIdentifier();
  CreateOwnerKey();

  std::cout << "Created new pool:\n";
  std::cout << "\tID:        " << id << "\n";
  std::cout << "\tIDHash:    " << idHash << "\n";
  std::cout << "\tSalt:      " << salt << "\n";
  std::cout << "\tOwnerKey:  " << ownerKey << "\n";
  std::cout << "\tOwnerHash: " << ownerHash << "\n";
}



void Pool::CreateSalt()
{
  std::ostringstream saltStream;
  saltStream << rand() << time( NULL ) << "-" << rand();

  string rawSalt( saltStream.str() );

  unsigned char hash[20];
  char hex[41];

  sha1::calc( rawSalt.c_str(), rawSalt.length(), hash );
  sha1::toHexString( hash, hex );

  this->salt = string( hex );
}



void Pool::CreateIdentifier()
{
  std::ostringstream idStream;
  idStream << rand() << "-" << time( NULL )/2 << rand();

  string id( idStream.str() );

  unsigned char hash[20];
  char hex[41];

  sha1::calc( id.c_str(), id.length(), hash );
  sha1::toHexString( hash, hex );

  this->id = string( hex );
  string tmp = this->id+salt;

  sha1::calc( tmp.c_str(), tmp.length(), hash );
  sha1::toHexString( hash, hex );

  idHash = string( hex );
}



void Pool::CreateOwnerKey()
{
  std::ostringstream keyStream;
  keyStream << rand() << time( NULL )/3 << rand();
  string key( keyStream.str() );

  unsigned char hash[20];
  char hex[41];

  sha1::calc( key.c_str(), key.length(), hash );
  sha1::toHexString( hash, hex );

  ownerKey = string( hex );
  string tmp = ownerKey+salt;

  sha1::calc( tmp.c_str(), tmp.length(), hash );
  sha1::toHexString( hash, hex );

  ownerHash = string( hex );
}



bool Pool::AddClient( Client *client )
{
  clientList.push_back( client );
  return true;
}



vector<Client*>* Pool::GetClientList()
{
  return &clientList;
}

