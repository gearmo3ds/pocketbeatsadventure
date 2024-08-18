#include "intro.h"
#include <citro2d.h>
#include "../globals.h"
#include <math.h>
#include "../Tools.h"

// static const struct sync_track* sbgalpha;
// static const struct sync_track* fuck;

static C3D_Tex logoTex;
C2D_SpriteSheet logoSheet;
C2D_SpriteSheet animSheet;
C2D_SpriteSheet wickSheet;

struct sthing {
    C2D_DrawParams params;
    float scale;
    float offset;
    float alpha;
};

const static uint8_t numThings = 50;
sthing things[numThings];
// = {
//     {10, 40, 0.5f, 4.3f},
//     {300, 140, 0.5f, 7.1f},
//     {50, 190, 0.4f, 3.7f},
//     {270, 30, 0.3f, 3.7f},
//     {80, 90, 0.2f, 3.7f},
//     {218, 215, 0.4f, 45.32f},
//     {375, 230, 0.3f, 15.82f},
//     {340, 50, 0.2f, 29.12f},
//     {113, 39, 0.32f, 5.52f},

//     {200, 120, 1, 0},
// };

float randomFloat(float min, float max) {
    // Generate a random float between 0.0 and 1.0
    float scale = rand() / (float)RAND_MAX;
    // Scale it to the range [min, max]
    return min + scale * (max - min);
}

void introInit()
{
    logoSheet = C2D_SpriteSheetLoad("romfs:/gfx/logo.t3x");
    if (logoSheet == NULL)
        printf("Why, Citra?!\n");
    animSheet = C2D_SpriteSheetLoad("romfs:/gfx/ncanim.t3x");
    wickSheet = C2D_SpriteSheetLoad("romfs:/gfx/wickbert.t3x");


    srand(12345);
    for (int i = 0; i < numThings; ++i) {
        C2D_DrawParams& params = things[i].params;
        params.pos.x = randomFloat(0, 399);
        params.pos.y = randomFloat(0, 239);
        params.center = {64, 64};
        params.depth = 0.5;
        params.angle = 0;
        things[i].offset = randomFloat(0, 100);
        things[i].scale = randomFloat(0.2f, 0.7f);
        things[i].alpha = things[i].scale;
    }

    sthing& last = things[numThings-1];
    last.alpha = 1;
    last.scale = 1;
    last.params.pos.x = 200;
    last.params.pos.y = 120;
}

void introExit()
{
    C2D_SpriteSheetFree(logoSheet);
    C2D_SpriteSheetFree(animSheet);
    C2D_SpriteSheetFree(wickSheet);
}

void drawThing(C3D_Tex texture, float posx, float posy, float scaleX=1.0f, float scaleY=1.0f)
{
    ensureFlatShader();

    C3D_TexSetFilter(&texture, GPU_NEAREST, GPU_NEAREST);
    C3D_TexSetWrap(&texture, GPU_REPEAT, GPU_REPEAT);
    C3D_TexBind(0, &texture);

    resetShadeEnv();

    // shift = shift / 400.0;
    float textureLeft = 0;
    float textureRight = 1;
    float textureTop = 0;
    float textureBottom = 1;

    float width = 128 * scaleX;
    float height = 128 * scaleY;
    posx -= width / 2;
    posy -= height / 2;

    // Turn off depth test as well as write
    // C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_COLOR);

    // Draw a textured quad directly
    C3D_ImmDrawBegin(GPU_TRIANGLES);

    // float ratio = 400.0f / 240.0f;
    C3D_ImmSendAttrib(posx, posy, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureLeft, textureBottom, 0.0f, 0.0f);

    C3D_ImmSendAttrib(posx+width, posy+height, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureRight, textureTop, 0.0f, 0.0f);

    C3D_ImmSendAttrib(posx+width, posy, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureRight, textureBottom, 0.0f, 0.0f);

    C3D_ImmSendAttrib(posx, posy, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureLeft, textureBottom, 0.0f, 0.0f);

    C3D_ImmSendAttrib(posx, posy+height, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureLeft, textureTop, 0.0f, 0.0f);

    C3D_ImmSendAttrib(posx+width, posy+height, 0.5f, 0.0f);
    C3D_ImmSendAttrib(textureRight, textureTop, 0.0f, 0.0f);

    C3D_ImmDrawEnd();

}

void drawThing2(C2D_Image img, float posx, float posy, float posz,
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


bool introRender(C3D_RenderTarget *top)
{
    static uint32_t cnt = 0;

    static float time = 0;
    time += 0.05f;
    static float circlePhase = 0;
    circlePhase += 0.03f;
    static float bgzoom = 2;
    static float bgPosY = - 75;
    static float bgRot = 0.5f;
    // bgzoom -= 0.0005f;
    bgzoom = bgzoom <= 1 ? 1 : bgzoom - 0.0010f;
    bgPosY = bgPosY >= 0 ? 0 : bgPosY+0.005f;
    // bgRot = bgRot <= 0 ? 0 : bgRot-0.0005f;
    bgRot = bgRot-0.0005f;
    static uint8_t bgFrame = 0;
    static float bgalpha = 0;
    static uint8_t numShownBubbles = 0;
    // printf("Frame %i\n", cnt);
    cnt++;
    if (cnt % 15 == 0)
        bgFrame = bgFrame == 3 ? bgFrame = 0 : bgFrame+1;

    // Render the scene
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C3D_FrameDrawOn(top);
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, 0x00000000, 0);

    C2D_Prepare();
    C2D_SceneBegin(top);

    if (cnt > 400 && cnt < 1000)
        bgalpha = min(bgalpha+0.01f,  1.0f);
    drawThing2(C2D_SpriteSheetGetImage(animSheet, bgFrame), 200, 120, 0, bgzoom, bgzoom, bgalpha,  bgRot, 400,  240);

    // drawThing2(C2D_SpriteSheetGetImage(logoSheet, 0), 0, 0, 0.5, 2, 1);

    if (cnt % 40 == 0)
        numShownBubbles = min(numShownBubbles+1, numThings-1);

    static float thingsAlpha = 1.0f;

    if (cnt > 700)
        thingsAlpha = max(thingsAlpha-0.005f, 0);
    // thingsAlpha = thingsAlpha < 0 ? 0 : thingsAlpha -= 0.001f;
    for (int i = 0; i < numShownBubbles; ++i)
    {
        sthing& thing = things[i];
        float scaleX = sin(time + thing.offset) * 0.1f + thing.scale;
        float scaleY = sin(time + thing.offset + M_PI + 0.4f) * 0.1f + thing.scale;
        float posX = sin(circlePhase + thing.offset) * 10 + thing.params.pos.x;
        float posY = cos(circlePhase + thing.offset) * 10 + thing.params.pos.y;
        // drawThing(*C2D_SpriteSheetGetImage(logoSheet, 0).tex, posX, posY, scaleX, scaleY);
        drawThing2(C2D_SpriteSheetGetImage(logoSheet, 0), posX, posY, thing.params.depth, scaleX, scaleY, thingsAlpha * thing.alpha);
        // break;
    }
    {
        sthing& thing = things[numThings-1];
        float scaleX = sin(time + thing.offset) * 0.1f + thing.scale;
        float scaleY = sin(time + thing.offset + M_PI + 0.4f) * 0.1f + thing.scale;
        float posX = sin(circlePhase + thing.offset) * 10 + thing.params.pos.x;
        float posY = cos(circlePhase + thing.offset) * 10 + thing.params.pos.y;
        // drawThing(*C2D_SpriteSheetGetImage(logoSheet, 0).tex, posX, posY, scaleX, scaleY);
        drawThing2(C2D_SpriteSheetGetImage(logoSheet, 0), posX, posY, thing.params.depth, scaleX, scaleY, thingsAlpha);
        // break;
    }
    static float fgalpha = 0;

    if (cnt>1100) {
        fgalpha = min(fgalpha + 0.01f,  1.0);
        bgalpha = max(bgalpha - 0.01f, 0.5);
        drawThing2(C2D_SpriteSheetGetImage(wickSheet, 0), 200, 120, 1.0, 1, 1, fgalpha, 0, 400, 240);
    }

    // C2D_DrawRectSolid(200, 200, 0, 50, 50, 0xf00f00f0);

    C3D_FrameEnd(0);
    // printf("\x1b[2;1HFrame:     %i%%\x1b[K", cnt);

    bool done = cnt > 1400;
    if (done)
        introExit();
    return done;
}
