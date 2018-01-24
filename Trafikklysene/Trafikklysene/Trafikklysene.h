#pragma once

#include "resource.h"
#include <vector>

void drawLight(HDC&, int, int, int, COLORREF);

void randomSpawn(int screenBottom, int carheigth);

void spawnHorizontal(int carheigth);

void spawnVertical(int screenBottom, int carheigth);
