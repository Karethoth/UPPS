#include "client.hh"

#include "sha1.hh"

using std::string;


Client::Client()
{
  CreateSalt();
  CreateIdentifier();

  std::cout << "New client with id '" << id << "'\n";

  state = NOT_AUTHENTICATED;
}



void Client::CreateSalt()
{
  char tmpBuf[256];
  sprintf( tmpBuf, "%d%d-%d\0", rand(), time( NULL ), rand() );
  string salt( tmpBuf );

  unsigned char hash[20];
  char hex[41];

  sha1::calc( salt.c_str(), salt.length(), hash );
  sha1::toHexString( hash, hex );

  this->salt = string( hex );
}



void Client::CreateIdentifier()
{
  char tmpBuf[100];
  sprintf( tmpBuf, "%d-%d\0", time( NULL )/2, rand() );
  string id( tmpBuf );

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
