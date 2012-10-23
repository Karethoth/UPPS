#include "server.hh"


int main( int c, char *v[] )
{
  setvbuf( stdout, NULL, _IONBF, 0 );

  Server server;
  server.Run();

  return 0;
}

