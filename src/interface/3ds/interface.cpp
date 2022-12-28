#include "interface.h"

#include <3ds.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

Interface::Interface() {
  gfxInitDefault();
  gfxSetWide(true);

  atexit(gfxExit);

  consoleInit(GFX_TOP, NULL);

  bool fail = m_server.failure();

  if (fail) {
    printf("Failed setting up web server...\n");
    printf("Error: %s\n", m_server.error().c_str());
    return;
  } else {
    printf("Successfully setup web server...\n");
  }
}

Interface::~Interface() { gfxExit(); }

void Interface::run() {
  printf("3ds-webserver demo\n");
  printf("Press \"A\" to clear the console\n");
  printf("Press \"Start\" to exit...\n");

  while (aptMainLoop()) {
    gspWaitForVBlank();
    gfxSwapBuffers();
    hidScanInput();

    m_server.tick();

    int kDown = hidKeysDown();
    if (kDown & KEY_START) {
      m_server.stop();
      break; // break in order to return to hbmenu
    }

    if (kDown & KEY_A) {
      consoleClear();
    }
  }
}