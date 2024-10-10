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

#define MAX_N 50
#define MAX_P 30

int n, p, tick;

int sanMatrix[MAX_N][MAX_N];

int santaX[MAX_P], santaY[MAX_P], stun[MAX_P], santaPower, score[MAX_P];
int rudolX, rudolY, rudolPower;
int aliveSanta;

bool out[MAX_P];

inline int getDistance(int x1, int y1, int x2, int y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

int rudolDirX[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
int rudolDirY[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };

inline bool isVaild(int x, int y) {
    return x >= 0 && x < n && y >= 0 && y < n;
}

inline bool compare(int x1, int y1, int dis1, int x2, int y2, int dis2) {
    if (dis1 != dis2) return dis1 < dis2;
    if (x1 != x2) return x1 > x2;
    return y1 > y2;
}

inline void changePosSanta(int idx, int x, int y) {
    sanMatrix[santaX[idx]][santaY[idx]] = 0;
    santaX[idx] = x;
    santaY[idx] = y;
    sanMatrix[santaX[idx]][santaY[idx]] = idx + 1;
}

inline void outSanta(int idx) {
    sanMatrix[santaX[idx]][santaY[idx]] = 0;
    out[idx] = true;
    --aliveSanta;
}

void collision(int idx, int dx, int dy, int power) {
    // dx, dy 와 power 만큼 밀려난 좌표 구하기
    int nx = santaX[idx] + dx * power, ny = santaY[idx] + dy * power;

    // 해당 좌표가 유효한가?
    if (isVaild(nx, ny)) {

        // 다른 산타가 있다면
        if (sanMatrix[nx][ny] != 0) {
            // 재귀로 다른 산타 충돌 처리, 1씩 밀려남
            collision(sanMatrix[nx][ny] - 1, dx, dy, 1);
        }
        // 모든 충돌 처리가 끝나면 산타 위치 변경
        changePosSanta(idx, nx, ny);
    }
    else {
        // 유효하지 않은 좌표면 아웃처리
        outSanta(idx);
    }
}

void moveRudolf() {
    int minDis = INT_MAX, x, y;
    int sanX = 0, sanY = 0, sanIdx, minDir;

    //// 가장 가까우면서 우선순위 높은 산타 구하기
    //for (int i = 0; i < 8; ++i) {
    //    int nx = rudolX + rudolDirX[i], ny = rudolY + rudolDirY[i];
    //    
    //    // 이동할 위치가 유효한가?
    //    if (isVaild(nx, ny)) {

    //        // 모든 산타를 뒤져 거리체크
    //        for (int j = 0; j < p; ++j) {

    //            // 아웃 당한 산타는 건너뛰기
    //            if (out[j]) continue;

    //            int dis = getDistance(santaX[j], santaY[j], nx, ny);

    //            // 거리가 짧거나, x가 크고, y가 큰 순으로 우선순위
    //            if (compare(santaX[j], santaY[j], dis, sanX, sanY, minDis)) {
    //                minDis = dis;
    //                x = nx; y = ny;
    //                sanX = santaX[j]; sanY = santaY[j];
    //                sanIdx = j;
    //                minDir = i;
    //            }
    //        }
    //    }
    //}

    for (int j = 0; j < p; ++j) {
        // 아웃 당한 산타는 건너뛰기
        if (out[j]) continue;

        int dis = getDistance(santaX[j], santaY[j], rudolX, rudolY);

        // 거리가 짧거나, x가 크고, y가 큰 순으로 우선순위
        if (compare(santaX[j], santaY[j], dis, sanX, sanY, minDis)) {
            minDis = dis;
            sanX = santaX[j]; sanY = santaY[j];
            sanIdx = j;
        }
    }

    minDis = INT_MAX;

    for (int i = 0; i < 8; ++i) {
        int nx = rudolX + rudolDirX[i], ny = rudolY + rudolDirY[i];

        int dis = getDistance(sanX, sanY, nx, ny);

        if (dis < minDis) {
            minDis = dis;
            x = nx; y = ny;
            minDir = i;
        }
    }

    // 목표 산타 구하고 위치 후보 선정 완료
    rudolX = x;
    rudolY = y;

    if (x == sanX && y == sanY) {
        collision(sanIdx, rudolDirX[minDir], rudolDirY[minDir], rudolPower);
        // 다음 턴 까지 기절
        stun[sanIdx] = tick + 1;
        // c만큼 점수 획득
        score[sanIdx] += rudolPower;
    }
}

int sanDirX[4] = { -1, 0, 1, 0 };
int sanDirY[4] = { 0, 1, 0, -1 };

void moveSanta() {
    for (int i = 0; i < p; ++i) {
        if (out[i]) continue;

        if (tick <= stun[i]) {
            continue;
        }

        int minDis = getDistance(santaX[i], santaY[i], rudolX, rudolY), x = santaX[i], y = santaY[i], dir;

        for (int j = 0; j < 4; ++j) {
            int nx = santaX[i] + sanDirX[j], ny = santaY[i] + sanDirY[j];

            // 이동 위치가 격자 안이고 ,다른 산타가 없다면.
            if (isVaild(nx, ny) && sanMatrix[nx][ny] == 0) {
                // 루돌프와 거리 구하고
                int dis = getDistance(nx, ny, rudolX, rudolY);

                // 거리가 작으면 업데이트
                if (dis < minDis) {
                    x = nx; y = ny;
                    minDis = dis;
                    dir = j;
                }
            }
        }

        // 산타가 움직일 수 있는 경우에는
        if (x != santaX[i] || y != santaY[i]) {
            // 위치 업데이트
            changePosSanta(i, x, y);
        }

        // 루돌프와 충돌했다고 판단되면
        if (santaX[i] == rudolX && santaY[i] == rudolY) {
            // 충돌 작업 수행
            collision(i, sanDirX[dir] * -1, sanDirY[dir] * -1, santaPower);
            // 다음 턴 까지 기절
            stun[i] = tick + 1;
            // D 만큼 점수 획득
            score[i] += santaPower;
        }
    }
}

int m;

void init() {
    *input >> n >> m >> p >> rudolPower >> santaPower;

    *input >> rudolX >> rudolY;
    --rudolX; --rudolY;

    for (int i = 0; i < p; ++i) {
        int idx;
        *input >> idx;
        --idx;

        *input >> santaX[idx] >> santaY[idx];
        --santaX[idx]; --santaY[idx];
        sanMatrix[santaX[idx]][santaY[idx]] = idx + 1;
    }

    aliveSanta = p;
}

void print() {
    vector<vector<int>> mat(n, vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (rudolX == i && rudolY == j) {
                debugFile << -1 << ' ';
            }
            else {
                debugFile << sanMatrix[i][j] << ' ';
            }
            
        }
        debugFile << '\n';
    }
    debugFile << '\n';

    for (int i = 0; i < p; ++i) {
        debugFile << score[i] << ' ';
    }
    debugFile << '\n';
    debugFile << '\n';
}

void solution() {
    print();
    for (int i = 0; i < m; ++i) {
        ++tick;
        debugFile << "tick: " << tick << '\n';
        moveRudolf();
        if (aliveSanta <= 0) {
            break;
        }
        debugFile << "rudol: " << tick << '\n';
        print();
        moveSanta();
        if (aliveSanta <= 0) {
            break;
        }
        debugFile << "santa: " << tick << '\n';
        print();

        for (int j = 0; j < p; ++j) {
            if (!out[j]) {
                ++score[j];
            }
        }
        debugFile << "alive bonus: " << tick << '\n';
        print();
    }

    for (int i = 0; i < p; ++i) {
        *output << score[i] << ' ';
    }
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