#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
#include <stack>
#include <algorithm>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define MAX_N 10
#define MAX_M 10

// nxn 격자
// 1,1 이 원점

int n, m;
int extractionX, extractionY;
int moveDistance;


// 빈 칸: 이동 가능한 칸
// 벽 : 1~9의 내구도를 가짐. 회전하면 1 깎임. 0이 되면 빈칸과 동일
// 출구: 참가자가 도착하면 즉시 탈출

int matrix[MAX_N][MAX_N];

// 1초에 참가자는 한 번씩 움직임
// 두 위치의 최단 거리는 abs(x1 - x2 ) + abs(y1 - y2)

int posX[MAX_M], posY[MAX_M];
bool out[MAX_M];

inline bool isValid(int x, int y) {
    return x >= 0 && x < n && y >= 0 && y < n;
}

inline int getDistance(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// 상하좌우로 움직일 수 있고

int dirX[4] = { -1, 1, 0, 0 };
int dirY[4] = { 0, 0, -1, 1 };

// 벽이 없는 곳으로 이동가능
// 움직이는 곳은 출구와의 최단 거리가 가까워 지는 곳이어야 함.
// 움직일 수 있는 칸이 2개 이상이면, 상하로 움직이는게 우선됨.
// 움직일 수 없는 상황이면 가만히 있음.
// 참가자가 2명이상 있을 수 있음.

void movePeople() {
    for (int i = 0; i < m; ++i) {
        if (out[i]) continue;

        int x = posX[i], y = posY[i];
        int minDis = getDistance(x, y, extractionX, extractionY);

        for (int j = 0; j < 4; ++j) {
            int nx = posX[i] + dirX[j], ny = posY[i] + dirY[j];
            int dis = getDistance(nx, ny, extractionX, extractionY);
            if (isValid(nx, ny) && matrix[nx][ny] == 0 && dis < minDis) {
                x = nx; y = ny;
                minDis = dis;
            }
        }

        if (posX[i] != x || posY[i] != y) {
            ++moveDistance;
            posX[i] = x; posY[i] = y;
        }

        if (posX[i] == extractionX && posY[i] == extractionY) {
            out[i] = true;
        }
    }
}

// 미로 회전
// 한 명 이상의 참가자와 출구를 포함한 가장 작은 정사각형을 잡음.
// 선택한 정사각형은 시계 방향으로 90도 회전. 내구도 1 깎임.

void rotateMaze(int x, int y, int size) {
    int temp[MAX_N][MAX_N];
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            temp[j][size - 1 - i] = matrix[x + i][y + j];
        }
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[x + i][y + j] = temp[i][j];
            
            if (matrix[x + i][y + j] > 0) {
                --matrix[x + i][y + j];
            }
        }
    }
}

void rotateExtraction(int x, int y, int size) {
    int newExtractionX = extractionY - y;
    int newExtractionY = size - 1 - (extractionX - x);

    extractionX = x + newExtractionX;
    extractionY = y + newExtractionY;
}

void peopleRotate(int x, int y, int size) {
    for (int i = 0; i < m; ++i) {
        if (out[i]) continue;
        if (posX[i] >= x && posX[i] < x + size && posY[i] >= y && posY[i] < y + size) {
            // 그냥 좌표를 쓸 때는 좌 상단 꼭지점 기준으로 상대 좌표를 써야됨.
            int newPosX = posY[i] - y;             
            int newPosY = size - 1 - (posX[i] - x);
            posX[i] = x + newPosX;
            posY[i] = y + newPosY;
        }
    }
}

void getRec(int x1, int y1, int x2, int y2, int& recX, int& recY, int& size ) {
    size = max(abs(x1 - x2), abs(y1 - y2)) + 1;

    recX = (max(x1, x2) - size + 1 < 0) ? 0 : max(x1, x2) - size + 1;
    recY = (max(y1, y2) - size + 1 < 0) ? 0 : max(y1, y2) - size + 1;
}

bool compare(int x1, int y1, int size1, int x2, int y2, int size2) {
    if (size1 != size2) return size1 < size2;
    if (x1 != x2) return x1 < x2;
    return y1 < y2;
}

// 가장 작은 크기를 갖는 정사각형이 2개 이상이면, 좌상단 r 좌표가 작은 것이 우선. 같다면 c 좌표가 작은 것.
void findMinRec(int& minRecX, int& minRecY, int& recSize) {
    minRecX = INT_MAX; minRecY = INT_MAX; recSize = INT_MAX;

    for (int i = 0; i < m; ++i) {
        if (out[i]) continue;

        int recX, recY, size;
        getRec(posX[i], posY[i], extractionX, extractionY, recX, recY, size);

        if (compare(recX, recY, size, minRecX, minRecY, recSize)) {
            minRecX = recX; minRecY = recY;
            recSize = size;
        }
    }
}

int k;

void solution() {

    for (int i = 0; i < k; ++i) {
        movePeople();

        int minRecX, minRecY, size;

        findMinRec(minRecX, minRecY, size);
        rotateMaze(minRecX, minRecY, size);
        peopleRotate(minRecX, minRecY, size);
        rotateExtraction(minRecX, minRecY, size);

        bool allout = true;

        for (int i = 0; i < m; ++i) {
            if (!out[i]) {
                allout = false;
                break;
            }
        }

        if (allout) {
            break;
        }
    }
    *output << moveDistance << '\n';
    *output << extractionX+1 << ' ' << extractionY+1 << '\n';
}

void init() {
    *input >> n >> m >> k;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            *input >> matrix[i][j];
        }
    }

    for (int i = 0; i < m; ++i) {
        *input >> posX[i] >> posY[i];
        --posX[i]; --posY[i];
        out[i] = false;
    }

    *input >> extractionX >> extractionY;
    --extractionX; --extractionY;
}


int main() {
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    init();
    solution();
}