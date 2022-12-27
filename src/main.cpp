#include "server/server.h"
#include <stdio.h>

#if IS_3DS
#include "interface/3ds/interface.h"
#else
#include "interface/linux/interface.h"
#endif

/*
TODO: Restructure the interface system to use #if conditional rendering instead
of different classes
TODO: Serve files from "./3ds-webserver" instead of "/tmp/webfiles"
TODO: Config file to change options?
 */

int main() {
  auto *i = new Interface();

  i->run();

  delete i;

#if IS_3DS
//  printf("We're about to enter the really long loop...\n");
//  for (long long i = 0; i < 5000000000LL; i++) {};
//  printf("We're done with the really long loop. Goodbye!\n");
#endif

  return 0;
}