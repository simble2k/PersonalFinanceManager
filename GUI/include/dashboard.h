#pragma once

#include <functional>
#include <string>

#include "Helper.h"
#include "LayoutHelper.h"
#include "button.h"
#include "raylib.h"
#include "DataManager.h"  // For WalletsData

void drawDashboard(const WalletsData& data, std::function<void(int)> onButtonClick = nullptr);
