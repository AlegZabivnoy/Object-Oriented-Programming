#pragma once
#include "Shape.h"

class CRectangleShape : public CShape
{
public:
    CRectangleShape(POINT ptCenter, POINT ptCorner) : CShape(ptCenter, ptCorner) {}

    virtual void Draw(HDC hdc) const override
    {
        RECT rect = GetRect();

        HPEN   hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));          
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 105, 180));

        HPEN   hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);
    }

    virtual const TCHAR* GetName() const override { return _T("Rectangle"); }

private:
    RECT GetRect() const
    {
        POINT ptOpposite;
        ptOpposite.x = 2 * m_ptStart.x - m_ptEnd.x;
        ptOpposite.y = 2 * m_ptStart.y - m_ptEnd.y;

        RECT rect;
        rect.left = min(m_ptEnd.x, ptOpposite.x);
        rect.right = max(m_ptEnd.x, ptOpposite.x);
        rect.top = min(m_ptEnd.y, ptOpposite.y);
        rect.bottom = max(m_ptEnd.y, ptOpposite.y);
        return rect;
    }
};