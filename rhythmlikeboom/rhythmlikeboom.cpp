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
    for (int i = 0; i < loop - BAR_LENGTH; i++) printf(" ");

    if (loop < BAR_LENGTH)
        for (int i = 0; i < BAR_LENGTH - loop; i++) printf("#");
    else
        for (int i = 0; i < BAR_LENGTH; i++) printf("#");
    
    for (int i = 0; i < LENGTH - loop; i++) printf(" ");
}

// animation
void introAnimation() {
    clear();

    int loop = 0;
    while (true) {
        const int BAR_LENGTH = 4;
        const int LENGTH = 130;

        if (loop > 130) loop = 0;
        if (GetKeyState(VK_RETURN) & 0x8000) {
            break;
        }

        rewrite();
        drawLine(LENGTH-loop, LENGTH, BAR_LENGTH);
        printFile("intro");
        drawLine(loop, LENGTH, BAR_LENGTH);
        Sleep(5);
        loop++;
    }
}

// initialize
void initScreen() {
    HWND wh = GetConsoleWindow();
    MoveWindow(wh, 0, 0, 1920, 1000, TRUE);
    MoveWindow(wh, 0, 0, 1920, 1080, TRUE);
    std::cout << "\n";
}

int main() {
    initScreen();
    introAnimation();
}