#include "framework.h"
#include "Module3.h"
#include "Module3.rh"

static INT_PTR CALLBACK DlgProc_MOD3(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:

		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hWnd, 1);
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_BUTTON_BACK)
		{
			EndDialog(hWnd, 2);
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

int Func_MOD3(HWND hWnd)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	return (int)DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, DlgProc_MOD3);
}