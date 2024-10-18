#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string.h>

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

void initScreen() {
    HWND wh = GetConsoleWindow();
    MoveWindow(wh, 0, 0, 1920, 1000, TRUE);
    MoveWindow(wh, 0, 0, 1920, 1080, TRUE);
    std::cout << "\n";
}

void introAnimation(int n) {

}

int main() {
    initScreen();

    int n = 0;
    int* np = &n;
    while (true) {
        if (GetKeyState(VK_RETURN) & 0x8000) {
            break;
        }
        rewrite();
        printFile("intro");
        Sleep(10);
    }
}