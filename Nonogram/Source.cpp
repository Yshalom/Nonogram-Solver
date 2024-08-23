#include<Windows.h>
#include "HomePanel.h"
#include "GamePanel.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	while (true)
	{
		int* res = input_panel(hInstance);
		if (!res)
			return 0;
		int size_board = res[0];
		int move_panel = res[1];

		game_panel(hInstance, size_board, move_panel);
	}
}
