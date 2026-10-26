#pragma once
#include <windows.h>
#include <commctrl.h>
#include "Shapes.h"
#include "MyTable.h"

class MyEditor
{
public:
    static const int N = 122;
    static const int TOOLS = 6;

    static MyEditor& Instance();

	MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void    OnCreate(HWND hWnd, HINSTANCE hInst);
    void    OnSize();
    bool    OnCommand(int id);
    void    OnInitMenuPopup(HMENU hMenu);
    LRESULT OnNotify(LPARAM lParam);
    void    OnLButtonDown(int x, int y);
    void    OnMouseMove(int x, int y);
    void    OnLButtonUp(int x, int y);
    void    OnPaint();
    void    OnDestroy();

    HWND    TableWindow() const { return m_table.Handle(); }

private:
    MyEditor();
    ~MyEditor();

    struct Tool
    {
        int    cmd;
        Shape* proto;
    };

    HWND       m_hWnd;
    HWND       m_hToolbar;
    HIMAGELIST m_hImageList;

    Tool   m_tools[TOOLS];
    int    m_cur;

    Shape* m_pShapes[N];
    int    m_nCount;

    bool   m_bTracking;
    POINT  m_ptStart;
    POINT  m_ptCurrent;

    MyTable m_table;

    void CreateToolbar(HINSTANCE hInst);
    void Select(int index);
    void DrawTrace(HDC hdc);
    void UpdateTable();
    void SaveToFile();
    void LoadFromFile();
};