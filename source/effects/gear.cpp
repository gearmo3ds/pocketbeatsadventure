#include "gear.h"
#include <citro2d.h>
#include "../globals.h"
#include <math.h>
#include "../Tools.h"

// static const struct sync_track* sbgalpha;
// static const struct sync_track* fuck;

// static C3D_Tex logoTex;
C2D_SpriteSheet gearSheet;
C2D_SpriteSheet logoSheet5;
C2D_SpriteSheet animSheet2;
C2D_TextBuf g_staticBuf;
C2D_Text g_staticText[8];

struct sthing2 {
    float x, y;
    uint8_t spriteIdx;
    float speed = 1;
};

const static uint8_t numThings2 = 50;
sthing2 things2[numThings2];

float randomFloat2(float min, float max) {
    // Generate a random float between 0.0 and 1.0
    float scale = rand() / (float)RAND_MAX;
    // Scale it to the range [min, max]
    return min + scale * (max - min);
}


struct sthing3 {
    C2D_DrawParams params;
    float scale;
    float offset;
    float alpha;
};


const static uint8_t numLogos = 50;
sthing3 logothings[numLogos];


void drawLogo(C2D_Image img, float posx, float posy, float posz,
                float scaleX=1.0f, float scaleY=1.0,
                float alpha=1.0f,
                float const& angle=0,
                float const& width=128, float const& height=128)
{
    // C2D_DrawImageAtRotated(img, posx, posy, posz, 0);
    C2D_DrawParams params;
    params.center.x = (width/2) * scaleX;
    params.center.y = (height/2) * scaleY;
    params.pos.x = posx;
    params.pos.y = posy;
    params.pos.w = width * scaleX;
    params.pos.h = height * scaleY;
    params.depth = posz;
    params.angle = angle;

    C2D_ImageTint tint;
    for (int i = 0; i < 4; ++i) {
        // tint.corners[i].color = 0x80000000;
        tint.corners[i].color = u32(alpha * 255) << 24;
        tint.corners[i].blend = 0;
    }
    C2D_DrawImage(img, &params, &tint);
}

void gearInit()
{
    gearSheet = C2D_SpriteSheetLoad("romfs:/gfx/gear.t3x");
    logoSheet5 = C2D_SpriteSheetLoad("romfs:/gfx/logo.t3x");
    animSheet2 = C2D_SpriteSheetLoad("romfs:/gfx/ncanim.t3x");

    srand(6575);
    for (int i = 0; i < numThings2; ++i) {
        // C2D_DrawParams& params = things2[i].params;
        things2[i].x = randomFloat2(-200, 0);
        things2[i].y = randomFloat2(-240, 120);
        if (i < 10) {
            things2[i].x = randomFloat2(0, 200);
            things2[i].y = randomFloat2(0, 120);
        }
        things2[i].spriteIdx = rand() % 9;
        things2[i].speed = randomFloat2(0.1f, 2.0f);
    }


    // logos
    srand(12345);
    for (int i = 0; i < numLogos; ++i) {
        C2D_DrawParams& params = logothings[i].params;
        params.pos.x = randomFloat2(0, 399);
        params.pos.y = randomFloat2(0, 239);
        params.center = {64, 64};
        params.depth = 0.5;
        params.angle = 0;
        logothings[i].offset = randomFloat2(0, 100);
        logothings[i].scale = randomFloat2(0.2f, 0.7f);
        logothings[i].alpha = logothings[i].scale;
    }

    sthing3& last = logothings[numLogos-1];
    last.alpha = 1;
    last.scale = 1;
    last.params.pos.x = 200;
    last.params.pos.y = 120 ;

    g_staticBuf  = C2D_TextBufNew(4096);
    C2D_TextParse(&g_staticText[0], g_staticBuf, "STR4NG3R D4NG3R");
    C2D_TextParse(&g_staticText[1], g_staticBuf, "Dave the word");
    C2D_TextParse(&g_staticText[2], g_staticBuf, "Zero the Prototype");
    C2D_TextParse(&g_staticText[3], g_staticBuf, "Capsule.Ghost");
    C2D_TextParse(&g_staticText[4], g_staticBuf, "Press Start");
    C2D_TextParse(&g_staticText[5], g_staticBuf, "patreon.com/NoiseCommander3DS");
    C2D_TextParse(&g_staticText[6], g_staticBuf, "youtube.com/@NoiseCommander3DS");
    C2D_TextParse(&g_staticText[7], g_staticBuf, "By Gearmo 3DS 2024");

    C2D_TextOptimize(&g_staticText[0]);
    C2D_TextOptimize(&g_staticText[1]);
    C2D_TextOptimize(&g_staticText[2]);
    C2D_TextOptimize(&g_staticText[3]);
    C2D_TextOptimize(&g_staticText[4]);
    C2D_TextOptimize(&g_staticText[5]);
    C2D_TextOptimize(&g_staticText[6]);
    C2D_TextOptimize(&g_staticText[7]);
}

void gearExit()
{
    C2D_SpriteSheetFree(gearSheet);
    C2D_SpriteSheetFree(logoSheet5);
    C2D_SpriteSheetFree(animSheet2);
    C2D_TextBufDelete(g_staticBuf);
}

void drawThing3(C2D_Image img, float posx, float posy, float posz,
                float scaleX=1.0f, float scaleY=1.0,
                float alpha=1.0f,
                float const& angle=0,
                float const& width=128, float const& height=128)
{
    // C2D_DrawImageAtRotated(img, posx, posy, posz, 0);
    C2D_DrawParams params;
    params.center.x = (width/2) * scaleX;
    params.center.y = (height/2) * scaleY;
    params.pos.x = posx;
    params.pos.y = posy;
    params.pos.w = width * scaleX;
    params.pos.h = height * scaleY;
    params.depth = posz;
    params.angle = angle;

    C2D_ImageTint tint;
    for (int i = 0; i < 4; ++i) {
        // tint.corners[i].color = 0x80000000;
        tint.corners[i].color = u32(alpha * 255) << 24;
        tint.corners[i].blend = 0;
    }
    C2D_DrawImage(img, &params, &tint);
}


bool gearRender(C3D_RenderTarget *top, C3D_RenderTarget *off, C3D_Tex offtex)
{
    static uint32_t cnt = 0;
    // printf("Frame %i\n", cnt);

    static float time = 0;
    time += 0.05f;
    static uint8_t numActive = 1;
    static float angleOffset = 0.0;
    if (cnt > 1000)
        angleOffset -= 0.002f;
    static bool grow = true;
    static bool dir = true;
    static uint8_t dirCase = 0;

    // printf("Frame %i sprites: %i\n", cnt, numActive);
    cnt++;

    // 1:4 chance of switching direction
    if (cnt % 300 == 0) {
        dirCase = (dirCase+1) % 4;
        dir = dirCase != 3;
    }

    if (cnt % 30 == 0) {
        if (grow)
            numActive = min(numActive+1, numThings2-1);
        else
            numActive = max(numActive-1, 1);

        if (numActive >= numThings2-1)
            grow = !grow;
        else if (numActive <= 1)
            grow = !grow;

    }

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(off);
    C3D_RenderTargetClear(off, C3D_CLEAR_ALL, 0x00000000, 0);

    C2D_Prepare();
    C2D_SceneBegin(off);

    for (int i = 0; i < numActive; ++i) {
        sthing2& thing = things2[i];
        // drawThing3(C2D_SpriteSheetGetImage(gearSheet, 0), 200, 120, 0, bgzoom, bgzoom, bgalpha,  bgRot, 400,  240);
        C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(gearSheet, thing.spriteIdx), thing.x, thing.y, 0, M_PI_4 + angleOffset, NULL, 1.0f, 1.0f);
        thing.x += thing.speed;
        if (cnt < 800) {
            if (dir)
                thing.y += 1;
            else
                thing.y -= 1;
        }
        else {
            if (dir)
                thing.y += thing.speed;
            else
                thing.y -= thing.speed;
        }

        if (thing.x > 400 && thing.y > 240) {
            thing.x = randomFloat2(-100, 200);
            thing.y = randomFloat2(-120, 240);
        }
    }



    C2D_Flush();

    C3D_FrameDrawOn(top);
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0x00000000, 0);

    bool mirrorCenter = true;
    if (mirrorCenter)
        fullscreenQuadMirror(offtex, 0, 1);
    else
        fullscreenQuad(offtex, 0, 1);

    // ------- outro
    C2D_Prepare();
    C2D_SceneBegin(top);
    static float bgzoom = 2;
    static float bgPosY = - 75;
    // static float bgRot = 0.5f;
    // bgzoom -= 0.0005f;
    static uint8_t bgFrame = 0;
    static float bgalpha = 0;
    if (cnt > 1500) {
        bgzoom = bgzoom <= 1 ? 1 : bgzoom - 0.0010f;
        bgPosY = bgPosY >= 0 ? 0 : bgPosY+0.005f;
    }
    if (cnt > 1800) {
        bgalpha = min(bgalpha+0.005f,  1.0f);
        // bgRot = bgRot <= 0 ? 0 : bgRot-0.0005f;
    }

    if (cnt % 15 == 0)
        bgFrame = bgFrame == 3 ? bgFrame = 0 : bgFrame+1;

    const uint16_t endloop = 3600;

    // 3DS Console
    if (cnt < endloop)
        drawThing3(C2D_SpriteSheetGetImage(animSheet2, bgFrame), 200, 120, 0, bgzoom, bgzoom, bgalpha,  0, 400,  240);

    static float circlePhase = 0;
    circlePhase += 0.03f;
    static float thingsAlpha = 1.0f;
    if (cnt > 1800)
        thingsAlpha = max(thingsAlpha-0.005f, 0);
    static uint8_t numShownBubbles = 0;
    if (cnt > 1000 && cnt < endloop){
        if (cnt % 40 == 0)
            numShownBubbles = min(numShownBubbles+1, numLogos-1);

        for (int i = 0; i < numShownBubbles; ++i)
        {
            sthing3& thing = logothings[i];
            float scaleX = sin(time + thing.offset) * 0.1f + thing.scale;
            float scaleY = sin(time + thing.offset + M_PI + 0.4f) * 0.1f + thing.scale;
            float posX = sin(circlePhase + thing.offset) * 10 + thing.params.pos.x;
            float posY = cos(circlePhase + thing.offset) * 10 + thing.params.pos.y;
            drawLogo(C2D_SpriteSheetGetImage(logoSheet5, 0), posX, posY, 0, scaleX, scaleY, thingsAlpha * thing.alpha);
        }
        // {
        //     sthing3& thing = logothings[numLogos-1];
        //     float scaleX = sin(time + thing.offset) * 0.1f + thing.scale;
        //     float scaleY = sin(time + thing.offset + M_PI + 0.4f) * 0.1f + thing.scale;
        //     float posX = sin(circlePhase + thing.offset) * 10 + thing.params.pos.x;
        //     float posY = cos(circlePhase + thing.offset) * 10 + thing.params.pos.y;
        //     drawLogo(C2D_SpriteSheetGetImage(logoSheet5, 0), posX, posY, thing.params.depth, scaleX, scaleY, thingsAlpha);
        // }
    }

    const float textSize = 0.7;
    const u32 textFlags = C2D_AtBaseline | C2D_WithColor | C2D_AlignCenter;
    static float textRot = 0;
    static float txtAlpha = 0;
    if (cnt > 2000 && cnt < 2600)
        txtAlpha = min(txtAlpha+0.01f,  1.0f);
    else
        txtAlpha = max(txtAlpha-0.01f,  0.0f);

    if (cnt > 2000) {
        textRot += 0.02f;
        // static float txtscl = 0;
        // txtscl += 0.05f;
        for (uint8_t i = 0; i < 4; ++i) {
            float txtx = sin(textRot + M_PI_2 * i) * 100 + 200;
            float txty = cos(textRot + M_PI_2 * i) * 100 + 120;
            // float scale = sin(txtscl) * textSize;
            C2D_DrawText(&g_staticText[i], textFlags, txtx, txty, 1, textSize, textSize, C2D_Color32f(1, 1, 1, txtAlpha));
        }
    }

    static bool blink = false;
    if (cnt % 30 == 0)
        blink = !blink;

    if (cnt > 2800 && blink && cnt < endloop) {
        C2D_DrawText(&g_staticText[4], textFlags, 200, 120, 1, 1.5, 1.5, C2D_Color32f(1, 1, 1, 1));
    }
    if (cnt > 2800){
        C2D_DrawText(&g_staticText[5], textFlags, 200, 225, 1, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));
        C2D_DrawText(&g_staticText[6], textFlags, 200, 240, 1, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));
        C2D_DrawText(&g_staticText[7], textFlags, 200, 20, 1, 0.5f, 0.5f, C2D_Color32f(1, 1, 1, 1));
    }

    // if (cnt > )
    C3D_FrameEnd(0);
    // printf("\x1b[2;1HFrame:     %i%%\x1b[K", cnt);

    return false;
    // return cnt > 3500;
}
