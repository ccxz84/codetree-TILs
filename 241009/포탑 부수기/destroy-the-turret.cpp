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

#define MAX_M 10
#define MAX_N 10

struct Turret {
    int tick = 0, attack = 0;
    int x = 0, y = 0;
    bool lastAttack = false;

    bool operator < (Turret b) const {
        if (attack != b.attack) return attack > b.attack;
        if (tick != b.tick) return tick < b.tick;
        if (x + y != b.x + b.y) return x + y < b.x + b.y;
        return y < b.y;
    }

    bool operator == (Turret b) const {
        return x == b.x && y == b.y;
    }

    bool operator != (Turret b) const {
        return !(x == b.x && y == b.y);
    }
};

Turret matrix[MAX_N][MAX_M];

int n, m, k;

Turret attacker;
Turret target;

int tick;

void debug() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            debugFile << matrix[i][j].attack << ' ';
        }
        debugFile << '\n';
    }
    debugFile << '\n';
}

// 모든 위치에는 포탑 존재
// 포탑에는 공격력 존재.
// 상황에 따라 늘어나거나 줄어듬.
// 공격력 0이 되면 부서짐. 공격 못함.
// 최초에 부서진 포탑이 존재할 수 있음.

void initTurret() {
    *input >> n >> m >> k;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            Turret newTurret;

            *input >> newTurret.attack;
            newTurret.x = i;
            newTurret.y = j;

            matrix[i][j] = newTurret;
        }
    }
}


// 공격자 선정
// 부서지지 않은 포탑 중 가장 약한 포탑 선정
// 선정된 포탑은 n + m만큼 공격력 증가.

// 선정 기준
// 1. 공격력이 가장 낮음
// 2. 가장 최근에 공격함.
// 3. 행과 열의 합이 가장 큰 포탑
// 4. 열 값이 가장 큰 포탑


void setAttacker() {
    attacker = { 0, INT_MAX, 0, 0 };
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (matrix[i][j].attack > 0 && attacker < matrix[i][j]) {
                attacker = matrix[i][j];
            }
        }
    }
    attacker.attack += n + m;
    matrix[attacker.x][attacker.y].attack += n + m;
    matrix[attacker.x][attacker.y].lastAttack = true;
    matrix[attacker.x][attacker.y].tick = tick;
}

// 공격대상 선정
// 자신을 제외한 가장 강한 포탑 선정.
// 1. 공격력이 가장 높음
// 2. 공격한지 가장 오래된 포탑
// 3. 행과 열의 합이 가장 작은 포탑
// 4. 열 값이 가장 작은 포탑.

bool setTarget() {
    target = { INT_MAX, 0, 9, 9 };
    Turret cur = { INT_MAX, 0, 9, 9 };
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (matrix[i][j].attack > 0 && matrix[i][j] < target && attacker != matrix[i][j]) {
                target = matrix[i][j];
            }
        }
    }

    if (target == cur) {
        return false;
    }
    return true;
}

// 공격 방법 (레이저 -> 포탄)
// 1. 레이저 공격
// 상하좌우 4개 방향으로 이동가능
// 부서진 포탑이 잇는 위치는 지날 수 없음
// 격자 끝이면 반대 방향으로 나옴
// 최단 경로가 2개 이상이면 우 하 좌 상 순서로 움직인 경로를 선택
// 대상은 공격력 만큼, 경로에 있는 포탑은 공격력 / 2 몫 만큼 피해 입음.

int dirX[4] = { 0, 1, 0, -1 };
int dirY[4] = { 1, 0, -1, 0 };

bool BFS() {
    queue<Turret> qu;
    bool found = false;

    vector<vector<bool>> visited(n, vector<bool>(m, 0));
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(m, { -1, -1 }));

    qu.push(attacker);

    while (!qu.empty()) {
        auto cur = qu.front();
        qu.pop();

        if (cur == target) {
            found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = (cur.x + dirX[i] + n) % n, ny = (cur.y + dirY[i] + m) % m;

            if (matrix[nx][ny].attack > 0 && !visited[nx][ny]) {
                visited[nx][ny] = true;
                qu.push(matrix[nx][ny]);
                parent[nx][ny] = { cur.x, cur.y };
            }
        }
    }
    stack<pair<int, int>> path;
    if (found) {
        
        int cx = target.x, cy = target.y;

        while (!(cx == attacker.x && cy == attacker.y)) {
            path.push({ cx, cy });
            pair<int, int> p = parent[cx][cy];
            cx = p.first;
            cy = p.second;
        }

        while (!path.empty()) {
            pair<int, int> step = path.top();
            path.pop();
            if (!(attacker.x == step.first && attacker.y == step.second)) {
                if (step.first == target.x && step.second == target.y) {
                    matrix[step.first][step.second].lastAttack = true;
                    matrix[step.first][step.second].attack -= attacker.attack;
                    //matrix[step.first][step.second].attack = (matrix[step.first][step.second].attack - attacker.attack) < 0 ? 0 : (matrix[step.first][step.second].attack - attacker.attack);
                }
                else {
                    matrix[step.first][step.second].lastAttack = true;
                    matrix[step.first][step.second].attack -= attacker.attack / 2;
                    //matrix[step.first][step.second].attack = (matrix[step.first][step.second].attack - attacker.attack / 2) < 0 ? 0 : (matrix[step.first][step.second].attack - attacker.attack / 2);
                }
            }
        }

        return true;
    }

    return false;
}

// 2. 포탄 공격
// 공격 대상에게 포탄을 던짐
// 공격 대상은 공격력 만큼 피해를 입음.
// 공격 대상 주위 8칸 포탑도 피해입음. 공격력 / 2 몫
// 공격자는 피해입지 않음.

void bomb() {
    int bombX[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 }, bombY[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };

    for (int i = 0; i < 9; ++i) {
        int nx = (target.x + bombX[i] + n) % n, ny = (target.y + bombY[i] + m) % m;

        if (matrix[nx][ny].attack > 0 && matrix[nx][ny] != attacker) {
            if (matrix[nx][ny] == target) {
                matrix[nx][ny].lastAttack = true;
                //matrix[nx][step.second].attack = (matrix[step.first][step.second].attack - attacker.attack) < 0 ? 0 : (matrix[step.first][step.second].attack - attacker.attack);
                matrix[nx][ny].attack -= attacker.attack;
            }
            else {
                matrix[nx][ny].lastAttack = true;
                matrix[nx][ny].attack -= attacker.attack / 2;
            }
        }
    }
}

// 공격력 0이 되면 포탑 부서짐

// 공격에 무관했던 포탑은 공격력 1 증가.

void resetTurret() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (matrix[i][j].attack > 0 && !matrix[i][j].lastAttack) {
                ++matrix[i][j].attack;
            }
            else {
                matrix[i][j].lastAttack = false;
            }
        }
    }
}



int main() {
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    initTurret();

    for (int i = 0; i < k; ++i) {
        ++tick;
        setAttacker();
        if (!setTarget()) {
            break;
        }
        if (!BFS()) {
            bomb();
        }
        resetTurret();
        debug();
    }

    int ans = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (ans < matrix[i][j].attack) {
                ans = matrix[i][j].attack;
            }
        }
    }

    *output << ans << '\n';
}