#define _CRT_SECURE_NO_WARNINGS

#define BUFFSIZE 768
#define WIDTH 1280
#define HEIGHT 720

#define MUSIC_PATH "./resources/music/Offbeats.wav"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>

#include <tchar.h> 
#include <string.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm")


using namespace std;
using namespace std::chrono;


// global variables
char* SCREEN;
int COLUMNS, ROWS;
int WINDOWSIZE[2];

int SPOTPOS[2];

int SCORE;
int CANCLICK;

// global utils
HWND GetConsoleHandle() {
    HWND hwnd = NULL;
    char pszWindowTitle[BUFFSIZE];
    char pszNewTitle[BUFFSIZE];

    if (!GetConsoleTitleA(pszWindowTitle, sizeof(pszWindowTitle)))
        return NULL;
    snprintf(pszNewTitle, sizeof(pszNewTitle), "%s%lu%lu", pszWindowTitle, GetTickCount(), GetCurrentProcessId());
    if (!SetConsoleTitleA(pszNewTitle))
        return NULL;
    Sleep(50);
    hwnd = FindWindowA(NULL, pszNewTitle);
    SetConsoleTitleA(pszWindowTitle);
    return hwnd;
}

int* getWindowBorderSize() {
    HWND consoleWindow = GetConsoleWindow();
    RECT wrect;
    GetWindowRect(consoleWindow, &wrect);
    RECT crect;
    GetClientRect(consoleWindow, &crect);
    POINT lefttop = { crect.left, crect.top };
    ClientToScreen(consoleWindow, &lefttop);
    POINT rightbottom = { crect.right, crect.bottom };
    ClientToScreen(consoleWindow, &rightbottom);

    int left_border = lefttop.x - wrect.left;
    int right_border = wrect.right - rightbottom.x;
    int bottom_border = wrect.bottom - rightbottom.y;
    int top_border_with_title_bar = lefttop.y - wrect.top;

    int borders[4] = { top_border_with_title_bar, left_border, right_border, bottom_border };
    return borders;
}

int* getRealWindowSize() {
    int size[2];
    HWND consoleWindow = GetConsoleWindow();
    int* borders = getWindowBorderSize();
    size[0] = WIDTH - borders[1] - borders[2];
    size[1] = HEIGHT - borders[0] - borders[3];
    return size;
}

POINT getMouseClickPosition() {
    POINT p;
    if (GetCursorPos(&p)) {
        RECT rect;
        if (ScreenToClient(GetConsoleWindow(), &p)) {
            return p;
        }
    }
    p.x = -1;
    p.y = -1;
    return p;
}

void getPosByPersentage(int x, int y, int* posisionTmp) {
    posisionTmp[0] = (int)((float)COLUMNS * ((float)x / (float)100)) + 1;
    posisionTmp[1] = (int)((float)ROWS * ((float)y / (float)100)) + 1;
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

time_point<high_resolution_clock> getCurrentTime() {
    return high_resolution_clock::now();;
}

vector<string> getNumber(char* number) {
    int len = strlen(number);
    vector<string> data;
    for (int i = 0; i < len; i++) {
        string filename = to_string(number[i] - '0');
        vector<string> file = getFile(filename);

        if (data.empty()) {
            for (int j = 0; j < file.size(); j++) {
                data.push_back(file.at(j));
            }
        }else {
            for (int j = 0; j < file.size(); j++) {
                data.at(j) += file.at(j);
            }
        }
    }

    return data;
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
    for (int i = 0; i < COLUMNS * ROWS; i++)
        SCREEN[i] = ' ';
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
        int space = (ROWS - data.size()) / 2;
        for (int j = 0; j < space; j++) printf("\n");
        for (int i = 0; i < data.size(); i++) {
            int pos = (COLUMNS - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }
    else {
        for (int i = 0; i < data.size(); i++) {
            int pos = (COLUMNS - data.at(i).length()) / 2;
            for (int i = 0; i < pos; i++) printf(" ");
            printf("%s\n", data.at(i).c_str());
        }
    }
}

char getPixel(int* pos) {
    return SCREEN[pos[1] * COLUMNS + pos[0]];
}

void setPixel(int* pixelPos, char c) {
    if (pixelPos[0] < 0 || pixelPos[0] >= COLUMNS || pixelPos[1] < 0 || pixelPos[1] >= ROWS) return;
    SCREEN[pixelPos[1] * COLUMNS + pixelPos[0]] = c;
}


void render() {
    rewrite();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++)
            printf("%c", SCREEN[i * COLUMNS + j]);
        if (i != ROWS - 1)
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

// main actions
void playMusic() {
    PlaySound(TEXT(MUSIC_PATH), NULL, SND_ASYNC);
}
void stopMusic() {
    PlaySound(NULL, 0, SND_ASYNC);  // 현재 재생 중인 소리 중지
}

void drawCircle(int* postion, char c) {
    setPixel(postion, c);

    int posTmp[2];

    // 1
    posTmp[0] = postion[0] + 1;
    posTmp[1] = postion[1];
    setPixel(posTmp, c);
    posTmp[0] = postion[0] - 1;
    setPixel(posTmp, c);
    posTmp[0] = postion[0];
    posTmp[1] = postion[1] + 1;
    setPixel(posTmp, c);
    posTmp[1] = postion[1] - 1;
    setPixel(posTmp, c);

    // 2
    posTmp[0] = postion[0] + 2;
    posTmp[1] = postion[1];
    setPixel(posTmp, c);
    posTmp[0] = postion[0] - 2;
    setPixel(posTmp, c);
    posTmp[0] = postion[0] + 1;
    posTmp[1] = postion[1] + 1;
    setPixel(posTmp, c);
    posTmp[1] = postion[1] - 1;
    setPixel(posTmp, c);
    posTmp[0] = postion[0] - 1;
    setPixel(posTmp, c);
    posTmp[1] = postion[1] + 1;
    setPixel(posTmp, c);
}

vector<long> scriptLoader() {
    vector<string> script = getFile("script");

    vector<long> timestamps;
    for (int i = 0;i < script.size();i++) {
        timestamps.push_back(stol(script.at(i)));
    }
    return timestamps;
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
        if (GetKeyState(VK_SPACE) & 0x8000) {
            break;
        }

        rewrite();
        vector<string> data;
        data.push_back(drawLine((LENGTH + BAR_LENGTH) - loop, LENGTH, BAR_LENGTH));
        concatVector(data, getFile("intro"));
        data.push_back(drawLine(loop, LENGTH, BAR_LENGTH));
        printString(data, 1);
        loop++;

        Sleep(1);
    }
}

void countdownAimation() {
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

void mainAimation() {
    clearScreen();

    int loop = 0;
    int nextSpotPos[2];

    CANCLICK = 1;
    vector<long> script = scriptLoader();
    getPosByPersentage(rand() % 101, rand() % 101, nextSpotPos);

    time_point<high_resolution_clock> start = getCurrentTime();
    playMusic();
    while (true) {
        int renderFlag = 0;
        time_point<high_resolution_clock> now = getCurrentTime();
        long estimatedTimeMillis = duration_cast<milliseconds>(now - start).count();

        if (GetKeyState(VK_SPACE) & 0x8000) {
            break;
        }

        if (GetKeyState(VK_LBUTTON) & 0x8000) {
            POINT pointer = getMouseClickPosition();
            if (pointer.x != -1 && pointer.y != -1) {
                int pos[2];
                getPosByPersentage((int)(((float)pointer.x * (float)100) / (float)WINDOWSIZE[0]), (int)(((float)pointer.y * (float)100) / (float)WINDOWSIZE[1]), pos);

                if (getPixel(pos) == '#') {
                    drawCircle(SPOTPOS, ' ');
                    SCORE += 3000 - (estimatedTimeMillis - script.at(loop - 1));
                    CANCLICK = 0;
                }
                setPixel(pos, '+');

                renderFlag = 1;
            }
        }

        if (loop < script.size() && estimatedTimeMillis >= script.at(loop)) {
            clear();
            SPOTPOS[0] = nextSpotPos[0];
            SPOTPOS[1] = nextSpotPos[1];
            getPosByPersentage(rand() % 81 + 10, rand() % 81 + 10, nextSpotPos);
            drawCircle(SPOTPOS, '#');

            if (loop + 1 < script.size()) {
                drawCircle(nextSpotPos, '-');
            }

            loop++;
            renderFlag = 1;
            CANCLICK = 1;
        }


        if (renderFlag) {
            render();
        }

        if (loop == script.size() && (estimatedTimeMillis - script.at(loop - 1)) > 2000) {
            stopMusic();
            break;
        }
    }
}


// initialize
void initScreen() {
    // window size
    HWND wh = GetConsoleHandle();
    MoveWindow(wh, 0, 0, WIDTH, HEIGHT, 1);
    cout << "\n";

    Sleep(100);

    // get console size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COLUMNS = csbi.srWindow.Right - csbi.srWindow.Left;
    ROWS = csbi.srWindow.Bottom - csbi.srWindow.Top;

    // hide cursor
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);

    // disable selecting
    DWORD prev_mode;
    GetConsoleMode(consoleHandle, &prev_mode);
    SetConsoleMode(consoleHandle, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));

    // get window size
    WINDOWSIZE[0] = getRealWindowSize()[0] - 10;
    WINDOWSIZE[1] = getRealWindowSize()[1] - 10;

    // screen buffer
    SCREEN = (char*)malloc(COLUMNS * ROWS * sizeof(char)); // 208 * 53

    // clear screen buffer
    clear();
    clearScreen();
}

int main() {
    srand(time(NULL));

    initScreen();

    introAnimation();
    countdownAimation();

    mainAimation();

    clearScreen();
    rewrite();
    printString(getNumber(const_cast<char*>(to_string(SCORE).c_str())), 1);

    while (1) {
        if (GetKeyState(VK_SPACE) & 0x8000) {
            break;
        }
        Sleep(10);
    }


    return 0;
}