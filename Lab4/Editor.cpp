#include "framework.h"
#include "Resource.h"
#include "Editor.h"

#pragma comment(lib, "comctl32.lib")

Editor::Editor()
    : m_hWnd(nullptr), m_hToolbar(nullptr), m_hImageList(nullptr),
    m_cur(2), m_nCount(0), m_bTracking(false),
    m_ptStart{}, m_ptCurrent{}
{
    m_tools[0] = { ID_OBJECTS_DOT,       new PointShape };
    m_tools[1] = { ID_OBJECTS_LINE,      new LineShape };
    m_tools[2] = { ID_OBJECTS_RECTANGLE, new RectShape };
    m_tools[3] = { ID_OBJECTS_ELLIPSE,   new EllipseShape };
    m_tools[4] = { ID_OBJECTS_LINEOO,    new LineOOShape };
    m_tools[5] = { ID_OBJECTS_CUBE,      new CubeShape };

    for (int i = 0; i < N; ++i)
        m_pShapes[i] = nullptr;
}

Editor::~Editor()
{
    for (int i = 0; i < TOOLS; ++i)
        delete m_tools[i].proto;     
}

void Editor::OnCreate(HWND hWnd, HINSTANCE hInst)
{
    m_hWnd = hWnd;
    CreateToolbar(hInst);
    Select(m_cur);
}

void Editor::CreateToolbar(HINSTANCE hInst)
{
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_BAR_CLASSES };
    InitCommonControlsEx(&icex);

    m_hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0, m_hWnd, nullptr, hInst, nullptr);
    SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    m_hImageList = ImageList_LoadImage(hInst, MAKEINTRESOURCE(IDB_TOOLBAR),
        24, 0, RGB(255, 255, 255), IMAGE_BITMAP, LR_CREATEDIBSECTION);
    SendMessage(m_hToolbar, TB_SETIMAGELIST, 0, (LPARAM)m_hImageList);

    TBBUTTON buttons[TOOLS] = {};
    for (int i = 0; i < TOOLS; ++i)
    {
        buttons[i].iBitmap = i;
        buttons[i].idCommand = m_tools[i].cmd;
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_CHECKGROUP;
    }
    SendMessage(m_hToolbar, TB_ADDBUTTONS, TOOLS, (LPARAM)buttons);
    SendMessage(m_hToolbar, TB_AUTOSIZE, 0, 0);
}

void Editor::OnSize()
{
    if (m_hToolbar)
        SendMessage(m_hToolbar, TB_AUTOSIZE, 0, 0);
}

void Editor::Select(int index)
{
    m_cur = index;
    SendMessage(m_hToolbar, TB_CHECKBUTTON, m_tools[index].cmd, TRUE);
}

bool Editor::OnCommand(int id)
{
    for (int i = 0; i < TOOLS; ++i)
        if (m_tools[i].cmd == id)
        {
            Select(i);
            return true;
        }
    return false;
}

void Editor::OnInitMenuPopup(HMENU hMenu)
{
    for (int i = 0; i < TOOLS; ++i)
        CheckMenuItem(hMenu, m_tools[i].cmd,
            MF_BYCOMMAND | (i == m_cur ? MF_CHECKED : MF_UNCHECKED));
}

LRESULT Editor::OnNotify(LPARAM lParam)
{
    LPNMHDR pHdr = (LPNMHDR)lParam;
    if (pHdr->code == TTN_GETDISPINFO)
    {
        LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO)lParam;
        GetMenuString(GetMenu(m_hWnd), (UINT)pHdr->idFrom, pInfo->szText, 80, MF_BYCOMMAND);

        WCHAR* src = pInfo->szText;
        WCHAR* dst = pInfo->szText;
        for (; *src; ++src)
            if (*src != L'&') *dst++ = *src;
        *dst = L'\0';

        pInfo->lpszText = pInfo->szText;
    }
    return 0;
}

void Editor::DrawTrace(HDC hdc)
{
    int oldROP = SetROP2(hdc, R2_NOTXORPEN);
    Shape* p = m_tools[m_cur].proto;
    p->Set(m_ptStart.x, m_ptStart.y, m_ptCurrent.x, m_ptCurrent.y);
    p->Show(hdc, true);
    SetROP2(hdc, oldROP);
}

void Editor::OnLButtonDown(int x, int y)
{
    m_ptStart.x = m_ptCurrent.x = x;
    m_ptStart.y = m_ptCurrent.y = y;
    m_bTracking = true;
    SetCapture(m_hWnd);
}

void Editor::OnMouseMove(int x, int y)
{
    if (!m_bTracking) return;
    HDC hdc = GetDC(m_hWnd);
    DrawTrace(hdc);                 
    m_ptCurrent.x = x;
    m_ptCurrent.y = y;
    DrawTrace(hdc);                
    ReleaseDC(m_hWnd, hdc);
}

void Editor::OnLButtonUp(int x, int y)
{
    if (!m_bTracking) return;

    HDC hdc = GetDC(m_hWnd);
    DrawTrace(hdc);                 
    ReleaseDC(m_hWnd, hdc);
    m_bTracking = false;
    ReleaseCapture();

    if (m_nCount >= N) return;

    Shape* p = m_tools[m_cur].proto->Clone();  
    p->Set(m_ptStart.x, m_ptStart.y, x, y);
    m_pShapes[m_nCount++] = p;
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void Editor::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    for (int i = 0; i < m_nCount; ++i)
        m_pShapes[i]->Show(hdc, false);         
    EndPaint(m_hWnd, &ps);
}

void Editor::OnDestroy()
{
    for (int i = 0; i < m_nCount; ++i)
        delete m_pShapes[i];
    m_nCount = 0;
    if (m_hImageList)
        ImageList_Destroy(m_hImageList);
}