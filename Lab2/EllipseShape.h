#pragma once
#include "Shape.h"

class CEllipseShape : public CShape
{
public:
    CEllipseShape(POINT ptCorner1, POINT ptCorner2) : CShape(ptCorner1, ptCorner2) {}

    virtual void Draw(HDC hdc) const override
    {
        int left = min(m_ptStart.x, m_ptEnd.x);
        int right = max(m_ptStart.x, m_ptEnd.x);
        int top = min(m_ptStart.y, m_ptEnd.y);
        int bottom = max(m_ptStart.y, m_ptEnd.y);

        HPEN   hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); 
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); 

        HPEN   hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Ellipse(hdc, left, top, right, bottom);

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }

    virtual const TCHAR* GetName() const override { return _T("Ellipse"); }
};