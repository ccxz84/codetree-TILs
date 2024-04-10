#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

// 최단거리 x1-x2 절대값 + y1 - y2 절대값
// 상하 우선 (상하좌우 가능)
// 움직인 곳은 최단 거리가 가까워야함
// 움직일 수 없으면 가만히 있고, 한칸에 두명 있을 수 있음

// 한 명 이상의 참가자와 출구를 포함한 가장 작은 정사각형
// r 좌표가 작거나, c 좌표가 작은 것 우선
// 시계 방향 90도 회전, 회전 되면 내구도 1 감소

// 참가자는 board에 -1 ~ -n으로 표시

int dirX[4] = {1, -1, 0, 0};
int dirY[4] = {0, 0, -1, 1}; 

vector<vector<int>> board;
vector<bool> extraction;
vector<int> dist;
int exitR, exitC;

vector<struct pos> people;

int ans = 0;

int n, m, k;

struct pos {
    int r;
    int c;
};


#define exit -11

void print_board() {
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void print_people() {
    for(int i = 0; i < m; ++i) {
        cout<<"people "<<i+1<<" pos: "<<people[i].r+1<<' '<<people[i].c+1<<" dist: "<<dist[i]<<" extraction: "<<extraction[i]<<'\n';
    }
    cout<<'\n';
}



bool is_range(int r, int c) {
    return r >= 0 && r < n && c >= 0 && c < n;
}

void print_exit() {
    cout<<"exit pos: "<<exitR+1<<' '<<exitC+1<<'\n';
    cout<<'\n';
}

void move_people(int idx) {
    struct pos target = people[idx];
    int dis = abs(target.r - exitR) + abs(target.c - exitC);
    int dir = -1;

    for(int i = 0; i < 4; ++i) {
        int newR = target.r + dirX[i], newC = target.c + dirY[i];

        if (!is_range(newR, newC) || board[newR][newC] > 0) continue;

        int diffX = newR - exitR, diffY = newC - exitC;
        int diff = abs(diffX) + abs(diffY);

        if (diff < dis) {
            dir = i;
            dis = diff;
        }
    }

    if (dir == -1) return;

    target.r += dirX[dir];
    target.c += dirY[dir];
    // cout<<idx<<'\n';
    people[idx] = target;
    if (target.r == exitR && target.c == exitC) {
        extraction[idx] = true;
    }
    ++dist[idx];
    ++ans;
}

pair<int, struct pos> find_location() {
    pair<int, pair<int, int>> val = {INT_MAX, {0, 0}};

    int index;
    for(int i = 0; i < m; ++i) {
        if (extraction[i]) continue;
        struct pos target = people[i];

        int diffX = abs(target.r - exitR), diffY = abs(target.c - exitC);
        pair<int, pair<int, int>> check = {max(diffX, diffY), {target.r, target.c}};
        
        if (check < val) {
            val = check;
            index = i;
        }
    }
    // cout<<exitR<<exitC<<'\n';
    // cout<<index<<val.second.first<<val.second.second<<'\n';
    
    int minR = max(val.second.first, exitR) - val.first;
    int minC = max(val.second.second, exitC) - val.first;

    // cout<<val.first<<' '<<val.second.first<<' '<<val.second.second<<'\n';

    // cout<<min(val.second.first, exitR)<<' '<<min(val.second.first, exitC)<<'\n';
    
    // cout<<(minR < 0 ? 0 : minR)<<' '<<(minC < 0 ? 0 : minC)<<'\n';

    return {val.first + 1, {minR < 0 ? 0 : minR, minC < 0 ? 0 : minC}};
}

struct pos rotateLeft90InSquare(int topLeftRow, int topLeftCol, int size, int row, int col) {
    int relativeRow = row - topLeftRow;
    int relativeCol = col - topLeftCol;
    // 좌측 90도 회전
    int newCol = size - 1 - relativeRow;
    int newRow = relativeCol;
    return {topLeftRow + newRow, topLeftCol + newCol};
}

bool isInsideSquare(int topLeftX, int topLeftY, int size, int x, int y) {
    return x >= topLeftX && x < topLeftX + size &&
           y >= topLeftY && y < topLeftY + size;
}

void rotation() {
    pair<int, struct pos> target = find_location();
    if(target.first < 0) return;
    // cout<<target.first<<' '<<target.second.r<<' '<<target.second.c<<'\n';
    vector<vector<int>> temp(target.first, vector<int>(target.first));

    for(int i = 0; i < target.first; ++i) {
        for(int j = 0; j < target.first; ++j) {
            temp[j][target.first - i - 1] = board[target.second.r + i][target.second.c + j];
        }
    }

    for(int i = 0; i < target.first; ++i) {
        for(int j = 0; j < target.first; ++j) {
            board[target.second.r + i][target.second.c + j] = temp[i][j] > 0 ? temp[i][j] - 1 : temp[i][j];
        }
    }

    for (int i = 0; i < m; ++i) {
        if (extraction[i]) continue;
        if (isInsideSquare(target.second.r, target.second.c, target.first, people[i].r, people[i].c)) {
            people[i] = rotateLeft90InSquare(target.second.r, target.second.c, target.first, people[i].r, people[i].c);
        }
    }

    struct pos _exit = rotateLeft90InSquare(target.second.r, target.second.c, target.first, exitR, exitC);
    exitR = _exit.r; exitC = _exit.c;
}

void solution() {

    for (int z = 0; z < k; ++z) {
        // cout<<"turn : "<<z+1<<'\n';
        // cout<<"move\n";
        for(int i = 0; i < m; ++i) {
            if (extraction[i]) continue;
            move_people(i);
        }

        // print_board();
        // print_people();
        // print_exit();
        // cout<<"\nrotation\n";

        rotation();

        // print_board();
        // print_people();
        // print_exit();
        // cout<<'\n';
    }
    
}

int main() {
    cin>>n>>m>>k;

    board = vector<vector<int>>(n, vector<int>(n));
    dist = vector<int>(m, 0);
    people = vector<struct pos>(m);
    extraction = vector<bool>(m, false);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cin>>board[i][j];
        }
    }

    for(int i = 1; i <= m; ++i) {
        int r, c;
        cin>>r>>c;

        people[i-1] = {r-1, c-1};
    }

    cin>>exitR>>exitC;
    --exitR;
    --exitC;

    // board[exitR][exitC] = exit;

    // cout<<"turn 0\n";

    // print_board();
    // print_people();
    // print_exit();

    solution();

    cout<<ans<<'\n';
    cout<<exitR+1<<' '<<exitC+1<<'\n';
    
    return 0;
}