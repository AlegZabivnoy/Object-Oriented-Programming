#pragma once
#include <windows.h>

class Shape
{
protected:
    long xs1 = 0, ys1 = 0, xs2 = 0, ys2 = 0;   

    static HPEN MakePen(bool trace, COLORREF color, int width = 1)
    {
        if (trace) return CreatePen(PS_DOT, 1, RGB(0, 0, 0));
        return CreatePen(PS_SOLID, width, color);
    }

public:
    virtual ~Shape() {}
    void Set(long x1, long y1, long x2, long y2) { xs1 = x1; ys1 = y1; xs2 = x2; ys2 = y2; }

    virtual void   Show(HDC hdc, bool trace) = 0;
    virtual Shape* Clone() const = 0;
    virtual const wchar_t* Name() const = 0;     

    long X1() const { return xs1; }             
    long Y1() const { return ys1; }
    long X2() const { return xs2; }
    long Y2() const { return ys2; }
};

class PointShape : public Shape
{
public:
    void Show(HDC hdc, bool trace) override
    {
        HPEN hPen = MakePen(trace, RGB(0, 0, 0), 4);
        HPEN hOld = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, xs1, ys1, nullptr);
        LineTo(hdc, xs1 + 1, ys1);
        SelectObject(hdc, hOld);
        DeleteObject(hPen);
    }
    Shape* Clone() const override { return new PointShape(*this); }
    const wchar_t* Name() const override { return L"Dot"; }
};

class LineShape : public virtual Shape
{
public:
    void Show(HDC hdc, bool trace) override
    {
        HPEN hPen = MakePen(trace, RGB(0, 0, 0));
        HPEN hOld = (HPEN)SelectObject(hdc, hPen);
        MoveToEx(hdc, xs1, ys1, nullptr);
        LineTo(hdc, xs2, ys2);
        SelectObject(hdc, hOld);
        DeleteObject(hPen);
    }
    Shape* Clone() const override { return new LineShape(*this); }
    const wchar_t* Name() const override { return L"Line"; }
};

class RectShape : public virtual Shape
{
protected:
    bool m_fill = true;                   
public:
    void Show(HDC hdc, bool trace) override
    {
        bool fill = m_fill && !trace;      
        HPEN   hPen = MakePen(trace, RGB(0, 0, 0));
        HBRUSH hBrush = fill ? CreateSolidBrush(RGB(255, 165, 0))
            : (HBRUSH)GetStockObject(NULL_BRUSH);
        HPEN   hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Rectangle(hdc, min(xs1, xs2), min(ys1, ys2), max(xs1, xs2), max(ys1, ys2));

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        if (fill) DeleteObject(hBrush);     
    }
    Shape* Clone() const override { return new RectShape(*this); }
    const wchar_t* Name() const override { return L"Rectangle"; }
};

class EllipseShape : public virtual Shape
{
public:
    void Show(HDC hdc, bool trace) override
    {
        long ox = 2 * xs1 - xs2;  
        long oy = 2 * ys1 - ys2;

        HPEN   hPen = MakePen(trace, RGB(0, 0, 0));
        HPEN   hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

        Ellipse(hdc, min(xs2, ox), min(ys2, oy), max(xs2, ox), max(ys2, oy));

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
    }
    Shape* Clone() const override { return new EllipseShape(*this); }
    const wchar_t* Name() const override { return L"Ellipse"; }
};

class LineOOShape : public LineShape, public EllipseShape
{
public:
    void Show(HDC hdc, bool trace) override
    {
        long x1 = xs1, y1 = ys1, x2 = xs2, y2 = ys2;   
        const long r = 6;                              

        LineShape::Show(hdc, trace);                   

        Set(x1, y1, x1 + r, y1 + r);                 
        EllipseShape::Show(hdc, trace);
        Set(x2, y2, x2 + r, y2 + r);               
        EllipseShape::Show(hdc, trace);

        Set(x1, y1, x2, y2);                         
    }
    Shape* Clone() const override { return new LineOOShape(*this); }
    const wchar_t* Name() const override { return L"Line with circles"; }
};

class CubeShape : public RectShape, public LineShape
{
public:
    CubeShape() { m_fill = false; }
    void Show(HDC hdc, bool trace) override
    {
        long x1 = xs1, y1 = ys1, x2 = xs2, y2 = ys2;   
        long w = x2 > x1 ? x2 - x1 : x1 - x2;
        long h = y2 > y1 ? y2 - y1 : y1 - y2;
        long d = (w < h ? w : h) / 3;                  

        Set(x1 + d, y1 - d, x2 + d, y2 - d);         
        RectShape::Show(hdc, trace);

        Set(x1, y1, x1 + d, y1 - d);  LineShape::Show(hdc, trace);  
        Set(x2, y1, x2 + d, y1 - d);  LineShape::Show(hdc, trace);
        Set(x1, y2, x1 + d, y2 - d);  LineShape::Show(hdc, trace);
        Set(x2, y2, x2 + d, y2 - d);  LineShape::Show(hdc, trace);

        Set(x1, y1, x2, y2);                          
        RectShape::Show(hdc, trace);
    }
    Shape* Clone() const override { return new CubeShape(*this); }
    const wchar_t* Name() const override { return L"Cube"; }
};