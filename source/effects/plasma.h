#pragma once
#include <3ds.h>

static const int SCR_WIDTH = 400;
static const int SCR_HEIGHT = 240;

void initPlasma();
void drawPlasma(u16 *fb);
// void drawPixel(u8* fb, int x, int y, u8 const& r, u8 const& g, u8 const& b);
