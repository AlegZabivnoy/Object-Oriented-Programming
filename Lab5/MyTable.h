#pragma once
#include <windows.h>
#include <commctrl.h>

class MyTable
{
public:
    MyTable();
    ~MyTable();

    bool Create(HINSTANCE hInst, HWND hParent, int dialogId);  
    void Show(bool show);                                     
    bool IsVisible() const;
    void Destroy();                                           
    HWND Handle() const { return m_hDlg; }

    void Clear();                                              
    void AddRow(const wchar_t* name, long x1, long y1, long x2, long y2);

private:
    HWND m_hDlg;     
    HWND m_hList;     

    static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
    INT_PTR OnMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void CreateList();
};
