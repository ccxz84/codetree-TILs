#include <iostream>
#include <fstream>
#include <queue>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define MAX_N 20
#define MAX_M 31

int n, m, k;

int dirX[4] = { -1, 0, 1, 0 };
int dirY[4] = { 0, 1, 0, -1 };

int score[MAX_M];
int ability[MAX_M];
int playerGun[MAX_M];
int posX[MAX_M], posY[MAX_M];
int dir[MAX_M];
priority_queue<int, vector<int>, less<int>> matrix[MAX_N][MAX_N];
int playerMatrix[MAX_N][MAX_N];

// n*n 격자에서 진행
// 각각 격자에는 무기가 있을 수 있음.
// 초기에는 무기 없는 빈 격자에 플레이어가 위치
// 플레이어의 초기 능력치는 모두 다름

// 각 총은 공격력과 플레이어는 초기 능력치를 가짐
// 플레이어는 플레이어 번호가 있음

void initGame() {
    *input >> n >> m >> k;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int gun;

            *input >> gun;

            if (gun != 0) {
                matrix[i][j].push(gun);
            }
        }
    }

    for (int i = 1; i <= m; ++i) {
        *input >> posX[i] >> posY[i] >> dir[i] >> ability[i];
        --posX[i];
        --posY[i];
        playerMatrix[posX[i]][posY[i]] = i;
    }
}

void getGun(int idx);
bool battle(int a, int b);
void losePlayer(int idx);
void winPlayer(int idx, int _score);

// 라운드 규칙
// 첫 플레이어부터 순차적으로 본인이 향하는 방향대로 한 칸 이동
// 격자를 벗어나는 경우 정 반대로 방향을 바꿔서 한 칸 이동

void movePlayer(int idx) {
    int x, y;
    x = posX[idx] + dirX[dir[idx]];
    y = posY[idx] + dirY[dir[idx]];

    if (x >= n || y >= n || x < 0 || y < 0) {
        dir[idx] = (dir[idx] + 2) % 4;

        posX[idx] += dirX[dir[idx]];
        posY[idx] += dirY[dir[idx]];
    }
    else {
        posX[idx] = x;
        posY[idx] = y;
    }
}

void runRound() {
    for (int i = 1; i <= m; ++i) {
        playerMatrix[posX[i]][posY[i]] = 0;
        movePlayer(i);

        if (playerMatrix[posX[i]][posY[i]] != 0) {
            int target = playerMatrix[posX[i]][posY[i]];
            int _score = abs(ability[i] + playerGun[i] - ability[target] - playerGun[target]);
            if (battle(i, target)) {
                losePlayer(target);
                winPlayer(i, _score);
            }
            else {
                losePlayer(i);
                winPlayer(target, _score);
            }
        }
        else {
            getGun(i);
            playerMatrix[posX[i]][posY[i]] = i;
        }
    }
}

// 적이 없는경우
// 이동한 방향에 플레이어가 없다면 총이 있는지 확인 총이 있는 경우 총 획득
// 플레이어가 이미 총이 있으면 가장 센 총을 선택하고 바닥에 내려놓음 (바닥에 총이 여러개 있을 수 있음)
// 이동한 방향에 플레이어가 있으면 싸움 발생.

void getGun(int idx) {
    int x = posX[idx], y = posY[idx];

    if (!matrix[x][y].empty()) {
        int gun = matrix[x][y].top();

        if (gun > playerGun[idx]) {
            matrix[x][y].pop();
            matrix[x][y].push(playerGun[idx]);
            playerGun[idx] = gun;
        }
    }
}

// 싸움
// 초기 능력 + 총 능력을 비교하여 큰 플레이어가 이김
// 수치가 같다면 초기 능력이 높은 플레이어가 이김
// 이긴 플레이어는 (초기 능력 + 총 능력)의 차이 만큼 점수 획득

bool battle(int a, int b) {
    if (ability[a] + playerGun[a] != ability[b] + playerGun[b]) return ability[a] + playerGun[a] > ability[b] + playerGun[b];
    return ability[a] > ability[b];
}

// 진 플레이어는 총을 격자에 내려놓음.
// 원래 가던 방향으로 한 칸 이동.
// 다른 플레이어가 있거나, 범위 밖이면 오른쪽으로 90도씩 회전
// 총 획득은 기존 이동과 같이 진행됨

void losePlayer(int idx) {
    if (playerGun[idx] != 0) matrix[posX[idx]][posY[idx]].push(playerGun[idx]);
    playerGun[idx] = 0;

    for (int i = 0; i < 4; ++i) {
        int x = posX[idx], y = posY[idx], d = (dir[idx] + i) % 4;

        x += dirX[d]; y += dirY[d];

        if (!(playerMatrix[x][y] || x >= n || y >= n || x < 0 || y < 0)) {
            playerMatrix[x][y] = idx;
            posX[idx] = x;
            posY[idx] = y;
            dir[idx] = d;
            getGun(idx);
            break;
        }
    }
}

// 이긴 플레이어는 승리한 칸에 떨어져 있는 가장 높은 총을 획득

void winPlayer(int idx, int _score) {
    playerMatrix[posX[idx]][posY[idx]] = idx;

    getGun(idx);
    score[idx] += _score;
}

// 각 플레이어가 얻는 점수를 출력

int main() {
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    initGame();

    for (int i = 0; i < k; ++i) {
        runRound();
    }

    for (int i = 1; i <= m; ++i) {
        *output << score[i] << ' ';
    }
    *output << '\n';

    return 0;
}