#include "globals.h"
#include <citro2d.h>
#include <citro3d.h>

// Function to set a pixel color in a texture
void setPixel(u16* buffer, int x, int y, u16 color) {
    if (x<0 || x>400)
        return;
    // if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    buffer[y * SCREEN_WIDTH + x] = color;
}

void setThickPixel(u16* buffer, int x, int y, u16 color) {
    setPixel(buffer, x, y,  color);
    setPixel(buffer, x+1, y+0,  color);
    setPixel(buffer, x+0, y+1,  color);
    setPixel(buffer, x+1, y+1,  color);

    setPixel(buffer, x-1, y-0,  color);
    setPixel(buffer, x-0, y-1,  color);
    setPixel(buffer, x-1, y-1,  color);

    setPixel(buffer, x+2, y,  color);
    setPixel(buffer, x-2, y,  color);
    setPixel(buffer, x, y+2,  color);
    setPixel(buffer, x, y-2,  color);
}


void crect(float const& x, float const& y, uint8_t const& width, uint8_t const& height, uint32_t const& color)
{
    C2D_DrawRectSolid(x - float(width)/2.0f, y - float(height)/2.0f, 0, width, height, color);
}

// Helper function for loading a texture from a t3x file
bool loadTextureFromFile(C3D_Tex* tex, C3D_TexCube* cube, const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return false;

    Tex3DS_Texture t3x = Tex3DS_TextureImportStdio(f, tex, cube, false);
    fclose(f);
    if (!t3x)
        return false;

    // Delete the t3x object since we don't need it
    Tex3DS_TextureFree(t3x);
    return true;
}
