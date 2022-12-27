#include "interface.h"

#include <stdio.h>

Interface::Interface() {
  printf("This is the linux interface starting...\n");

  bool fail = m_server.failure();

  if (fail) {
    printf("Failed setting up web server...\n");
    printf("Error: %s\n", m_server.error().c_str());
    return;
  } else {
    printf("Successfully setup web server...\n");
  }
}

Interface::~Interface() {
  printf("Goodbye linux interface!\n");
}

void Interface::run() {

  printf("This is the linux interface running...\n");
  while (true) {
    m_server.tick();
  }
}