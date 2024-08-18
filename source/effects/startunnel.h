#pragma once
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

void initTunnelScene();
void deinitTunnelScene();
void drawTunnel(u16* mypix, const bool &clear=true);
void drawHeadSprites(C2D_SpriteSheet texSheet, const bool &show);
void drawTileSquares(float time);
void drawThings();
bool renderTunnelScene(u16* mypix,
                       C2D_SpriteSheet texSheet,
                       C3D_Tex offtex,
                       C3D_RenderTarget* off,
                       C3D_Tex tex,
                       C3D_RenderTarget* top);

bool renderPlasmaScene(u16* mypix,
                       C2D_SpriteSheet texSheet,
                       C3D_Tex offtex,
                       C3D_RenderTarget* off,
                       C3D_Tex tex,
                       C3D_RenderTarget* top);
