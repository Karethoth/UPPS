#include "server.hh"
#include "pool.hh"

#include <ctime>


int main( int c, char *v[] )
{
  srand( time( NULL ) );
  setvbuf( stdout, NULL, _IONBF, 0 );

  Server server;
  server.Run();

  return 0;
}

