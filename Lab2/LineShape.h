#pragma once
#include "Shape.h"

class CLineShape : public CShape
{
public:
    CLineShape(POINT ptStart, POINT ptEnd) : CShape(ptStart, ptEnd) {}

    virtual void Draw(HDC hdc) const override
    {
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        MoveToEx(hdc, m_ptStart.x, m_ptStart.y, nullptr);
        LineTo(hdc, m_ptEnd.x, m_ptEnd.y);

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    virtual const TCHAR* GetName() const override { return _T("Line"); }
};