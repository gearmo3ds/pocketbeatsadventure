#include "startunnel.h"

#include <string>
#include "../globals.h"
#include <citro2d.h>
#include <citro3d.h>
#include <3ds.h>
#include <stdio.h>
#include "../Tools.h"
#include <math.h>
#include "plasma.h"
#include "fire.h"
#include "../internal.h"
#include"c2d/base.h"

//C2D_SpriteSheet texSheet;
C2D_SpriteSheet fontSheet;
static uint32_t sineScrollMotion = 0;
C2D_SpriteSheet logoSheet2;

void initTunnelScene()
{
    // Create a texture
    //texSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    fontSheet= C2D_SpriteSheetLoad("romfs:/gfx/font.t3x");
    logoSheet2 = C2D_SpriteSheetLoad("romfs:/gfx/logo.t3x");
}

void deinitTunnelScene()
{
    //C2D_SpriteSheetFree(texSheet);
    C2D_SpriteSheetFree(fontSheet);
    C2D_SpriteSheetFree(logoSheet2);
}

void putChar(uint16_t* buf, int16_t const& x,  int16_t const& y, char const& character)
{
    // if (x >= 400 || x < -16)
    //     return;

    static uint16_t cnt = 0;
    uint8_t index = character - 33;
    if (character == ' ')
        index = 0;
    else if (character == '!')
        index = 1;

    float rad;
    uint16_t i;
    short centery = SCR_HEIGHT >> 1;

    rad = (float)((x+sineScrollMotion) % 360) * 0.0174532;
    short value = centery + (short)((sin(rad) * 45.0));

    C2D_DrawImageAt(C2D_SpriteSheetGetImage(fontSheet, index), x, value, 0.5);
    // return;

    // uint16_t* ptr = (uint16_t*) &font.pixel_data[0];
    // for (int px = 0; px < 15; ++px) {
    //     for (int py = 0; py < 32; ++py) {
    //         uint16_t pixl = ptr[font.width*py + px + index * 16 ];
    //         if (pixl == 0)
    //             continue;
    //         // int16_t offset = sin(float(x+px)/400.0f * 10 + movin) * 32.0f;
    //         setPixel(buf, x + px, y + py + aSin[(x+px+sineScrollMotion) % 360], pixl);
    //         // setPixel(buf, x + px, y + py + aSin[xmotion % 400], pixl);
    //     }
    // }
}


void putText(uint16_t* buf, std::string const& text, int32_t pos)
{
    static const uint16_t textsize = 164;
    int16_t startIndex = 0;
    if (pos < 0)
        startIndex = (abs(pos) / 16);
    if (startIndex >= textsize)
        return;
    // printf("Index %i\n", startIndex);

    int16_t endIndex = startIndex + 28;
    startIndex = min(startIndex, textsize);
    endIndex = min(endIndex, textsize);
    static uint32_t cnt = 0;
    for (uint16_t i = startIndex; i < endIndex; ++i) {
        char c = text[i];
        // if (pos > 400 || pos < -16)
        //     continue;
        putChar(buf, pos + (startIndex * 16), 0, c);
        pos += 16;
        // if (pos > 400)
        //     return;
    }
}
bool renderTunnelScene(u16* mypix,
                       C2D_SpriteSheet texSheet,
                       C3D_Tex offtex,
                       C3D_RenderTarget* off,
                       C3D_Tex tex,
                       C3D_RenderTarget* top)
{
    static uint32_t frameCounter = 0;
    frameCounter++;
    static uint32_t last = 0;
    static uint32_t fps = 0;
    //        cpuCounter.text = std::to_string(ptnClock);
    u32 now = svcGetSystemTick() / (CPU_TICKS_PER_MSEC * 1000);
    if (now != last) {
        //            cpuCounter.text = "FPS: " + std::to_string(frameCounter % 64);
        fps = frameCounter % 64;
        frameCounter = 0;
    }
    last = now;

    uint16_t row = 0;
    float time = 0;
    float angle = 0;
    float a=3, b=4;
    static uint32_t count = 0;

    // printf("Frame: %i\n fps: %i", count, fps);
    row++;
    if (row>= 128)
        row = 0;

    // Bring in tunnel after some time
    static bool tunnelOnly = true;
    if (tunnelOnly && count >= 400)
        tunnelOnly = false;

    // drawPlasma(mypix);
    // if (!tunnelOnly)
        drawFire(mypix, false, true, true);
    drawTunnel(mypix, true);
    time += 0.01f * 0.4;
    angle += 0.003f;

    // Draw a custom colored pixel at (200, 120)
    // u32* buffer = (u32*)image.tex.d
    // setPixel(mypix, 200, 120, C2D_Color32(255, 255, 255, 255)); // Red pixel

    GSPGPU_FlushDataCache(mypix, TSIZE*TSIZE*2);
    GX_DisplayTransfer((u32*)mypix,
                       GX_BUFFER_DIM(TSIZE, TSIZE),
                       (u32*)tex.data,
                       GX_BUFFER_DIM(TSIZE, TSIZE),
                       TEXTURE_TRANSFER_FLAGS);
    gspWaitForPPF();

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(top);
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0x00000000, 0);

    // fullscreenQuadFlat(*image.tex);
    fullscreenQuad(tex, 0.0f, 1.0f);
    // fullscreenQuadFlat(tex); // only needed when rendering to off-screen buffer, it seems

    C2D_Prepare();
    C2D_SceneBegin(top);
    // C2D_DrawImageAt(C2D_SpriteSheetGetImage(texSheet, 1), 0, 0, 0.0f, NULL, 1.0f, 1.0f);

    if (count > 300)
        drawHeadSprites(texSheet,  count > 400);

    static int32_t scroll = 400;
    sineScrollMotion += 5;
    if (count > 200) {
        if (true) {
            scroll -= 2;
            scroll = scroll < -3900 ? 400 : scroll;
            putText(mypix, "Greets to halcy & co from SVatG for showing the way! "
                           "Will try more 3D next time! "
                           "Happy to be at Evoke 2024. "
                           "Shouts to fincs & Wintermute + all libctru contributors!"
                    ,
                    scroll);
        }
    }

    // C2D_DrawRectSolid(offsetx, offsety, 0, 20, 20, 0xff0000ff);
    C2D_Flush();

    // C3D_FrameDrawOn(top);
    // C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0x00000000, 0);

    // bool mirrorCenter = false;
    // if (mirrorCenter)
    //     fullscreenQuadMirror(offtex, 0, 1);
    // else
    //     fullscreenQuad(offtex, 0, 1);

    C3D_FrameEnd(0);
    count++;

    return count > 1700;
}

bool renderPlasmaScene(u16* mypix,
                       C2D_SpriteSheet texSheet,
                       C3D_Tex offtex,
                       C3D_RenderTarget* off,
                       C3D_Tex tex,
                       C3D_RenderTarget* top)
{
    uint16_t row = 0;
    static float time = 0;
    float angle = 0;
    float a=3, b=4;
    static uint32_t count = 0;

    // printf("Frame: %i\n", count);

    drawPlasma(mypix);

    time += 0.01f * 0.4;
    angle += 0.003f;

    // Draw a custom colored pixel at (200, 120)
    // u32* buffer = (u32*)image.tex.d
    // setPixel(mypix, 200, 120, C2D_Color32(255, 255, 255, 255)); // Red pixel

    GSPGPU_FlushDataCache(mypix, TSIZE*TSIZE*2);
    GX_DisplayTransfer((u32*)mypix,
                       GX_BUFFER_DIM(TSIZE, TSIZE),
                       (u32*)tex.data,
                       GX_BUFFER_DIM(TSIZE, TSIZE),
                       TEXTURE_TRANSFER_FLAGS);
    gspWaitForPPF();

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(off);
    C3D_RenderTargetClear(off, C3D_CLEAR_ALL, 0x00000000, 0);

    // fullscreenQuadFlat(*image.tex);
    // fullscreenQuad(tex, 0.0f, 1.0f);
    fullscreenQuadFlat(tex); // only needed when rendering to off-screen buffer, it seems

    C2D_Prepare();
    C2D_SceneBegin(off);
    // C2D_DrawImageAt(C2D_SpriteSheetGetImage(texSheet, 1), 0, 0, 0.0f, NULL, 1.0f, 1.0f);

    if (count < 300)
    drawHeadSprites(texSheet, true);
    if (count > 300)
        drawTileSquares(time);

    // C2D_DrawRectSolid(offsetx, offsety, 0, 20, 20, 0xff0000ff);
    C2D_Flush();

    C3D_FrameDrawOn(top);
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0x00000000, 0);

    bool mirrorCenter = false;
    if (mirrorCenter)
        fullscreenQuadMirror(offtex, 0, 1);
    else
        fullscreenQuad(offtex, 0, 1);

    C3D_FrameEnd(0);
    count++;

    bool done = count > 700;
    if (done)
        deinitTunnelScene();
    return done;
}

// Function to compose a 16-bit color in GPU_RGBA5551 format
inline uint16_t composeColorRGBA5551(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    // Ensure the input values are within the acceptable ranges
    red = red & 0x1F;    // 5 bits
    green = green & 0x1F;  // 5 bits
    blue = blue & 0x1F;   // 5 bits
    alpha = alpha & 0x01;  // 1 bit

    // Compose the 16-bit color value
    uint16_t color = (red << 11) | (green << 6) | (blue << 1) | alpha;

    return color;
}

void drawTunnel(u16* mypix, bool const& clear)
{

    static float time = 0;
    static float angle = 0;
    static float a=3, b=4;
    static float px, py;
    static int radiustime = 0;
    static int numCircles = 50;
    static int circleRes = 30;
    static int circleDist = 20;

    if (clear) {
        for (u32 i=0; i<(TSIZE*TSIZE); i++) {
            // mypix[i] = 0x543f;
            // mypix[i] = composeColorRGBA5551(i, 10, 10, 1);
            // mypix[i] = composeColorRGBA5551(0, 0, 0, 0);
            // mypix[i] = rand();
            mypix[i] = 0;
        }
    }
    // }

    // setPixel(mypix, 0, 0, composeColorRGBA5551(31, 31, 31, 1));
    // setPixel(mypix, 0, 239, composeColorRGBA5551(31, 31, 31, 1));
    // setPixel(mypix, 399, 239, composeColorRGBA5551(31, 31, 31, 1));
    // setPixel(mypix, 200, 120, composeColorRGBA5551(31, 0, 31, 1));


    if (false) {
        for (int i = 0; i < 10; ++i) {
            float timetmp = time - i * 0.15;
            px = 200 * sin(a * timetmp + angle) + 200;
            py = 120 * sin(b * timetmp) + 120;
            setPixel(mypix, px, py,  composeColorRGBA5551(31, 31, 31, 1));
            setPixel(mypix, px+1, py,  composeColorRGBA5551(31, 31, 31, 1));
            setPixel(mypix, px+1, py+1,  composeColorRGBA5551(31, 31, 31, 1));
            setPixel(mypix, px, py+1,  composeColorRGBA5551(31, 31, 31, 1));
        }
    }

    int radiusStep = 20;
    for (int cidx = 0; cidx < numCircles; ++cidx) {
        // if (cidx < 5)
        //     continue;
        float depth = pow(fmod(time*1.0 + float(cidx) /numCircles, 1.0f), 7.0);
        // printf("\x1b[2;1HCPU:     %f%%\x1b[K", depth);

        // float pup = pow(fmod(time - cidx * 0.1, 1.0), 3.0) * 3.14;
        float timetmp = time + cidx * 0.15;
        float offsetx = 30 * sin(a * timetmp + angle);
        float offsety = 30 * sin(b * timetmp);
        float tmpb = float(cidx) / float(numCircles);
        // tmpb = tmpb < 0.3 ? 0.3 : tmpb;
        float b = depth * 2.0;
        b = b > 1.0f ? 1.0f : b;
        uint8_t brightness = tmpb * 31;
        brightness = b * 31;
        // uint8_t brightness = float(cidx) / float(numCircles) * 31;

        // uint8_t brightness = (1.0 - float(cidx) / float(numCircles)) * 31;
        // offsetx = 0;
        // offsety = 0;
        // offsetx *= 0.6;
        // offsety *= 0.6;

        for (int i = 0; i < circleRes; ++i) {
            float angle = (1.0 / circleRes) * float(i) * 3.14 * 2.0 + time;
            // float radius = cidx * circleDist;// + (radiustime % radiusStep);
            // float radius = (fmod(time + cidx, 5.0)) * circleDist;
            float radius = pow(fmod(time + cidx * 0.1, 1.0), 3.0) * 200.0 + 50;
            radius = depth * 200;
            float ipx = sin(angle) * radius + 200;
            float ipy = cos(angle) * radius + 120;

            setThickPixel(mypix, ipx+offsetx, ipy+offsety, composeColorRGBA5551(brightness, brightness, brightness, 1));
            // setPixel(mypix, ipx+offsetx+1, ipy+offsety+0, composeColorRGBA5551(brightness, brightness, brightness, 1));
            // setPixel(mypix, ipx+offsetx+0, ipy+offsety+1, composeColorRGBA5551(brightness, brightness, brightness, 1));
            // setPixel(mypix, ipx+offsetx+1, ipy+offsety+1, composeColorRGBA5551(brightness, brightness, brightness, 1));
        }
    }

    time += 0.01f * 0.4;
    angle += 0.003f;
    radiustime+=1;
}

void drawHeadSprites(C2D_SpriteSheet texSheet, bool const& show)
{
    static float time = 0;
    static float angle = 0;
    float a=3, b=4;

    time += 0.01f * 0.4;
    angle += 0.003f;
    for (int i = 0; i < 6; ++i) {
        float offsetx = 150 * sin(a * time*3 + float(i)*0.4f + angle) + 200;
        float offsety = 80 * sin(b * time*3 + float(i)*0.4f) + 120;
        float size = sin(2.0f * PI * 0.3 * time + (time*9));
        size = (size + 1.0f) / 2.0f;
        size = 1.0f - size;
        size = 0.2f + size * 0.3f;
        // double sineValue = sin(2 * PI * frequency * t);
        if (show)
            C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(logoSheet2, 0), offsetx, offsety, 0, angle*3, nullptr, size, size);
        // printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", size);

    }
}

void drawTileSquares(float time)
{
    //  0,  1,  2,  3,  4,  5,  6,  7,
    //  8,  9, 10, 11, 12, 13, 14, 15,
    // 16, 17, 18, 19, 20, 21, 22, 23,
    // 24, 25, 26, 27, 28, 29, 30, 31,
    // 32, 33, 34, 35, 36, 37, 38, 39

    static uint8_t hori[] = { 0,  1,  2,  3,  4,  5,  6,  7,
                             15, 23, 31, 39, 38, 37, 36, 35,
                             34, 33, 32, 24, 16,  8,  9, 10,
                             11, 12, 13, 14, 22, 30, 29, 28,
                             27, 26, 25, 17, 18, 19, 20, 21,
                             };

    for (int i = 0; i < 40; ++i) {
        uint8_t id = hori[i];
        float idx = id % 8;
        uint8_t idy = id / 8;
        float qscale = (sin(time*10.0f - float(i) * 0.1f) + 1.0f) / 2.0f * 48.0f;
        // crect(24+idx*48, 24+idy*48, fmod(time, 1.0f)* 48.0f, fmod(time, 1.0f)*48.0f, 0xff0088ff);
        uint32_t alpha = uint32_t((sin(time) + 1.0f) / 2.0f * 255) << 24;
        // uint32_t color = 0x000080ff + alpha;
        uint32_t color = 0x00000000 + uint32_t(255 << 24);
        crect(24+idx*48, 24+idy*48, qscale, qscale, color);
    }
    // crect(200, 120, 100, 100, 0xff0088ff);
    // crect(rot, 100, 50, 50, 0xffffffff);
}

void drawThings()
{

}
