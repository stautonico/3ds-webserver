#include "interface.h"

#include <stdio.h>

Interface::Interface() {
  printf("This is the linux interface starting...\n");
}

Interface::~Interface() {
  printf("Goodbye linux interface!\n");
}

void Interface::run() {
  printf("This is the linux interface running...\n");
}