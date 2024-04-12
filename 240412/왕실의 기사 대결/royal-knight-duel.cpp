#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int l, n, q;

struct knight {
    int r,c,h,w,origin_health, current_health;
};

vector<vector<int>> board(40, vector<int>(40, 0));
vector<struct knight> knights;

int dirX[4] = {-1, 0, 1, 0};
int dirY[4] = {0, 1, 0, -1};

int dir, target;

void print_board() {
    for(int i = 0; i < l; ++i) {
        for(int j = 0; j < l; ++j) {
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void print_knights() {
    for(int i = 0; i < n; ++i) {
        cout<<"knights "<<i<<" pos: "<<knights[i].r<<' '<<knights[i].c<<" health: "<<knights[i].current_health<<'\n';
    }
    cout<<'\n';
}

bool is_range(int r, int c) {
    return r >= 0 && r < l && c >= 0 && c < l;
}

bool is_overlap(struct knight a, struct knight b) {
    int aRightR = a.r + a.h - 1; int aRightC = a.c + a.w - 1;
    int bRightR = b.r + b.h - 1; int bRightC = b.c + b.w - 1;

    if (a.r > bRightR) return false;
    if (aRightR < b.r) return false;
    if (a.c > bRightC) return false;
    if (aRightC < b.c) return false;

    return true;
}

// 기사 이동
// 명령 (입력)을 통해 상하좌우로 한칸 이동
// 이동 위치에 다른 기사 있으면 연쇄적으로 한칸 밀림
// 이동 방향에 벽이 있다면 이동 불가 (bfs를 통해 가능 여부 파악 필요)

bool try_move() {
    queue<int> q;
    q.push(target);
    vector<bool> visited(n, false);
    vector<int> dmg(n, 0);

    visited[target] = true;

    while(!q.empty()) {
        int current = q.front();
        struct knight current_knight = knights[current];
        q.pop();

        current_knight.r += dirX[dir]; current_knight.c += dirY[dir];

        if (!is_range(current_knight.r, current_knight.c)) {
            // cout<<"1 out knights: "<<current<<'\n';
            return false;
        }

        for (int i = current_knight.r; i < current_knight.r + current_knight.h; ++i) {
            for (int j = current_knight.c; j < current_knight.c + current_knight.w; ++j) {
                if (board[i][j] == 1)
                    ++dmg[current];
                else if (board[i][j] == 2){
                    // cout<<"2 out knights: "<<current<<'\n';
                    return false;
                } 
            }
        }

        for (int i = 0; i < n; ++i) {
            if (visited[i] || knights[i].current_health <= 0) continue;

            if (!is_overlap(current_knight, knights[i])) continue;

            visited[i] = true;
            q.push(i);
        }
    }

    dmg[target] = 0;

    for(int i = 0; i < n; ++i) {
        if (visited[i]) {
            knights[i].r += dirX[dir]; knights[i].c += dirY[dir];
            knights[i].current_health -= dmg[i];
        }
    }

    return true;
}

void move_knight() {
    if (knights[target].current_health <= 0) return;

    try_move();
}


// !! 사라진 기사에게 명령 내리면 명령 무시



// 대결
// 명령을 받은 기사가 다른 기사를 밀치면
// 이동한 위치의 함정의 수 만큼 피해를 입음
// 체력 0 이하면 사라짐
// 직접 명령 받은 기사는 피해 안입음
// 밀려나도 함정 없으면 피해 안입음




// int test_1() {
//     l = 4; n = 3;
//     board = {{
//         0,0, 1, 0
//     },{
//         0, 0, 1, 0
//     },{
//         1, 1, 0, 1
//     },{
//         0, 0, 2, 0
//     }};

//     knights = {{
//         0, 1, 2, 1, 5, 5
//     },{
//         1, 0, 2, 1, 1, 1
//     },{
//         2, 1, 1, 2, 3, 3
//     }};

//     target = 1; dir = 1;

//     print_knights();

//     try_move();

//     target = 2; dir = 3;

//     try_move();

//     print_board();
//     print_knights();
//     return 0;
// }

int main() {
    cin>>l>>n>>q;

    board = vector<vector<int>>(l, vector<int>(l, 0));
    knights = vector<struct knight>(n);

    for(int i = 0; i < l; ++i) {
        for (int j = 0; j < l; ++j) {
            cin>>board[i][j];
        }
    }

    for(int i = 0; i < n; ++i) {
        int r,c,h,w,k;
        cin>>r>>c>>h>>w>>k;
        knights[i] = {r-1, c-1, h, w, k, k};
    }

    for(int i = 0; i < q; ++i) {
        cin>>target>>dir;
        --target;

        move_knight();
    }

    int ans = 0;

    for(int i = 0; i < n; ++i) {
        if (knights[i].current_health > 0) {
            ans += knights[i].origin_health - knights[i].current_health;
        }
    }
    cout<<ans<<'\n';
    return 0;
}