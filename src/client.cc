#include "client.hh"

#include "sha1.hh"
#include <cstdlib>
#include <sstream>

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
  std::ostringstream saltStream;
  saltStream << rand() << time( NULL ) << "-" << rand();

  string rawSalt( saltStream.str() );

  unsigned char hash[20];
  char hex[41];

  sha1::calc( rawSalt.c_str(), rawSalt.length(), hash );
  sha1::toHexString( hash, hex );

  this->salt = string( hex );
}



void Client::CreateIdentifier()
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

