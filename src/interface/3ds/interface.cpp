#include "interface.h"

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>

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

  while (aptMainLoop()) {
    gspWaitForVBlank();
    gfxSwapBuffers();
    hidScanInput();

    // Your code goes here
    int kDown = hidKeysDown();
    if (kDown & KEY_START)
      break; // break in order to return to hbmenu
  }
}