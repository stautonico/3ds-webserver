#include "interface.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

Interface::Interface() {
  gfxInitDefault();
  gfxSetWide(true);

  atexit(gfxExit);

  consoleInit(GFX_TOP, NULL);
}

Interface::~Interface() {
  gfxExit();
}

void Interface::run() {
  printf("3ds-webserver demo\n");
  printf("Press \"Start\" to exit...\n");
  m_server.run();

  while (aptMainLoop()) {
    gspWaitForVBlank();
    gfxSwapBuffers();
    hidScanInput();

    // Your code goes here
    int kDown = hidKeysDown();
    if (kDown & KEY_START) {
      m_server.stop();
      break; // break in order to return to hbmenu
    }
  }
}