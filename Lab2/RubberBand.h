#pragma once
#include <windows.h>
#include "Shape.h"

inline void DrawRubberBand(HDC hdc, ShapeType type, POINT ptStart, POINT ptCurrent)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH)); 
    int oldROP = SetROP2(hdc, R2_NOTXORPEN);

    switch (type)
    {
    case ShapeType::Point:
        MoveToEx(hdc, ptCurrent.x, ptCurrent.y, nullptr);
        LineTo(hdc, ptCurrent.x + 1, ptCurrent.y);
        break;

    case ShapeType::Line:
        MoveToEx(hdc, ptStart.x, ptStart.y, nullptr);
        LineTo(hdc, ptCurrent.x, ptCurrent.y);
        break;

    case ShapeType::Rectangle:
    {
        int ox = 2 * ptStart.x - ptCurrent.x;
        int oy = 2 * ptStart.y - ptCurrent.y;
        Rectangle(hdc, min(ptCurrent.x, ox), min(ptCurrent.y, oy),
            max(ptCurrent.x, ox), max(ptCurrent.y, oy));
        break;
    }

    case ShapeType::Ellipse:
        Ellipse(hdc, min(ptStart.x, ptCurrent.x), min(ptStart.y, ptCurrent.y),
            max(ptStart.x, ptCurrent.x), max(ptStart.y, ptCurrent.y));
        break;
    }

    SetROP2(hdc, oldROP);
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}