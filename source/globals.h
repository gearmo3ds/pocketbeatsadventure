#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#define SCREEN_WIDTH TSIZE
#define SCREEN_HEIGHT 240
#define TSIZE 512

#define PI 3.14159265358979323846

static uint32_t *SOC_buffer = NULL;

void setPixel(u16* buffer, int x, int y, u16 color);
void setThickPixel(u16* buffer, int x, int y, u16 color);
void crect(float const& x, float const& y, uint8_t const& width, uint8_t const& height, uint32_t const& color);
bool loadTextureFromFile(C3D_Tex* tex, C3D_TexCube* cube, const char* path);
