#pragma once
#include <windows.h>
#include <tchar.h>

enum class ShapeType
{
    Point,
    Line,
    Rectangle,
    Ellipse
};

class CShape
{
protected:
    POINT m_ptStart;   
    POINT m_ptEnd;     

public:
    CShape(POINT ptStart, POINT ptEnd)
        : m_ptStart(ptStart), m_ptEnd(ptEnd)
    {
    }

    virtual ~CShape() {}
    virtual void Draw(HDC hdc) const = 0;
    virtual const TCHAR* GetName() const = 0;

    POINT GetStart() const { return m_ptStart; }
    POINT GetEnd()   const { return m_ptEnd; }
};