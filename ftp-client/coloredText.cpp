#include "coloredText.h"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void Write(std::string text, int color) {
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	std::cout << text;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | WHITE));
}

void WriteLine(std::string text, int color) {
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | color));
	std::cout << text << std::endl;
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | WHITE));
}