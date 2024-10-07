#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define MAX_N 15
#define MAX_M 30

int n, m;

void movePeople(int idx);
void checkPeople(int idx);
void joinPerson(int idx);

int matrix[MAX_N][MAX_N]; // 0이면 이용가능, 1이면 이용 불가능
int posX[MAX_M], posY[MAX_M];
int conX[MAX_M], conY[MAX_M];
bool join[MAX_M];

int dirX[4] = { -1, 0, 0, 1 };
int dirY[4] = { 0, -1, 1, 0 };

int step[MAX_N][MAX_N];      // 최단거리 결과 기록
bool visited[MAX_N][MAX_N];  // 방문 여부 표시

int tick;

inline bool InRange(int x, int y) {
    return 0 <= x && x < n && 0 <= y && y < n;
}

// (x, y)로 이동이 가능한지 판단합니다.
inline bool CanGo(int x, int y) {
    return InRange(x, y) &&       // 범위를 벗어나지 않으면서 
        !visited[x][y] &&      // 방문했던 적이 없으면서
        matrix[x][y] != 2;       // 이동 가능한 곳이어야 합니다.
}

// 최단거리로 움직이며 최단 거리인 방향이 여러 개면 (상, 좌, 우, 하) 순으로 움직임
// 최단거리는 상하좌우 인접한 칸 중 이동가능한 칸으로만 이동하여 도달하기 까지 거쳐야 되는 칸이 가장 적인 경로
void BFS(int srcX, int srcY, int dstX, int dstY) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            visited[i][j] = false;
            step[i][j] = 0;
        }
    }

    queue<pair<int, int>> qu;
    qu.push({ srcX, srcY });

    visited[srcX][srcY] = true;
    step[srcX][srcY] = 0;

    while (!qu.empty()) {
        auto cur = qu.front();
        qu.pop();

        int x = cur.first, y = cur.second;

        for (int i = 0; i < 4; ++i) {
            int nx = cur.first + dirX[i], ny = cur.second + dirY[i];

            if (CanGo(nx, ny)) {
                visited[nx][ny] = true;
                step[nx][ny] = step[x][y] + 1;
                qu.push({ nx, ny });
            }
        }
    }
}

// 빵을 구하는 사람들의 동작은 다음 순서로 구성됨

// 본인이 가고 싶은 편의점 방향으로 1칸 움직임.

void movePeople(int idx) {
    if (posX[idx] == conX[idx] && posY[idx] == conY[idx]) {
        return;
    }

    if (!join[idx]) {
        return;
    }

    BFS(conX[idx], conY[idx], posX[idx], posY[idx]);

    int minDist = INT_MAX;
    int minX = -1, minY = -1;

    for (int i = 0; i < 4; ++i) {
        int nx = posX[idx] + dirX[i], ny = posY[idx] + dirY[i];

        if (InRange(nx, ny) && visited[nx][ny] && minDist > step[nx][ny]) {
            minDist = step[nx][ny];
            minX = nx; minY = ny;
        }
    }

    posX[idx] = minX; posY[idx] = minY;
}

// 편의점에 도착한 경우
// 해당 편의점에서 멈추고, 그 칸은 이동할 수 없는 칸이 됨.
// 격자에 있는 사람이 모두 이동한 후에 봉인됨.
void checkPeople(int idx) {
    if (posX[idx] == conX[idx] && posY[idx] == conY[idx]) {
        matrix[posX[idx]][posY[idx]] = 2;
    }
}

// 현재 시간이 t분이고 t <= m을 만족하면, t번 사람은 자신이 가고 싶은 편의점과 가까이 있는 베이스 캠프에 들어감.
// 가장 가까운 베이스 캠프가 여러 개인 경우 행, 열 순으로 작은 베이스 캠프에 들어감
// 이때 부터 베이스 캠프 칸은 비활성화 됨.
// 모든 사람이 이동한 후 비활성화 됨.
void joinPerson(int idx) {
    BFS(conX[idx], conY[idx], posX[idx], posY[idx]);

    int minDist = INT_MAX;
    int minX = -1, minY = -1;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (visited[i][j] && matrix[i][j] == 1 && minDist > step[i][j]) {
                minDist = step[i][j];
                minX = i; minY = j;
            }
        }
    }

    posX[idx] = minX; posY[idx] = minY;
    matrix[minX][minY] = 2;
    join[idx] = true;
}

bool checkEnd() {
    for (int i = 0; i < m; ++i) {
        if (posX[i] != conX[i] || posY[i] != conY[i]) {
            return false;
        }
    }

    return true;
}

void solution() {
    while (true) {
        ++tick;

        for (int i = 0; i < m; ++i) {
            if (!join[i]) {
                continue;
            }

            movePeople(i);
        }
        for (int i = 0; i < m; ++i) {
            if (!join[i]) {
                continue;
            }

            checkPeople(i);
        }

        if (tick <= m) {
            joinPerson(tick - 1);
        }

        if (checkEnd()) break;
    }
}

int main() {
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    *input >> n >> m;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            *input >> matrix[i][j];
        }
    }

    for (int i = 0; i < m; ++i) {
        *input >> conX[i] >> conY[i];
        --conX[i];
        --conY[i];
    }

    solution();

    *output << tick << '\n';

    return 0;
}