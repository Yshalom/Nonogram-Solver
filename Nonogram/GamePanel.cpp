#include "GamePanel.h"

int size_board;

// ----------------------------------------- << The window of the game >> ---------------------------------------------

WNDPROC editClassProc;
LRESULT CALLBACK WindowProc_inputBox(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

HANDLE updateBoardThread = NULL;
ULONG CALLBACK updateBoardProc(LPVOID param);

bool** solve_board_thread_result;
ULONG CALLBACK solve_board_Proc(LPVOID param);

HWND hwnd, hImageView_main, button_main, Stamp;
bool** board_main = NULL;
vector<HWND> X_input_main, Y_input_main;
void game_panel(HINSTANCE hInstance, int size, int move_panel)
{
	size_board = size;

	WNDCLASSW editClass;
	if (!GetClassInfoW(hInstance, L"Edit", &editClass))
		return;
	editClassProc = editClass.lpfnWndProc;

	RECT r;
	GetWindowRect(GetDesktopWindow(), &r);
	int x = (r.right - size_board * 20 - 160) / 2,
		y = (r.bottom - size_board * 20 - 195) / 2;

	hwnd = CreateWindowA("Static", "Nonogram", WS_VISIBLE | WS_OVERLAPPEDWINDOW, x, y, size_board * 20 + 160 + move_panel, size_board * 20 + 195 + move_panel, NULL, NULL, hInstance, NULL);
	SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)WindowProc_main);
	HBRUSH brush = CreateSolidBrush(RGB(235, 235, 235));
	SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
	InvalidateRect(hwnd, NULL, TRUE);

	hImageView_main = CreateWindowEx(NULL, L"STATIC", NULL, SS_BITMAP | WS_VISIBLE | WS_CHILD, 70 + move_panel, 95 + move_panel, 0, 0, hwnd, NULL, hInstance, NULL);
	button_main = CreateWindowA("Button", "update the board", WS_VISIBLE | WS_CHILD, size_board * 10 + move_panel, size_board * 20 + 113 + move_panel, 130, 30, hwnd, NULL, hInstance, NULL);
	Stamp = CreateWindowA("Static", "יצחק שלום", WS_VISIBLE | WS_CHILD, 10, size_board * 20 + 133 + move_panel, 70, 20, hwnd, NULL, hInstance, NULL);
	X_input_main.resize(size_board);
	Y_input_main.resize(size_board);
	for (int i = 0; i < size_board; i++)
	{
		X_input_main[i] = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD, 10, 96 + 20 * i + move_panel, 58 + move_panel, 18, hwnd, NULL, hInstance, NULL);
		SetWindowLongPtr(X_input_main[i], GWLP_WNDPROC, (LONG)WindowProc_inputBox);
		Y_input_main[i] = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD | ES_MULTILINE, 71 + 20 * i + move_panel, 10, 18, 83 + move_panel, hwnd, NULL, hInstance, NULL);
		SetWindowLongPtr(Y_input_main[i], GWLP_WNDPROC, (LONG)WindowProc_inputBox);
	}

	HDC hdcImage = GetDC(hImageView_main);
	for (int i = 0; i < size_board * 20; i++)
		for (int j = 0; j < size_board * 20; j++)
		{
			if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
				SetPixelV(hdcImage, i, j, RGB(127, 127, 127));
			else
				SetPixelV(hdcImage, j, i, RGB(255, 255, 255));
		}
	for (int i = 0; i < size_board * 20; i++)
	{
		SetPixelV(hdcImage, i, 1, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, 2, RGB(127, 127, 127));
		SetPixelV(hdcImage, 1, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, 2, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, size_board * 20 - 2, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, size_board * 20 - 3, RGB(127, 127, 127));
		SetPixelV(hdcImage, size_board * 20 - 2, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, size_board * 20 - 3, i, RGB(127, 127, 127));
	}
	if (size_board % 5 == 0)
	{
		for (int i = 5; i < size_board; i += 5)
		{
			for (int j = 0; j < size_board * 20; j++)
			{
				SetPixelV(hdcImage, i * 20 + 1, j, RGB(127, 127, 127));
				SetPixelV(hdcImage, j, i * 20 + 1, RGB(127, 127, 127));
			}
		}
	}

	DeleteDC(hdcImage);


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int findIndexOf(vector<HWND> arr, HWND item)
{
	for (int i = 0; i < arr.size(); i++)
		if (arr[i] == item)
			return i;
	return -1;
}

LRESULT CALLBACK WindowProc_main(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if ((HWND)lParam == button_main)
		{	
			if (updateBoardThread)
			{
				if (WaitForSingleObject(updateBoardThread, 0) == WAIT_TIMEOUT)
					break;
				CloseHandle(updateBoardThread);
			}
			updateBoardThread = CreateThread(NULL, 0, updateBoardProc, NULL, 0, NULL);
		}
		break;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(235, 235, 235));
		return (LRESULT)CreateSolidBrush(RGB(235, 235, 235));
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WindowProc_inputBox(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN)
    {
		switch (wParam)
        {
			int index;

            case VK_LEFT:
				index = findIndexOf(Y_input_main, hwnd);
				if (index == -1)
					break;
				SetFocus(index == 0 ? Y_input_main.back() : Y_input_main[index - 1]);
				return 0;
            case VK_RIGHT:
				index = findIndexOf(Y_input_main, hwnd);
				if (index == -1)
					break;
				index++;
				SetFocus(index == Y_input_main.size() ? Y_input_main.front() : Y_input_main[index]);
				return 0;
            case VK_UP:
				index = findIndexOf(X_input_main, hwnd);
				if (index == -1)
					break;
				SetFocus(index == 0 ? X_input_main.back() : X_input_main[index - 1]);
                return 0;
            case VK_DOWN:
				index = findIndexOf(X_input_main, hwnd);
				if (index == -1)
					break;
				index++;
				SetFocus(index == X_input_main.size() ? X_input_main.front() : X_input_main[index]);
                return 0;
        }
    }

    return editClassProc(hwnd, uMsg, wParam, lParam);
}

ULONG CALLBACK updateBoardProc(LPVOID param)
{
	updateBoard();
	return 0;
}

ULONG CALLBACK solve_board_Proc(LPVOID param)
{
	solve_board_thread_result = solve_board();

	return 0;
}

void updateBoard()
{
	HANDLE solveBoardThread = CreateThread(NULL, 0, solve_board_Proc, NULL, 0, NULL);

	HDC hdcImage = GetDC(hImageView_main);

	for (int i = size_board * 20 - 1; i >= 0; i--)
		for (int j = size_board * 20 - 1; j >= 0; j--)
		{
			if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
				SetPixelV(hdcImage, i, j, RGB(235, 235, 235));
			else
				SetPixelV(hdcImage, j, i, RGB(235, 235, 235));
		}

	bool Q[18][18] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,1,1,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};
	for (int i = 0; i < size_board * 20; i++)
		for (int j = 0; j < size_board * 20; j++)
		{
			if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
				SetPixelV(hdcImage, i, j, RGB(127, 127, 127));
			else
				SetPixelV(hdcImage, j, i, (Q[i % 20 - 1][j % 20 - 1] ? RGB(255, 255, 255) : RGB(200, 100, 100)));
		}

	WaitForSingleObject(solveBoardThread, INFINITE);
	CloseHandle(solveBoardThread);

	if (solve_board_thread_result) {

		for (int i = size_board * 20 - 1; i >= 0; i--)
			for (int j = size_board * 20 - 1; j >= 0; j--)
			{
				if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
					SetPixelV(hdcImage, i, j, RGB(235, 235, 235));
				else
					SetPixelV(hdcImage, j, i, RGB(235, 235, 235));
			}

		bool X[18][18] = {
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
			{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
			{0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0},
			{0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0},
			{0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
			{0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
			{0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0},
			{0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
			{0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
			{0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
			{0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0},
			{0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
			{0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0},
			{0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0},
			{0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0},
			{0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		};
		for (int i = 0; i < size_board * 20; i++)
			for (int j = 0; j < size_board * 20; j++)
			{
				if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
					SetPixelV(hdcImage, i, j, RGB(127, 127, 127));
				else
					SetPixelV(hdcImage, j, i, (solve_board_thread_result[i / 20][j / 20] ? RGB(0, 0, 0) : (X[i % 20 - 1][j % 20 - 1] ? RGB(0, 0, 0) : RGB(255, 255, 255))));
			}

		for (int i = 0; i < size_board; i++)
			delete[] solve_board_thread_result[i];
		delete solve_board_thread_result;
	}
	else
	{
		MessageBoxA(hwnd, "It seems like there's no solution for the input.\nCouldn't find a board which is appropriate to the input.", "No solution", MB_OK);

		for (int i = size_board * 20 - 1; i >= 0; i--)
			for (int j = size_board * 20 - 1; j >= 0; j--)
			{
				if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
					SetPixelV(hdcImage, i, j, RGB(235, 235, 235));
				else
					SetPixelV(hdcImage, j, i, RGB(235, 235, 235));
			}

		for (int i = 0; i < size_board * 20; i++)
			for (int j = 0; j < size_board * 20; j++)
			{
				if (i % 20 == 0 || i % 20 == 19 || j % 20 == 0 || j % 20 == 19)
					SetPixelV(hdcImage, i, j, RGB(127, 127, 127));
				else
					SetPixelV(hdcImage, j, i, RGB(255, 255, 255));
			}
	}
	for (int i = 0; i < size_board * 20; i++)
	{
		SetPixelV(hdcImage, i, 1, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, 2, RGB(127, 127, 127));
		SetPixelV(hdcImage, 1, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, 2, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, size_board * 20 - 2, RGB(127, 127, 127));
		SetPixelV(hdcImage, i, size_board * 20 - 3, RGB(127, 127, 127));
		SetPixelV(hdcImage, size_board * 20 - 2, i, RGB(127, 127, 127));
		SetPixelV(hdcImage, size_board * 20 - 3, i, RGB(127, 127, 127));
	}
	if (size_board % 5 == 0)
	{
		for (int i = 5; i < size_board; i += 5)
		{
			for (int j = 0; j < size_board * 20; j++)
			{
				SetPixelV(hdcImage, i * 20 + 1, j, RGB(127, 127, 127));
				SetPixelV(hdcImage, j, i * 20 + 1, RGB(127, 127, 127));
			}
		}
	}

	DeleteDC(hdcImage);
}

// ----------------------------------------- << The logics of the game >> ---------------------------------------------

bool** solve_board()
{
	Mode3::size = size_board;
	Mode3::right.clear();
	Mode3::top.clear();
	for (int i = 0; i < size_board; i++)
	{
		char c[50];
		GetWindowTextA(X_input_main[i], c, 50);
		Mode3::right.push_back(ToVector(c));
		GetWindowTextA(Y_input_main[i], c, 50);
		char c2[50];
		int k = 0;
		for (int j = 0; c[j]; j++)
		{
			if (c[j] == '\r')
				c2[k++] = ' ';
			else if (c[j] != '\n')
				c2[k++] = c[j];
		}
		c2[k] = '\0';
		Mode3::top.push_back(ToVector(c2));
	}
	Mode3 a;
	a = solve_rec(a);
	if (!a.is_right())
		return NULL;

	bool** res = new bool* [size_board];
	for (int i = 0; i < size_board; i++)
	{
		res[i] = new bool[size_board];
		for (int j = 0; j < size_board; j++)
			res[i][j] = (bool)a[i][j];
	}

	//cout << "\n\n**************************\n\n";
	return res;
}

Mode3 solve_rec(Mode3 m)
{
	//if (!play)
	//	return m;

	MSG msg;
	if (PeekMessageA(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	int index = 0;
	for (; index < size_board && m[0][index] != Indefinite; index++);
	if (index == size_board)
		return m;

	vector<int> v(Mode3::top[index].size() + 1);
	v[0] = -1;
	v[v.size() - 1] = 0;
	for (int i = 1; i < v.size() - 1; i++)
		v[i] = 1;

	int max = size_board;
	if (0 < (int)v.size() - 3) max -= v.size() - 3;
	for (int i = 0; i < Mode3::top[index].size(); i++)
		max -= Mode3::top[index][i];

	while (true)
	{
		v[0]++;
		vector<int>::iterator it = v.begin();
		while (*it > max && it != v.end() - 1)
		{
			if (it == v.begin())
				*it = 0;
			else
				*it = 1;
			(*++it)++;
		}

		int sum = 0;
		for (int i = 0; i < v.size(); i++)
			sum += v[i];
		for (int i = 0; i < Mode3::top[index].size(); i++)
			sum += Mode3::top[index][i];

		if (sum == size_board)
		{
			int index2 = 0;
			for (int i = 0; i < v.size(); i++)
			{
				for (int j = v[i]; j; j--)
					m[index2++][index] = False;
				if (i != v.size() - 1)
					for (int j = Mode3::top[index][i]; j; j--)
						m[index2++][index] = True;
			}


			if (m.is_right())
				return m;
			else if (m.can_be_right())
			{
				Mode3 x = solve_rec(m);
				if (x.is_right())
					return x;
			}
		}

		if (it == v.end() - 1 && *it > max)
			break;
	}
	return m;
}

vector<int> ToVector(char c[])
{
	vector<int> res;
	for (int i = 0; c[i]; i++)
	{
		if (c[i] >= '0' && c[i] <= '9')
		{
			if (i == 0 || c[i - 1] == ' ')
				res.push_back(c[i] - '0');
			else
				res[res.size() - 1] = res[res.size() - 1] * 10 + c[i] - '0';
		}
	}
	return res;
}

