/*
The Plan
--------

Clean this up
Convert the other effects to GPU based format
Add music
Add text scroller

Sequence things over time!
*/

#include <string>
#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include <stdio.h>
#include "Tools.h"
#include <math.h>
#include "globals.h"
#include "effects/intro.h"
#include "effects/startunnel.h"
#include "effects/plasma.h"
#include "effects/fire.h"
#include "effects/gear.h"
#include "effects/cube.h"
#include "soundtrack.h"

int main() {
    romfsInit();

    int r = audio_init_preload();
    if (r != 0) return r;

    // Initialize services
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    // consoleInit(GFX_BOTTOM, NULL);

    initPlasma();
    initFire();
    introInit();
    gearInit();
    initTunnelScene();

    audio_init_mixer();

    // Create screen targets
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    C3D_Tex tex;
    if (!C3D_TexInit(&tex, TSIZE, TSIZE, GPU_RGBA5551)) {
        bool shit = false;
    }

    C3D_Tex offtex;
    C3D_TexInitVRAM(&offtex, 512, 512, GPU_RGBA8);
    C3D_RenderTarget* off = C3D_RenderTargetCreateFromTex(&offtex, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH24_STENCIL8);

    C3D_TexSetFilter(&offtex, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetFilter(&tex, GPU_NEAREST, GPU_NEAREST);

    // Create a texture
    C2D_SpriteSheet texSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");

    u16* mypix = (u16*) linearAlloc(TSIZE*TSIZE*sizeof(u16));
    if (mypix == NULL)
        abort();

    audio_init_play();

    uint32_t count = 0;
    // Main loop
    while (aptMainLoop()) {
        // Scan for user input
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break; // Exit when START is pressed

        static bool introDone = false;
        static bool tunnelDone = false;
        static bool plasmalDone = false;
        static bool gearDone = false;
        if (!introDone)
            introDone = introRender(top);
        else if (!tunnelDone)
            tunnelDone = renderTunnelScene(mypix, texSheet, offtex, off, tex, top);
        else if (!plasmalDone)
            plasmalDone = renderPlasmaScene(mypix, texSheet, offtex, off, tex, top);
        else if (!gearDone)
            gearDone = gearRender(top, off, offtex);
        if (gearDone)
            break;

        // printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
        // printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
        // printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

        count++;
    }

    C2D_SpriteSheetFree(texSheet);

    audio_deinit();

    exitFire();
    gearExit();
    linearFree(mypix);

    // Deinitialize services
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
