#include "server.hh"
#include "pool.hh"

#include <ctime>


int main( int c, char *v[] )
{
  srand( time( NULL ) );
  setvbuf( stdout, NULL, _IONBF, 0 );

  Pool *p = new Pool();

  Server server;
  server.AddPool( p );
  server.Run();

  return 0;
}

