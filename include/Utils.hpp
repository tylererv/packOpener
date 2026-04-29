#pragma once

#include "Enums.hpp"

#include <string>

std::string formatCash(int amount, const std::string& prefix = "$");
int clampInt(int value, int minValue, int maxValue);
bool drawTextButton(Rectangle bounds, const std::string& label);
bool drawTextButton(Rectangle bounds, const std::string& label, Color fillColor, Color textColor);
void drawWrappedText(const std::string& text, int x, int y, int maxWidth, int fontSize, Color color);
Color fadeColor(Color color, float alpha);
