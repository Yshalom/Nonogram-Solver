#pragma once
#include<Windows.h>
#include "Mode3.h"
#include<vector>
using namespace std;

LRESULT CALLBACK WindowProc_main(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
void game_panel(HINSTANCE, int, int);
void updateBoard();
bool** solve_board();
Mode3 solve_rec(Mode3);
vector<int> ToVector(char[]);