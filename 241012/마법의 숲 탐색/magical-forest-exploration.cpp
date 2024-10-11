#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
#include <stack>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define MAX_R 70
#define MAX_C 70
#define MAX_K 1000

int k, r, c;
vector<vector<int>> matrix;
int score;

int golemExtraction[MAX_K + 1];
pair<int, int> golemCenter[MAX_K + 1];

inline bool isValid(int x, int y) {
    return x >= 0 && x < r+3 && y >= 0 && y < c;
}

int dirGolemX[5] = { 0, -1, 1, 0, 0 }, dirGolemY[5] = { 0, 0, 0, -1, 1 };

bool moveGolemDown(int x, int y, vector<vector<int>>& mat, int& extraction, int num) {
    // 남쪽으로 한 칸
    // 중심을 기준으로 왼쪽, 아래, 오른쪽 칸이 이동가능한지 확인
    

    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i], ny = y + dirGolemY[i];

        if (isValid(nx, ny)) {
            mat[nx][ny] = 0;
        }
    }

    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i] + 1, ny = y + dirGolemY[i];
        if (isValid(nx, ny) && mat[nx][ny] == 0) {
            mat[nx][ny] = num;
        }
        else if (nx != -1) {
            return false;
        }
    }

    return true;
}

bool rotateGolemLeft(int x, int y, vector<vector<int>>& mat, int& extraction, int num) {
    // 좌로 한 칸 및 회전
    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i], ny = y + dirGolemY[i];

        if (isValid(nx, ny)) {
            mat[nx][ny] = 0;
        }
    }

    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i], ny = y + dirGolemY[i] - 1;
        if (isValid(nx, ny) && mat[nx][ny] == 0) {
            mat[nx][ny] = num;
        }
        else if(i != 1 || nx != -1) {
            return false;
        }
    }

    extraction = (extraction - 1) < 0 ? 4 - abs((extraction - 1) % 4) : (extraction - 1) % 4;

    if (moveGolemDown(x, y - 1, mat, extraction, num)) {
        return true;
    }
    return false;
}

bool rotateGolemRight(int x, int y, vector<vector<int>>& mat, int& extraction, int num) {
    // 우로 한 칸 및 회전
    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i], ny = y + dirGolemY[i];

        if (isValid(nx, ny)) {
            mat[nx][ny] = 0;
        }
    }

    for (int i = 0; i < 5; ++i) {
        int nx = x + dirGolemX[i], ny = y + dirGolemY[i] + 1;
        if (isValid(nx, ny) && mat[nx][ny] == 0) {
            mat[nx][ny] = num;
        }
        else if (i != 1 || nx != -1) {
            return false;
        }
    }

    extraction = (extraction +1) % 4;

    if (moveGolemDown(x, y + 1, mat, extraction, num)) {
        return true;
    }
    return false;
}

bool inRange(int y, int x) {
    return 3 <= y && y < r + 3 && 0 <= x && x < c;
}

bool rotateGolem(int x, int y, int num) {
    int count = 0;
    // 골렘이 최대한 남쪽으로 내려 가게 이동시킴.
    while (true) {
        vector<vector<int>> mat = matrix;
        int extraction = golemExtraction[num];
        if (moveGolemDown(x, y, mat, extraction, num)) {
            matrix = mat;
            golemExtraction[num] = extraction;
            ++x;
            golemCenter[num] = { x, y };
            continue;
        }

        mat = matrix;
        extraction = golemExtraction[num];
        if (rotateGolemLeft(x, y, mat, extraction, num)) {
            matrix = mat;
            golemExtraction[num] = extraction;
            ++x; --y;
            golemCenter[num] = { x, y };
            continue;
        }

        mat = matrix;
        extraction = golemExtraction[num];
        if (rotateGolemRight(x, y, mat, extraction, num)) {
            matrix = mat;
            golemExtraction[num] = extraction;
            ++x; ++y;
            golemCenter[num] = { x, y };
            continue;
        }

        break;
    }

    if (!inRange(x, y)) {
        return false;
    }

    return true;
}

int dirX[4] = { -1, 1, 0, 0 };
int dirY[4] = { 0, 0, -1, 1 };

int extractionDx[4] = { -1, 0, 1, 0 }, extractionDy[4] = { 0, 1, 0, -1 };

void moveSpirit(int x, int y) {
    // 출구를 타고, 인접한 다른 정령으로 옮겨탈 수 있음.
    queue<pair<int, int>> qu;
    qu.push({ x, y });

    int maxRow = 0;

    int dirX[4] = { -1, 1, 0, 0 }, dirY[4] = { 0, 0, -1, 1 };

    vector<bool> visited(k+1, 0);

    while (!qu.empty()) {
        auto pos = qu.front();
        qu.pop();

        if (pos.first + 1 > maxRow) {
            maxRow = pos.first + 1;
        }
        int num = matrix[pos.first][pos.second];
        int dir = golemExtraction[num];

        for (int i = 0; i < 4; ++i) {
            int nx = pos.first + extractionDx[dir] + dirX[i], ny = pos.second + extractionDy[dir] + dirY[i];

            if (isValid(nx, ny) && matrix[nx][ny] != 0 && matrix[nx][ny] != num && !visited[matrix[nx][ny]]) {
                qu.push(golemCenter[matrix[nx][ny]]);
                visited[matrix[nx][ny]] = true;
            }
        }
    }

    
    if (maxRow != 0) {
        score += maxRow + 1 - 3;
        debugFile << maxRow + 1 - 3 << "\n\n";
    }
}

void print() {
    for (int i = 0; i < r+3; ++i) {
        for (int j = 0; j < c; ++j) {
            debugFile << matrix[i][j] << ' ';
        }
        debugFile << '\n';
    }
    debugFile << '\n';
}

void resetBoard() {
    matrix = vector<vector<int>>(r + 3, vector<int>(c, 0));
}

void solution() {
    for (int i = 1; i <= k; ++i) {
        int c, d;

        *input >> c >> d;
        golemExtraction[i] = d;
        golemCenter[i] = { 0, c - 1 };

        /*for (int i = 0; i < 5; ++i) {

        }*/

        if (!rotateGolem(0, c - 1, i)) {
            resetBoard();
            continue;
        }

        print();
        debugFile << "center: " << golemCenter[i].first << ' ' << golemCenter[i].second << '\n';
        debugFile << "extraction: " << golemCenter[i].first + extractionDx[golemExtraction[i]] << ' ' << golemCenter[i].second + extractionDy[golemExtraction[i]] << '\n';

        moveSpirit(golemCenter[i].first, golemCenter[i].second);
    }

    *output << score << '\n';
}

void init() {
    *input >> r >> c >> k;
    matrix = vector<vector<int>>(r + 3, vector<int>(c, 0));
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