#include "fire.h"
#include <math.h>
#include <string.h>


typedef struct {
    uint8_t r; // Red component
    uint8_t g; // Green component
    uint8_t b; // Blue component
    uint8_t a; // Alpha component (optional, can be set to 255 if not needed)
} Color;

static Color colors[256];
static uint16_t colors16[256];
u8* fire = NULL;


// Function to set a pixel color in a texture
inline void _setPixel_(u16* buffer, int const& x, int const& y, u16 const& color) {
    // if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    buffer[y * 512 + x] = color;
}


// Function to compose a 16-bit color in GPU_RGBA5551 format
inline uint16_t composeColorRGBA5551__(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    // Ensure the input values are within the acceptable ranges
    red = red & 0x1F;    // 5 bits
    green = green & 0x1F;  // 5 bits
    blue = blue & 0x1F;   // 5 bits
    alpha = alpha & 0x01;  // 1 bit

    // Compose the 16-bit color value
    uint16_t color = (red << 11) | (green << 6) | (blue << 1) | alpha;

    return color;
}


void initFire()
{
    int i;
    fire = (u8*) linearAlloc(SCR_WIDTH * SCR_HEIGHT);

    /* create a suitable fire palette, this is crucial for a good effect */
    /* black to blue, blue to red, red to yellow, yellow to white*/

    for (i = 0; i < 32; ++i)
    {
        /* black to blue, 32 values*/
        colors[i].b = i << 1;

        /* blue to red, 32 values*/
        colors[i + 32].r = i << 3;
        colors[i + 32].b =  64 - (i << 1);

        /*red to yellow, 32 values*/
        colors[i + 64].r = 255;
        colors[i + 64].g = i << 3;

        /* yellow to white, 162 */
        colors[i + 96].r = 255;
        colors[i + 96].g = 255;
        colors[i + 96].b = i << 2;
        colors[i + 128].r = 255;
        colors[i + 128].g = 255;
        colors[i + 128].b = 64 + (i << 2);
        colors[i + 160].r = 255;
        colors[i + 160].g = 255;
        colors[i + 160].b = 128 + (i << 2);
        colors[i + 192].r = 255;
        colors[i + 192].g = 255;
        colors[i + 192].b = 192 + i;
        colors[i + 224].r = 255;
        colors[i + 224].g = 255;
        colors[i + 224].b = 224 + i;
    }

    for (int i = 0; i < 256; ++i) {
        Color& cl = colors[i];
        colors16[i] = composeColorRGBA5551__(cl.r>>3, cl.g>>3, cl.b>>3, 31);
    }
}

void exitFire()
{
    linearFree(fire);
}

void drawFire(u16 *fb, bool const& clear, bool const& bottom, bool const& top)
{
    u16 temp;
    u8 index;
    static int i,j;

    if (clear)
        memset(fb, 0, SCR_WIDTH * SCR_HEIGHT * sizeof(uint16_t));
    // for (u32 i=0; i<(SCR_WIDTH*SCR_HEIGHT*3); i++)
        // fb[i] = 0;

    j = SCR_WIDTH * (SCR_HEIGHT- 1);
    for (i = 0; i < SCR_WIDTH - 1; i++)
    {
        int random = 1 + (int)(16.0 * (rand()/(RAND_MAX+1.0)));
        if (random > 9) /* the lower the value, the intenser the fire, compensate a lower value with a higher decay value*/
            fire[j + i] = 255; /*maximum heat*/
        else
            fire[j + i] = 0;
    }

    /* move fire upwards, start at bottom*/

    for (index = 0; index < 100 ; ++index)
    {
        for (i = 0; i < SCR_WIDTH - 1; ++i)
        {
            if (i == 0) /* at the left border*/
            {
                temp = fire[j];
                temp += fire[j + 1];
                temp += fire[j - SCR_WIDTH];
                temp /=3;
                // temp+=1;
            }
            else if (i == SCR_WIDTH - 1) /* at the right border*/
            {
                temp = fire[j + i];
                temp += fire[j - SCR_WIDTH + i];
                temp += fire[j + i - 1];
                temp /= 3;
                // temp+=1;
            }
            else
            {
                temp = fire[j + i];
                temp += fire[j + i + 1];
                temp += fire[j + i - 1];
                temp += fire[j - SCR_WIDTH + i];
                // temp+=3;
                temp >>= 2;
            }
            if (temp > 1)
                temp -= 1; /* decay */

            fire[j - SCR_WIDTH + i] = temp;
        }
        j -= SCR_WIDTH;
    }

    /* draw fire array to screen from bottom to top + 300*/
    for (i = SCR_HEIGHT - 1; i >= 120; --i)
    {
        for (j = SCR_WIDTH - 1; j >= 0; --j)
        {
            // Color cl = colors[fire[i*SCR_WIDTH + j]];
            // drawPixel(fb, j, i, cl.r, cl.g, cl.b);
            // uint16_t const& value = colors16[fire[i*SCR_WIDTH + j]];
            uint16_t const& value = colors16[fire[i*SCR_WIDTH + j]];
            if (bottom)
                _setPixel_(fb, j, i, value);
            if (top)
                _setPixel_(fb, j, SCR_HEIGHT-i, value);
        }
    }

}
