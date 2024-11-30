#define _CRT_SECURE_NO_WARNINGS
#define BUFFSIZE 768


#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <string.h>
#include <vector>
#include <string>

using namespace std;


// global variables
char* screen;
int columns, rows;

// global utils
HWND GetConsoleHandle() {
    HWND hwnd;
    char pszWindowTitle[BUFFSIZE];

    GetConsoleTitleA(pszWindowTitle, BUFFSIZE);
    int i = strlen(pszWindowTitle);
    _itoa(GetTickCount(), &pszWindowTitle[i], 16);
    _itoa(GetCurrentProcessId(), &pszWindowTitle[strlen(pszWindowTitle)], 16);
    SetConsoleTitleA(pszWindowTitle);
    Sleep(50);
    hwnd = FindWindowA(NULL, pszWindowTitle);
    pszWindowTitle[i] = 0;
    SetConsoleTitleA(pszWindowTitle);
    return(hwnd);
}

int* getPosByPersentage(int x, int y) {
    int pos[2];
    pos[0] = columns * x / 100;
    pos[1] = rows * y / 100;
    return pos;
}

vector<string> getFile(string filename) {
    ifstream file(("./resources/text/" + filename + ".txt").c_str());
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();

    return lines;
}

void concatVector(vector<string>& a, const vector<string>& b) {
    for (int i = 0; i < b.size(); i++) {
        a.push_back(b.at(i));
    }
}

// global actions
void rewrite() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    SetConsoleCursorPosition(hConsole, coord);
}

void clear() {
    for (int i = 0; i < columns * rows; i++)
        screen[i] = ' ';
}

void clearScreen() {
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    DWORD numOfChar;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 10000, coord, &numOfChar);
}

void printString(vector<string> data, int center) {
    if (center) {
        int space = (rows - data.size()) / 2;
        for (int j = 0; j < space; j++) printf("\n");
        for (int i = 0; i < data.size(); i++) {
            int pos = (columns - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }else {
        for (int i = 0; i < data.size(); i++) {
            int pos = (columns - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }
}

void render() {
    rewrite();
    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < rows; j++)
            printf("%c", screen[i * rows + j]);
        printf("\n");
    }
}

// intro actions
string drawLine(int loop, int LENGTH, int BAR_LENGTH) {
    string line;

    loop = LENGTH + BAR_LENGTH < loop ? (LENGTH + BAR_LENGTH) * 2 - loop : loop;
    loop = loop < 0 ? loop * -1 : loop;

    for (int i = 0; i < loop - BAR_LENGTH; i++) {
        if (BAR_LENGTH - (loop - LENGTH) < 0) break;
        line.append("=");
    }

    if (loop < BAR_LENGTH)
        for (int i = 0; i < loop; i++) line.append("#");
    else if (LENGTH < loop)
        for (int i = 0; i < BAR_LENGTH - (loop - LENGTH); i++) line.append("#");
    else
        for (int i = 0; i < BAR_LENGTH; i++) line.append("#");

    for (int i = 0; i < LENGTH - loop; i++) line.append("=");

    return line;
}

// animation
void introAnimation() {
    clearScreen();

    int loop = 0;
    while (true) {
        const int BAR_LENGTH = 10;
        const int LENGTH = 130;

        if (loop > (LENGTH + BAR_LENGTH) * 2) {
            loop = 0;
            clear();
        }
        if (GetKeyState(VK_RETURN) & 0x8000) {
            break;
        }

        rewrite();
        vector<string> data;
        data.push_back(drawLine(LENGTH - loop, LENGTH, BAR_LENGTH));
        concatVector(data, getFile("intro"));
        data.push_back(drawLine(loop, LENGTH, BAR_LENGTH));
        printString(data, 1);
        loop++;

        Sleep(1);
    }
}

void mainAimation() {
    clearScreen();
    rewrite();
    printString(getFile("3"), 1);
    Sleep(1000);
    clearScreen();
    rewrite();
    printString(getFile("2"), 1);
    Sleep(1000);
    clearScreen();
    rewrite();
    printString(getFile("1"), 1);
    Sleep(1000);
}


// initialize
void initScreen() {
    // window size
    HWND wh = GetConsoleHandle();
    MoveWindow(wh, 0, 0, 1920, 1080, 1);
    cout << "\n";

    Sleep(100);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    screen = (char*)malloc(columns * rows * sizeof(char)); // 208 * 53
}

int main() {
    initScreen();
    introAnimation();
    mainAimation();
}