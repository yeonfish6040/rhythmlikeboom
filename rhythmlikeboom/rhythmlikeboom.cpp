#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string.h>

// global actions
void printFile(std::string filename) {
    std::ifstream file(("./resources/text/" + filename + ".txt").c_str());
    char ch;
    while (file.get(ch)) {
        printf("%c", ch);
    }
    file.close();
}

void rewrite() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(hConsole, coord);
}

void clear() {
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    DWORD numOfChar;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 10000, coord, &numOfChar);
}

// intro actions
void drawLine(int loop, int LENGTH, int BAR_LENGTH) {
    loop = LENGTH + BAR_LENGTH < loop ? (LENGTH + BAR_LENGTH)*2 - loop : loop;
	loop = loop < 0 ? loop * -1 : loop;

    for (int i = 0; i < loop - BAR_LENGTH; i++) { 
        if (BAR_LENGTH - (loop - LENGTH) < 0) break;
        printf("=");
    }

    if (loop < BAR_LENGTH)
        for (int i = 0; i < loop; i++) printf("#");
    else if (LENGTH < loop)
        for (int i = 0; i < BAR_LENGTH-(loop-LENGTH); i++) printf("#");
    else
        for (int i = 0; i < BAR_LENGTH; i++) printf("#");
    
    for (int i = 0; i < LENGTH - loop; i++) printf("=");
}

// animation
void introAnimation() {
    clear();

    int loop = 0;
    while (true) {
        const int BAR_LENGTH = 10;
        const int LENGTH = 130;

        if (loop > (LENGTH + BAR_LENGTH)*2) {
            loop = 0;
            clear();
            }
        if (GetKeyState(VK_RETURN) & 0x8000) {
            break;
        }

        rewrite();
        drawLine(LENGTH-loop, LENGTH, BAR_LENGTH);
        printFile("intro");
        drawLine(loop, LENGTH, BAR_LENGTH);
        loop++;

        Sleep(1);
    }
}

// initialize
void initScreen() {
    HWND wh = GetConsoleWindow();
    MoveWindow(wh, 0, 0, 1920, 1000, 1);
    MoveWindow(wh, 0, 0, 1920, 1080, 1);
    std::cout << "\n";
}

int main() {
    initScreen();
    introAnimation();
}