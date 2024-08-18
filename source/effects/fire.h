#pragma once
#include <3ds.h>
#include "plasma.h"


void initFire();
void drawFire(u16* fb, const bool &clear=true, const bool &bottom=true, const bool &top=false);
void exitFire();
