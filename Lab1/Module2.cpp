#include "framework.h"
#include "Module2.h"
#include "Module2.rh"

static INT_PTR CALLBACK DlgProc_MOD2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hWnd, 1);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, 0);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int Func_MOD2(HWND hWnd)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	return (int)DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DlgProc_MOD2);
}