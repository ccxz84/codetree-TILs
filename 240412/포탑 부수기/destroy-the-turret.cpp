#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <queue>

using namespace std;

struct pos {
    int r;
    int c;
};

int n, m, k;
vector<vector<int>> board(10, vector<int>(10, 0));
vector<vector<int>> lastAttack(10, vector<int>(10, -1));
vector<vector<int>> lastDamage(10, vector<int>(10, -1));

int dirX[4] = {0, 1, 0, -1};
int dirY[4] = {1, 0, -1, 0};

int bomb_dirX[8] = {0, 1, 0, -1, 1, 1, -1, -1};
int bomb_dirY[8] = {1, 0, -1, 0, 1, -1, 1, -1};

void print_board() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
}

// 선정 기준
// 1. 공격력이 가장 낮은 포탑
// 2. 공격력이 같으면 최근에 공격한 포탑 (시점 0에서는 모두 공격)
// 3. 행과 열의 합이 가장 큰 포탑
// 4. 열 값이 가장 큰 포탑

struct pos find_attacker() {
    struct pos weakest = {-1, -1}; // 가장 약한 포탑의 위치를 초기화합니다.
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (board[i][j] <= 0) continue; // 포탑이 없는 경우 건너뜁니다.
            // 가장 약한 포탑을 찾는 조건들을 순서대로 적용합니다.
            if (weakest.r == -1 || // 아직 포탑을 찾지 못했거나,
                board[i][j] < board[weakest.r][weakest.c] || // 더 낮은 공격력을 가진 포탑을 찾았거나,
                (board[i][j] == board[weakest.r][weakest.c] && // 공격력이 같고 최근에 공격한 포탑을 찾았거나,
                 lastAttack[i][j] > lastAttack[weakest.r][weakest.c]) ||
                (board[i][j] == board[weakest.r][weakest.c] && // 공격력과 마지막 공격 시간이 같고,
                 lastAttack[i][j] == lastAttack[weakest.r][weakest.c] &&
                 (i + j > weakest.r + weakest.c || // 행과 열의 합이 더 크거나,
                  (i + j == weakest.r + weakest.c && i > weakest.r)))) { // 행과 열의 합이 같을 때 열의 값이 더 큽니다.
                weakest = {i, j}; // 새로운 가장 약한 포탑으로 업데이트합니다.
            }
        }
    }
    return weakest; // 가장 약한 포탑의 위치를 반환합니다.
}

// 1. 공격력이 가장 높은
// 2. 공격한지 가장 오래된
// 3. 행과 열의 합이 가장 작은
// 4. 열 값이 가장 작은

struct pos find_target() {
    struct pos strongest = {-1, -1}; // 가장 강한 포탑의 위치를 초기화합니다.
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (board[i][j] <= 0) continue; // 포탑이 없는 경우 건너뜁니다.
            // 가장 강한 포탑을 찾는 조건들을 순서대로 적용합니다.
            if (strongest.r == -1 || // 아직 포탑을 찾지 못했거나,
                board[i][j] > board[strongest.r][strongest.c] || // 더 큰 공격력을 가진 포탑을 찾았거나,
                (board[i][j] == board[strongest.r][strongest.c] && // 공격력이 같고 마지막 공격 시간이 더 오래되었거나,
                 lastAttack[i][j] < lastAttack[strongest.r][strongest.c]) ||
                (board[i][j] == board[strongest.r][strongest.c] && // 공격력과 마지막 공격 시간이 같고,
                 lastAttack[i][j] == lastAttack[strongest.r][strongest.c] &&
                 (i + j < strongest.r + strongest.c || // 행과 열의 합이 더 작거나,
                  (i + j == strongest.r + strongest.c && i < strongest.r)))) { // 행과 열의 합이 같을 때 열의 값이 더 작습니다.
                strongest = {i, j}; // 새로운 가장 강한 포탑으로 업데이트합니다.
            }
        }
    }
    return strongest; // 가장 강한 포탑의 위치를 반환합니다.
}

// 레이저 공격
// 최단 거리를 찾아서 공격 (우하좌상)
// 대상에는 공격력, 경로에는 1/2(몫)
// 무조건 1씩 증가하므로
void adjustPos(int& r, int& c) {
    // 행 위치 조정
    if (r < 0) r = n - 1;
    else if (r >= n) r = 0;

    // 열 위치 조정
    if (c < 0) c = m - 1;
    else if (c >= m) c = 0;
}

bool try_laser_attack(struct pos attacker, struct pos target, int turn) {
    int dmg = board[attacker.r][attacker.c];
    queue<struct pos> q;
    q.push(attacker);

    vector<vector<bool>> visited(n, vector<bool>(m, false));
    vector<vector<struct pos>> prev(n, vector<struct pos>(m, {-1, -1}));

    visited[attacker.r][attacker.c] = true;

    while (!q.empty()) {
        struct pos position = q.front();

        q.pop();

        if (target.r == position.r && target.c == position.c) {
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int newR = position.r + dirX[i], newC = position.c + dirY[i];
            // cout<<newR<<' '<<newC<<'\n';
            adjustPos(newR, newC);
            // cout<<newR<<' '<<newC<<"\n\n";
            if(!visited[newR][newC] && board[newR][newC] > 0) {
                visited[newR][newC] = true;
                prev[newR][newC] = position;
                // cout<<position.r<<' '<<position.c<<"  "<<newR<<' '<<newC<<'\n';
                q.push({newR, newC});
            }
        }
    }

   //  lastDamage = vector<vector<int>>(10, vector<int>(10, -1)); // 이건 이미 솔루션 함수에서 초기화 되어야함.

    if (!visited[target.r][target.c]) return false;

    // 경로 찾음

    // 대상 공격력 감소
    board[target.r][target.c] -= dmg;
    lastDamage[attacker.r][attacker.c] = 1;
    lastDamage[target.r][target.c] = 1;

    struct pos at = prev[target.r][target.c];
    if (at.r != -1) {
        for (; prev[at.r][at.c].r != -1 && prev[at.r][at.c].c != -1; at = prev[at.r][at.c]){
            // cout<<"path : "<<at.r<<' '<<at.c<<'\n';
            board[at.r][at.c] -= dmg / 2;
            lastDamage[at.r][at.c] = 1;
        }
    }

    return true;
}

// 포탄 공격
// 경로 없으면 원거리 공격 감행 (주변 8칸에 스플래시)

void bomb_attack(struct pos attacker, struct pos target) {
    int dmg = board[attacker.r][attacker.c];

    // 대상 공격
    lastDamage[attacker.r][attacker.c] = 1;
    board[target.r][target.c] -= dmg;
    lastDamage[target.r][target.c] = 1;

    for(int i = 0; i < 8; ++i) {
        int newR = target.r + bomb_dirX[i];
        int newC = target.c + bomb_dirY[i];
        
        // 새 위치가 배열 범위를 벗어날 수 있으므로 조정
        adjustPos(newR, newC);
        // cout<<newR<<' '<<newC<<' '<<board[newR][newC]<<'\n';

        if (board[newR][newC] > 0 && (newR != attacker.r || newC != attacker.c)) {
            board[newR][newC] -= dmg / 2;
            lastDamage[newR][newC] = 1;
        }
    }
}

// 공격 받지 않은 곳은 공격력 증가

void non_attacked() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (lastDamage[i][j] != 1 && board[i][j] > 0) {
                ++board[i][j];
            }
        }
    } 
}

void solution() {
    for (int i = 0; i < k; ++i) {
        // cout<<"turn : "<<i<<'\n';
        lastDamage = vector<vector<int>>(10, vector<int>(10, -1));

        struct pos attacker = find_attacker();
        // cout<<"attacker: "<<attacker.r<<' '<<attacker.c<<'\n';

        struct pos target = find_target();
        // cout<<"target: "<<target.r<<' '<<target.c<<'\n';

        if (attacker.r == target.r && attacker.c == target.c) return;

        lastAttack[attacker.r][attacker.c] = i;
        board[attacker.r][attacker.c] += n + m;

        if (!try_laser_attack(attacker, target, i)){
            bomb_attack(attacker, target);

        }

        non_attacked();
        // print_board();
    }
    
}


int main() {
    ios_base::sync_with_stdio(false);
    // 여기에 코드를 작성해주세요.
    cin>>n>>m>>k;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cin>>board[i][j];
        }
    }

    solution();
    int ans = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            ans = max(board[i][j], ans);
        }
    }

    cout<<ans<<'\n';
    return 0;
}