#include "framework.h"
#include <CommCtrl.h>
#include <wchar.h>
#include "Module1.h"
#include "Module1.rh"

static int s_scrollPos = 1;
static INT_PTR CALLBACK DlgProc_MOD1(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hScroll = GetDlgItem(hWnd, IDC_SCROLLBAR1);
		SetScrollRange(hScroll, SB_CTL, 1, 100, FALSE);
		SetScrollPos(hScroll, SB_CTL, s_scrollPos, TRUE);

		WCHAR szBuffer[16];
		swprintf_s(szBuffer, L"%d", s_scrollPos);
		SetDlgItemText(hWnd, IDC_STATIC1, szBuffer);
		return (INT_PTR)TRUE;
	}

	case WM_HSCROLL:
	{
		HWND hScroll = (HWND)lParam;
		int scrollPos = GetScrollPos(hScroll, SB_CTL);

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			scrollPos--;
			break;
		case SB_LINERIGHT:
			scrollPos++;
			break;
		case SB_PAGELEFT:
			scrollPos -= 10;
			break;
		case SB_PAGERIGHT:
			scrollPos += 10;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			scrollPos = (int)(short)HIWORD(wParam);
			break;
		default:
			break;
		}
		if (scrollPos < 1) scrollPos = 1;
		else if (scrollPos > 100) scrollPos = 100;

		s_scrollPos = scrollPos;
		SetScrollPos(hScroll, SB_CTL, scrollPos, TRUE);

		WCHAR szBuffer[16];
		swprintf_s(szBuffer, L"%d", scrollPos);
		SetDlgItemText(hWnd, IDC_STATIC1, szBuffer);
		return (INT_PTR)TRUE;
	}

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

int Func_MOD1(HWND hWnd, int* pValue)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	INT_PTR result = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc_MOD1);

	if (result == 1 && pValue != nullptr)
	{
		*pValue = s_scrollPos;
		return 1;
	}
	{
		return 0;
	}
}