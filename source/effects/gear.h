#pragma once
#include <3ds.h>
#include <citro3d.h>


void gearInit();
void gearExit();
bool gearRender(C3D_RenderTarget* top, C3D_RenderTarget* off, C3D_Tex offtex);
