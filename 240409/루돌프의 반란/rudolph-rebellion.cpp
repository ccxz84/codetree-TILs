#include <iostream>
#include <vector>
#include <climits>
#include <cmath>

using namespace std;

struct pos {
    int x;
    int y;
};

vector<struct pos> santa;
vector<vector<int>> board;
struct pos rudol;
vector<bool> out;
vector<int> stun;
vector<int> score;

int dirX[4] = {1, 0, -1, 0};
int dirY[4] = {0, 1, 0, -1};

int n, m, p, c, d;

void print_rudol() {
    cout<<"rudol pos: "<<rudol.x<<' '<<rudol.y<<'\n';
}

void print_santa() {
    for (int i = 0; i < p; ++i) {
        cout<<"santa "<<i<<" pos: "<<santa[i].x<<' '<<santa[i].y<<" score: "<<score[i]<<" stun: "<<stun[i]<<" out: "<<out[i]<<'\n';
    }
}

// 루돌프 기준 가장 가까운 산타 찾기
int find_santa() {
    int dis = INT_MAX;
    int index = -1;

    pair<int, pair<int, int>> current = {INT_MAX, {0, 0}};

    for (int i = 0; i < p; ++i) {
        if(out[i]) continue;
        int diffX = santa[i].x - rudol.x;
        int diffY = santa[i].y - rudol.y;
        pair<int, pair<int, int>> diff = {diffX * diffX + diffY * diffY, {-diffX, diffY}};

        if (diff < current) {
            current = diff;
            index = i;
        }
    }

    return index;
}

bool is_range(struct pos pos) {
    return pos.x >= 0 && pos.x < n && pos.y >= 0 && pos.y < n;
}

void chain(struct pos pos, struct pos dir) {
    struct pos target = pos;
    int next = board[target.x][target.y];
    while (next >= 0) {
        int index = board[target.x][target.y];
        int newR = target.x + dir.x, newC = target.y + dir.y;

        board[target.x][target.y] = -2;

        if (is_range({newR, newC})) {
            santa[index] = {newR, newC};
            next = board[newR][newC];
            board[newR][newC] = index;
        } else {
            out[index] = true;
            next = -1;
        }
    }
}

// 루돌프 이동
void move_rudol(int index, int turn) {
    struct pos target = santa[index];
    int diffX = target.x - rudol.x;
    int diffY = target.y - rudol.y;
    struct pos dir = { diffX == 0 ? 0 : diffX / abs(diffX), diffY == 0 ? 0 : diffY / abs(diffY) };

    board[rudol.x][rudol.y] = -2;
    rudol = { rudol.x + dir.x, rudol.y + dir.y };

    if (board[rudol.x][rudol.y] == index) {
        int newR = target.x + dir.x * c , newC = target.y + dir.y * c; 
        score[index] += c;
        board[target.x][target.y] = -2;

        if (is_range({newR, newC})) {
            santa[index] = {newR, newC};

            // 연쇄 작용 체크
            chain({newR, newC}, dir); // 옮겨온 위치 파라미터
            board[newR][newC] = index;
            stun[index] = turn + 1;
        } else {
            out[index] = true;
        }
    }
    
    board[rudol.x][rudol.y] = -1;
}

// 산타 이동
void move_santa(int turn) {
    for (int i = 0; i < p; ++i) {
        if (stun[i] >= turn || out[i]) continue;

        int diffX = santa[i].x - rudol.x;
        int diffY = santa[i].y - rudol.y;
        int dis = diffX * diffX + diffY * diffY;
        struct pos dir = {0, 0};

        for (int j = 0; j < 4; ++j) {
            struct pos new_pos = {santa[i].x + dirX[j], santa[i].y + dirY[j]};

            if (!(is_range(new_pos) && board[new_pos.x][new_pos.y] < 0)) continue;

            diffX = new_pos.x - rudol.x;
            diffY = new_pos.y - rudol.y;

            int diff = diffX * diffX + diffY * diffY;

            if (diff < dis) {
                dir = {dirX[j], dirY[j]};
                dis = diff;
            }
        }

        // cout<<"santa dir: "<<dir.x<<' '<<dir.y<<'\n';

        board[santa[i].x][santa[i].y] = -2;
        santa[i] = { santa[i].x + dir.x, santa[i].y + dir.y };

        if (board[santa[i].x][santa[i].y] == -1) {
            int newR = santa[i].x - dir.x * d , newC = santa[i].y - dir.y * d;

            score[i] += d;
            
            if(is_range({newR, newC})) {
                santa[i] = {newR, newC};
                // cout<<newR<<newC<<-dir.x<<-dir.y<<'\n';
                // cout<<santa[1].x<<santa[1].y<<'\n';
                chain({newR, newC}, {-dir.x, -dir.y}); // 옮겨온 위치 파라미터
                board[newR][newC] = i;
                stun[i] = turn + 1;
            } else {
                out[i] = true;
            }
        } else {
            board[santa[i].x][santa[i].y] = i;
        }
    }

    for(int i = 0; i < p; ++i) {
        if (!out[i]) ++score[i];
    }
}

void solution() {

    // print_rudol();
    // print_santa();

    for(int z = 0; z < m; ++z) {
        // 산타 찾기
        int index = find_santa();

        // cout<<"rudol move\n";
        // 루돌프 이동
        move_rudol(index, z);

        // print_rudol();
        // print_santa();

        // cout<<"santa move\n";
        // 산타 이동
        move_santa(z);

        // print_rudol();
        // print_santa();
    }
}


int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); 

    cin>>n>>m>>p>>c>>d;
    cin>>rudol.x>>rudol.y;

    rudol.x -= 1;
    rudol.y -= 1;

    santa = vector<struct pos>(p, {0, 0});
    out = vector<bool>(p, false);
    board = vector<vector<int>>(n, vector<int>(n, -2));
    stun = vector<int>(p, -1);
    score = vector<int>(p, 0);

    for(int i = 0; i < p; ++i) {
        int index;
        int x, y;
        cin>>index>>x>>y;

        santa[index-1] = {x-1, y-1};
    }

    solution();

    for (int i = 0; i < p; ++i) {
        cout<<score[i]<<' ';
    }
    cout<<'\n';
    return 0;
}