#pragma once
#include "Shape.h"

class CPointShape : public CShape
{
public:
    explicit CPointShape(POINT pt) : CShape(pt, pt) {}

    virtual void Draw(HDC hdc) const override
    {
        HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));   
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        MoveToEx(hdc, m_ptStart.x, m_ptStart.y, nullptr);
        LineTo(hdc, m_ptStart.x + 1, m_ptStart.y); 

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }

    virtual const TCHAR* GetName() const override { return _T("Point"); }
};