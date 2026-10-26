#include "MyTable.h"

#pragma comment(lib, "comctl32.lib")

MyTable::MyTable() : m_hDlg(nullptr), m_hList(nullptr) {}

MyTable::~MyTable()
{
    Destroy();
}

bool MyTable::Create(HINSTANCE hInst, HWND hParent, int dialogId)
{
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_LISTVIEW_CLASSES };
    InitCommonControlsEx(&icex);

    m_hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(dialogId), hParent,
                               DlgProc, (LPARAM)this);
    return m_hDlg != nullptr;
}

void MyTable::Show(bool show)
{
    if (m_hDlg)
        ShowWindow(m_hDlg, show ? SW_SHOW : SW_HIDE);
}

bool MyTable::IsVisible() const
{
    return m_hDlg && IsWindowVisible(m_hDlg);
}

void MyTable::Destroy()
{
    if (m_hDlg)
    {
        DestroyWindow(m_hDlg);
        m_hDlg = nullptr;
        m_hList = nullptr;
    }
}

void MyTable::Clear()
{
    if (m_hList)
        ListView_DeleteAllItems(m_hList);
}

void MyTable::AddRow(const wchar_t* name, long x1, long y1, long x2, long y2)
{
    if (!m_hList) return;

    int row = ListView_GetItemCount(m_hList);       

    LVITEM item = {};
    item.mask = LVIF_TEXT;
    item.iItem = row;
    item.pszText = const_cast<LPWSTR>(name);         
    ListView_InsertItem(m_hList, &item);

    long v[4] = { x1, y1, x2, y2 };
    WCHAR buf[16];
    for (int c = 0; c < 4; ++c)                      
    {
        wsprintfW(buf, L"%ld", v[c]);
        ListView_SetItemText(m_hList, row, c + 1, buf);
    }
}

INT_PTR CALLBACK MyTable::DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MyTable* self;
    if (msg == WM_INITDIALOG)
    {
        self = (MyTable*)lParam;                              
        SetWindowLongPtr(hDlg, DWLP_USER, (LONG_PTR)self);   
        self->m_hDlg = hDlg;
    }
    else
        self = (MyTable*)GetWindowLongPtr(hDlg, DWLP_USER);

    return self ? self->OnMessage(msg, wParam, lParam) : FALSE;
}

INT_PTR MyTable::OnMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        CreateList();
        return TRUE;

    case WM_SIZE:                     
        if (m_hList)
            MoveWindow(m_hList, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return TRUE;

    case WM_CLOSE:                    
        Show(false);
        return TRUE;
    }
    return FALSE;
}

void MyTable::CreateList()
{
    RECT rc;
    GetClientRect(m_hDlg, &rc);

    m_hList = CreateWindowEx(0, WC_LISTVIEW, nullptr,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
        0, 0, rc.right, rc.bottom, m_hDlg, nullptr,
        (HINSTANCE)GetWindowLongPtr(m_hDlg, GWLP_HINSTANCE), nullptr);
    ListView_SetExtendedListViewStyle(m_hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    const wchar_t* titles[5] = { L"Name", L"x1", L"y1", L"x2", L"y2" };
    int widths[5] = { 130, 55, 55, 55, 55 };
    for (int i = 0; i < 5; ++i)
    {
        LVCOLUMN col = {};
        col.mask = LVCF_TEXT | LVCF_WIDTH;
        col.pszText = const_cast<LPWSTR>(titles[i]);
        col.cx = widths[i];
        ListView_InsertColumn(m_hList, i, &col);
    }
}
