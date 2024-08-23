#include "HomePanel.h"


HWND button_input, size_input, move_input;
int size_board_move_panel[2];
int& size_board = size_board_move_panel[0];
int& move_panel = size_board_move_panel[1];
bool quit_program = false;

int* input_panel(HINSTANCE hInstance)
{
	HWND hwnd = CreateWindowA("Static", "Nonogram", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 500, 400, 350, 250, NULL, NULL, hInstance, NULL);
	SetWindowLongPtr(hwnd, -4, (LONG)WindowProc_input);
	HBRUSH brush = CreateSolidBrush(RGB(235, 235, 235));
	SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
	InvalidateRect(hwnd, NULL, TRUE);

	CreateWindowA("Static", "Write the size of nonogram board:", WS_VISIBLE | WS_CHILD, 30, 50, 230, 20, hwnd, NULL, hInstance, NULL);
	size_input = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD | ES_NUMBER, 260, 50, 24, 20, hwnd, NULL, hInstance, NULL);
	CreateWindowA("Static", "Write the size of nonogram Input:", WS_VISIBLE | WS_CHILD, 30, 100, 225, 20, hwnd, NULL, hInstance, NULL);
	move_input = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD | ES_NUMBER, 260, 100, 24, 20, hwnd, NULL, hInstance, NULL);
	button_input = CreateWindowA("Button", "OK", WS_VISIBLE | WS_CHILD | ES_NUMBER, 125, 150, 50, 30, hwnd, NULL, hInstance, NULL);


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CallWindowProc(DefWindowProc, hwnd, WM_CLOSE, 0, 0);
	return quit_program? NULL : size_board_move_panel;
}

LRESULT CALLBACK WindowProc_input(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(235, 235, 235));
		return (INT_PTR)CreateSolidBrush(RGB(235, 235, 235));
	case WM_CTLCOLOREDIT:
		SetBkColor((HDC)wParam, RGB(255, 255, 255));
		return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));

	case WM_CLOSE:
		quit_program = true;
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if ((HWND)lParam == button_input)
		{
			char a[4];
			GetWindowTextA(size_input, a, 4);
			size_board = 0;
			for (int i = 0; i < 4 && a[i]; i++)
			{
				size_board *= 10;
				size_board += a[i] - '0';
			}

			if (size_board == 0)
				size_board = 10;

			GetWindowTextA(move_input, a, 4);
			move_panel = 0;
			for (int i = 0; i < 4 && a[i]; i++)
			{
				move_panel *= 10;
				move_panel += a[i] - '0';
			}

			if (move_panel == 0)
				move_panel = 2 * size_board - 20;

			PostQuitMessage(0);
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
