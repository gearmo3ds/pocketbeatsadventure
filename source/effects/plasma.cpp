#include "plasma.h"
#include <math.h>
#include "../globals.h"


typedef struct {
    uint8_t r; // Red component
    uint8_t g; // Green component
    uint8_t b; // Blue component
    uint8_t a; // Alpha component (optional, can be set to 255 if not needed)
} Color;

static u16 pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0, tpos1, tpos2, tpos3, tpos4;
static int aSin[512];
static Color colors[256];
static u16 colors16[256];

// void drawPixel(u8* fb, int x, int y, u8 const& r, u8 const& g, u8 const& b)
// {
//     u16 myy = SCR_HEIGHT - y - 1;
//     // if ((x*SCR_HEIGHT*3 + myy*3 + 2) > (SCR_WIDTH * SCR_HEIGHT))
//         // return;
//     fb[ (x*SCR_HEIGHT*3) + (myy*3) + 0] = b;
//     fb[ (x*SCR_HEIGHT*3) + (myy*3) + 1] = g;
//     fb[ (x*SCR_HEIGHT*3) + (myy*3) + 2] = r;
// }

/*
uint32_t ColorToU32(Color color)
{
    return color.a | (color.b << (u32)8) | (color.g << (u32)16) | (color.r << (u32)24);
    // return C2D_Color32(255, color.b, color.g, color.r);
}
*/

inline uint32_t ColorToU32(Color const& color)
{
    return color.a | (color.b << (u32)8) | (color.g << (u32)16) | (color.r << (u32)24);
    // return C2D_Color32(255, color.b, color.g, color.r);
}



// Function to compose a 16-bit color in GPU_RGBA5551 format
inline uint16_t composeColorRGBA5551_(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    // Ensure the input values are within the acceptable ranges
    red = red & 0x1F;    // 5 bits
    green = green & 0x1F;  // 5 bits
    blue = blue & 0x1F;   // 5 bits
    alpha = alpha & 0x01;  // 1 bit

    // Compose the 16-bit color value
    uint16_t color = (red << 11) | (green << 6) | (blue << 1) | alpha;

    return color;
}

void initPlasma()
{
    int i;
    float rad;

    /*create sin lookup table */
    for (i = 0; i < 512; i++)
    {
        rad =  ((float)i * 0.703125) * 0.0174532; /* 360 / 512 * degree to rad, 360 degrees spread over 512 values to be able to use AND 512-1 instead of using modulo 360*/
        aSin[i] = sin(rad) * 1024; /*using fixed point math with 1024 as base*/
    }

    const u8 df = 0;
    /* create palette */
    for (i = 0; i < 64; ++i)
    {
        colors[i].r = i << 2;
        colors[i].b = 255 - ((i << 2) + 1);
        colors[i+64].g = 255;
        colors[i+64].r = (i << 2) + 1;
        colors[i+128].r = 255 - ((i << 2) + 1);
        colors[i+128].b = 255 - ((i << 2) + 1);
        colors[i+192].g = (i << 2) + 1;
    }

    for (int i = 0; i < 256; ++i) {
        // colors[i].a = 255;
        Color& cl = colors[i];
        // colors32[i] = ColorToU32(colors[i]);
        colors16[i] = composeColorRGBA5551_(cl.r>>3, cl.g>>3, cl.b>>3, 255);

    }
}

// Function to set a pixel color in a texture
inline void _setPixel(u16* buffer, int const& x, int const& y, u16 const& color) {
    // if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    buffer[y * SCREEN_WIDTH + x] = color;
}


void drawPlasma(u16 *fb)
{
    // u32 clor;
    // for (int i = 0; i < 400; ++i) {
    //     for (int iy = 0; iy < 240; ++iy) {
    //         setPixel(fb,i,iy,colors32[iy%256]);
    //     }
    // }
    // return;

    u8 index=0;
    u64 x;
    u16 value=0;

    tpos4 = pos4;
    tpos3 = pos3;

    // image = screen->pixels;
    // plt = plasm;

    // u8 shit = 0;
    // u32* bf = (u32*) fb;

    for (u16 py = 0; py < 240; py++)
    {
        tpos1 = pos1 + 5;
        tpos2 = pos2 + 3;

        tpos3 &= 511;
        tpos4 &= 511;

        for (u16 px = 0; px < 400; px++)
        {
            tpos1 &= 511;
            tpos2 &= 511;

            x = aSin[tpos1] + aSin[tpos2] + aSin[tpos3] + aSin[tpos4]; /*actual plasma calculation*/

            index = 128 + (x >> 4); /*fixed point multiplication but optimized so basically it says (x * (64 * 1024) / (1024 * 1024)), x is already multiplied by 1024*/
            // index = px % 256;
            // index = (x * (64 * 1024) / (1024 * 1024));

            // index = shit % 32;
            // index = 32;
            // u16 myy = 240 - py;
            // plasm[px*240 + py] = index;

            // plasm[blbl] = index;
            // blbl++;
            // *plt++ = index;

            Color& cl = colors[index];
            value = composeColorRGBA5551_(cl.r>>3, cl.g>>3, cl.b>>3, 255);

            // value = ColorToU32(colors[index]);
            // value = 0x0000ffff;
            // drawPixel(fb, px, py, cl.r, cl.g, cl.b);
            // u32 clrrr =  ((uint32_t)255 << 24) | ((uint32_t)cl.r << 16) | ((uint32_t)cl.g << 8) | (uint32_t)cl.b;
            _setPixel(fb, px, py, value);
            // _setPixel(fb, px, py, 0);

            tpos1 += 2;
            tpos2 += 1;
        }

        tpos4 += 2;
        tpos3 += 1;
    }

    /* move plasma */

    static u32 cnt = 0;
    if (cnt % 2 == 0) {
        pos1 +=3;
        pos3 +=4;
    }
    cnt++;
}
