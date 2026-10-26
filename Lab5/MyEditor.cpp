#include "framework.h"
#include "Resource.h"
#include "MyEditor.h"
#include <commdlg.h>
#include <cstdio>
#include <cwchar>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

MyEditor& MyEditor::Instance()
{
	static MyEditor instance;
	return instance;
}

MyEditor::MyEditor()
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

MyEditor::~MyEditor()
{
    for (int i = 0; i < TOOLS; ++i)
        delete m_tools[i].proto;
}

void MyEditor::OnCreate(HWND hWnd, HINSTANCE hInst)
{
    m_hWnd = hWnd;
    CreateToolbar(hInst);
    Select(m_cur);
    m_table.Create(hInst, hWnd, IDD_TABLE);
}

void MyEditor::CreateToolbar(HINSTANCE hInst)
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

void MyEditor::OnSize()
{
    if (m_hToolbar)
        SendMessage(m_hToolbar, TB_AUTOSIZE, 0, 0);
}

void MyEditor::Select(int index)
{
    m_cur = index;
    SendMessage(m_hToolbar, TB_CHECKBUTTON, m_tools[index].cmd, TRUE);
}

bool MyEditor::OnCommand(int id)
{
    if (id == ID_VIEW_TABLE)
    {
        m_table.Show(!m_table.IsVisible());
        return true;
    }

    if (id == ID_FILE_SAVE) { SaveToFile(); return true; }

    if (id == ID_FILE_OPEN) { LoadFromFile(); return true; }

    for (int i = 0; i < TOOLS; ++i)
        if (m_tools[i].cmd == id)
        {
            Select(i);
            return true;
        }
    return false;
}

void MyEditor::OnInitMenuPopup(HMENU hMenu)
{
    for (int i = 0; i < TOOLS; ++i)
        CheckMenuItem(hMenu, m_tools[i].cmd,
            MF_BYCOMMAND | (i == m_cur ? MF_CHECKED : MF_UNCHECKED));

    CheckMenuItem(hMenu, ID_VIEW_TABLE,
        MF_BYCOMMAND | (m_table.IsVisible() ? MF_CHECKED : MF_UNCHECKED));
}

LRESULT MyEditor::OnNotify(LPARAM lParam)
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

void MyEditor::DrawTrace(HDC hdc)
{
    int oldROP = SetROP2(hdc, R2_NOTXORPEN);
    Shape* p = m_tools[m_cur].proto;
    p->Set(m_ptStart.x, m_ptStart.y, m_ptCurrent.x, m_ptCurrent.y);
    p->Show(hdc, true);
    SetROP2(hdc, oldROP);
}

void MyEditor::OnLButtonDown(int x, int y)
{
    m_ptStart.x = m_ptCurrent.x = x;
    m_ptStart.y = m_ptCurrent.y = y;
    m_bTracking = true;
    SetCapture(m_hWnd);
}

void MyEditor::OnMouseMove(int x, int y)
{
    if (!m_bTracking) return;
    HDC hdc = GetDC(m_hWnd);
    DrawTrace(hdc);
    m_ptCurrent.x = x;
    m_ptCurrent.y = y;
    DrawTrace(hdc);
    ReleaseDC(m_hWnd, hdc);
}

void MyEditor::OnLButtonUp(int x, int y)
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
    UpdateTable();
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void MyEditor::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    for (int i = 0; i < m_nCount; ++i)
        m_pShapes[i]->Show(hdc, false);
    EndPaint(m_hWnd, &ps);
}

void MyEditor::OnDestroy()
{
    m_table.Destroy();
    for (int i = 0; i < m_nCount; ++i)
        delete m_pShapes[i];
    m_nCount = 0;
    if (m_hImageList)
        ImageList_Destroy(m_hImageList);
}

void MyEditor::UpdateTable()
{
    m_table.Clear();
    for (int i = 0; i < m_nCount; ++i)
    {
        Shape* p = m_pShapes[i];
        m_table.AddRow(p->Name(), p->X1(), p->Y1(), p->X2(), p->Y2());
    }
}

void MyEditor::SaveToFile()
{
    WCHAR fileName[MAX_PATH] = L"shapes.csv";

    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = L"CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"csv";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (!GetSaveFileName(&ofn)) return;         

    FILE* f = nullptr;
    if (_wfopen_s(&f, fileName, L"w") != 0 || !f)
    {
        MessageBox(m_hWnd, L"?? ??????? ???????? ????", L"???????", MB_ICONERROR);
        return;
    }

    fwprintf(f, L"Name,x1,y1,x2,y2\n");
    for (int i = 0; i < m_nCount; ++i)
    {
        Shape* p = m_pShapes[i];
        fwprintf(f, L"%ls,%ld,%ld,%ld,%ld\n",
            p->Name(), p->X1(), p->Y1(), p->X2(), p->Y2());
    }
    fclose(f);
}

void MyEditor::LoadFromFile()
{
    WCHAR fileName[MAX_PATH] = L"";

    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = L"CSV files (*.csv)\0*.csv\0All files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (!GetOpenFileName(&ofn)) return;          

    FILE* f = nullptr;
    if (_wfopen_s(&f, fileName, L"r") != 0 || !f)
    {
        MessageBox(m_hWnd, L"Не вдалося відкрити файл", L"Помилка", MB_ICONERROR);
        return;
    }

    for (int i = 0; i < m_nCount; ++i)          
        delete m_pShapes[i];
    m_nCount = 0;

    WCHAR line[256], name[64];
    long x1, y1, x2, y2;
    while (fgetws(line, 256, f) && m_nCount < N)
    {
        // назва — усе до коми, далі 4 числа
        if (swscanf_s(line, L"%63[^,],%ld,%ld,%ld,%ld",
            name, (unsigned)_countof(name), &x1, &y1, &x2, &y2) != 5)
            continue;                          

        for (int t = 0; t < TOOLS; ++t)         
            if (wcscmp(m_tools[t].proto->Name(), name) == 0)
            {
                Shape* p = m_tools[t].proto->Clone();
                p->Set(x1, y1, x2, y2);
                m_pShapes[m_nCount++] = p;
                break;
            }
    }
    fclose(f);

    UpdateTable();                              
    InvalidateRect(m_hWnd, nullptr, TRUE);      
}