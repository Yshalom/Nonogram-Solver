#pragma once
#include<Windows.h>

LRESULT CALLBACK WindowProc_input(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

/// <returns>
/// { size_board, move_panel }
/// When the program closes it'll return NULL
/// </returns>
int* input_panel(HINSTANCE hInstance);
