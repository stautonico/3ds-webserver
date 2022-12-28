#include "./util/string.h"
#include "server/server.h"
#include <unistd.h>

#if IS_3DS
#include "interface/3ds/interface.h"
#else
#include "interface/linux/interface.h"
#endif

/*
TODO: Config file to change options?
 */

int main(int argc, char **argv) {
  auto current_executable_path = split(argv[0], '/');
  current_executable_path.erase(current_executable_path.end() - 1);

  std::string current_path;

  for (auto &path : current_executable_path)
    current_path += path + "/";

  chdir(current_path.c_str());

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