#include "../internal.h"

static inline bool C2Di_CheckBufSpace(C2Di_Context* ctx, unsigned idx, unsigned vtx)
{
    size_t free_idx = ctx->idxBufSize - ctx->idxBufPos;
    size_t free_vtx = ctx->vtxBufSize - ctx->vtxBufPos;
    return free_idx >= idx && free_vtx >= vtx;
}

static inline void C2Di_RotatePoint(float* point, float rsin, float rcos)
{
    float x = point[0] * rcos - point[1] * rsin;
    float y = point[1] * rcos + point[0] * rsin;
    point[0] = x;
    point[1] = y;
}

static inline void C2Di_SwapUV(float* a, float* b)
{
    float temp[2] = { a[0], a[1] };
    a[0] = b[0];
    a[1] = b[1];
    b[0] = temp[0];
    b[1] = temp[1];
}

void C2Di_CalcQuad2(C2Di_Quad* quad, const C2D_DrawParams* params)
{
    const float w = fabs(params->pos.w);
    const float h = fabs(params->pos.h);

    quad->topLeft[0]  = -params->center.x;
    quad->topLeft[1]  = -params->center.y;
    quad->topRight[0] = -params->center.x+w;
    quad->topRight[1] = -params->center.y;
    quad->botLeft[0]  = -params->center.x;
    quad->botLeft[1]  = -params->center.y+h;
    quad->botRight[0] = -params->center.x+w;
    quad->botRight[1] = -params->center.y+h;

    if (params->angle != 0.0f)
    {
        float rsin = sinf(params->angle);
        float rcos = cosf(params->angle);
        C2Di_RotatePoint(quad->topLeft,  rsin, rcos);
        C2Di_RotatePoint(quad->topRight, rsin, rcos);
        C2Di_RotatePoint(quad->botLeft,  rsin, rcos);
        C2Di_RotatePoint(quad->botRight, rsin, rcos);
    }

    quad->topLeft[0]  += params->pos.x;
    quad->topLeft[1]  += params->pos.y;
    quad->topRight[0] += params->pos.x;
    quad->topRight[1] += params->pos.y;
    quad->botLeft[0]  += params->pos.x;
    quad->botLeft[1]  += params->pos.y;
    quad->botRight[0] += params->pos.x;
    quad->botRight[1] += params->pos.y;
}

bool C2D_DrawImage2(C2D_Image img, const C2D_DrawParams* params, const C2D_ImageTint* tint)
{
    C2Di_Context* ctx = C2Di_GetContext();
    if (!(ctx->flags & C2DiF_Active))
        return false;
    if (!C2Di_CheckBufSpace(ctx, 6, 4))
        return false;

    C2Di_SetMode((ctx->flags & C2DiF_TintMode_Mask) >> (C2DiF_TintMode_Shift - C2DiF_Mode_Shift));
    C2Di_SetTex(img.tex);
    C2Di_Update();

    // Calculate positions
    C2Di_Quad quad;
    C2Di_CalcQuad2(&quad, params);

    // Calculate texcoords
    float tcTopLeft[2], tcTopRight[2], tcBotLeft[2], tcBotRight[2];
    Tex3DS_SubTextureTopLeft    (img.subtex, &tcTopLeft[0],  &tcTopLeft[1]);
    Tex3DS_SubTextureTopRight   (img.subtex, &tcTopRight[0], &tcTopRight[1]);
    Tex3DS_SubTextureBottomLeft (img.subtex, &tcBotLeft[0],  &tcBotLeft[1]);
    Tex3DS_SubTextureBottomRight(img.subtex, &tcBotRight[0], &tcBotRight[1]);

    // Perform flip if needed
    if (params->pos.w < 0)
    {
        C2Di_SwapUV(tcTopLeft, tcTopRight);
        C2Di_SwapUV(tcBotLeft, tcBotRight);
    }
    if (params->pos.h < 0)
    {
        C2Di_SwapUV(tcTopLeft, tcBotLeft);
        C2Di_SwapUV(tcTopRight, tcBotRight);
    }

    // Calculate colors
    static const C2D_Tint s_defaultTint = { uint32_t(0xFF) << 24, 0.0f };
    const C2D_Tint* tintTopLeft  = tint ? &tint->corners[C2D_TopLeft]  : &s_defaultTint;
    const C2D_Tint* tintTopRight = tint ? &tint->corners[C2D_TopRight] : &s_defaultTint;
    const C2D_Tint* tintBotLeft  = tint ? &tint->corners[C2D_BotLeft]  : &s_defaultTint;
    const C2D_Tint* tintBotRight = tint ? &tint->corners[C2D_BotRight] : &s_defaultTint;

    C2Di_AppendQuad();
    C2Di_AppendVtx(quad.topLeft[0],  quad.topLeft[1],  params->depth, tcTopLeft[0],  tcTopLeft[1],  0, tintTopLeft->blend,  tintTopLeft->color);
    C2Di_AppendVtx(quad.topRight[0], quad.topRight[1], params->depth, tcTopRight[0], tcTopRight[1], 0, tintTopRight->blend, tintTopRight->color);
    C2Di_AppendVtx(quad.botLeft[0],  quad.botLeft[1],  params->depth, tcBotLeft[0],  tcBotLeft[1],  0, tintBotLeft->blend,  tintBotLeft->color);
    C2Di_AppendVtx(quad.botRight[0], quad.botRight[1], params->depth, tcBotRight[0], tcBotRight[1], 0, tintBotRight->blend, tintBotRight->color);
    return true;
}
