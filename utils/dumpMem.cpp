#include <usrttype.h>
#include <MapMem.h>

int main( int argc, char *argv[] )
{
  attach(argv[1]);
  dumpHead(argv[1]);
  printStack(argv[1]);
  return 0;  
}