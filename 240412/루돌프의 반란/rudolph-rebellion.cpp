#include <iostream>
#include <vector>
#include <climits>

using namespace std;

struct pos {
    int r, c;
};

int n, m, p, c, d;

struct santa {
    struct pos position;
    bool lived;
    int score;
    int stun;
};

vector<struct santa> santa;
struct pos rudol;

int turn;

vector<vector<int>> board;

void print_rudol() {
    cout<<"rudol: "<<rudol.r<<' '<<rudol.c<<'\n';
}

void print_board() {
    for(int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
}

void print_santa() {
    for(int i = 0; i < p; ++i) {
        cout<<"santa "<<i<<" pos: "<<santa[i].position.r<<' '<<santa[i].position.c<<" lived: "<<santa[i].lived<<" score: "<<santa[i].score<<" stun: "<<santa[i].stun<<'\n';
    }
}

bool is_range(struct pos a) {
    return a.r >= 0 && a.r < n && a.c >= 0 && a.c < n;
}

// 충돌
// 산타와 루돌프가 같은 칸에 있으면 충돌
// 밀려난 위치가 게임판 밖이면 탈락


// 밀려난 칸에 다른 산타가 있다면 상호작용
// 상호작용
// 산타끼리 박치기 하면 상호작용 발생
// 밀려나온 산타의 방향으로 한칸 밀려남
// 연쇄적으로 밀려날 수 있음

void santa_chain(struct pos position, int dirX, int dirY) {
    struct pos chain = position;
    int index = board[position.r][position.c];
    board[position.r][position.c] = -1;
    while (true) {
        struct pos newPosition = {chain.r + dirX, chain.c + dirY};

        if (!is_range(newPosition)) {
            santa[index].lived = false;
            santa[index].position = newPosition;
            break;
        }

        if (board[newPosition.r][newPosition.c] > -1) {
            int temp = board[newPosition.r][newPosition.c];
            board[newPosition.r][newPosition.c] = index;
            santa[index].position = newPosition;
            index = temp;
            chain = newPosition;
        } else {
            board[newPosition.r][newPosition.c] = index;
            santa[index].position = newPosition;
            break;
        }
    }
}


// 루돌프가 움직인 충돌은 c만큼 점수 획득, 산타는 루돌프가 이동한 방향으로 c칸 넉백
// 산타가 움직여서 일어난 충돌은 d 점수 획득, 산타는 자신이 이동한 반대 방향으로 d칸 넉백
void knockback(int santaIndex, int dirX, int dirY, int score) {
    struct pos santaPosition = santa[santaIndex].position;

    struct pos newPosition = {santaPosition.r + dirX * score, santaPosition.c + dirY * score};
    // cout<<"knockback new Position: "<<newPosition.r<<' '<<newPosition.c<<'\n';

    board[santaPosition.r][santaPosition.c] = -1;

    if (!is_range(newPosition)) {
        santa[santaIndex].position = newPosition;
        santa[santaIndex].lived = false;
        santa[santaIndex].score += score;
        return;
    }

    if (board[newPosition.r][newPosition.c] > -1) {
        santa_chain(newPosition, dirX, dirY);
    }

    board[newPosition.r][newPosition.c] = santaIndex;
    santa[santaIndex].score += score;
    santa[santaIndex].stun = turn + 1;
    santa[santaIndex].position = newPosition;
}


// 루돌프 -> 산타 순으로 이동
// 두칸 사이의 거리는 (r 차이^2 + x 차이^2);

int get_distance(struct pos a, struct pos b) {
    return (a.r - b.r) * (a.r - b.r) + (a.c - b.c) * (a.c - b.c);
}

// 루돌프 이동

// 가장 가까운 산타에게 이동함 (탈락하지 않은   O)
// 거리가 같다면 r 좌표가 큰, r이 같다면 c가 큰                 O

bool compare(pair<int, struct pos> a, pair<int, struct pos> b) {
    if (a.first != b.first) return a.first < b.first;
    if (a.second.r != b.second.r) return a.second.r > b.second.r;
    return a.second.c > b.second.c;
}

int find_santa(){
    int val = INT_MAX;
    struct pos ret = {-1, -1};
    int index = -1;
    for(int i = 0; i < p; ++i) {
        if (!santa[i].lived) continue;

        struct pos position = santa[i].position;
        int dist = get_distance(position, rudol);

        if (compare({dist, position}, {val, ret})) {
            val = dist;
            ret = position;
            index = i;
        } 
    }

    return index;
}

// 루돌프는 상하좌우, 대각선(모두 1칸) -> 가까워 지는 방향으로 1칸
int rudol_moveR[8] = {1, -1, 0, 0, 1, -1, 1, -1};
int rudol_moveC[8] = {0, 0, 1, -1, 1, -1, -1, 1};

int get_rudol_dir(struct pos santaPos) {
    int val = INT_MAX;
    int index = -1;

    for (int i = 0 ; i < 8; ++i) {
        struct pos newPosition = {rudol.r + rudol_moveR[i], rudol.c + rudol_moveC[i]};

        if (!is_range(newPosition)) continue;

        int dist = get_distance(newPosition, santaPos);

        if (dist < val) {
            val = dist;
            index = i;
        }
    }
    return index;
}

void move_rudol() {
    int targetSanta = find_santa();

    if (targetSanta == -1) return;

    int dir = get_rudol_dir(santa[targetSanta].position);

    rudol.r += rudol_moveR[dir]; rudol.c += rudol_moveC[dir];

    if (rudol.r == santa[targetSanta].position.r && rudol.c == santa[targetSanta].position.c) {
        knockback(targetSanta, rudol_moveR[dir], rudol_moveC[dir], c);
    }
}


// 산타 이동
// 1번 부터 P번까지 순차적으로 이동
// 산타는 루돌프에게 거리가 가장 가까워지는 방향으로 이동
// 산타는 다른 산타가 있는 칸, 게임 판 밖으로 이동 불가능
// 움직일 수 없다면 한 턴 대기

// 상우하좌 우선순위
int dirR[4] = {-1, 0, 1, 0};
int dirC[4] = {0, 1, 0, -1};

int get_santa_dir(int santaIndex) {
    struct pos santaPos = santa[santaIndex].position;

    int val = get_distance(rudol, santaPos), dir = -1;
    for(int i = 0; i < 4; ++i) {
        struct pos newPosition = {santaPos.r + dirR[i], santaPos.c + dirC[i]};

        if (!is_range(newPosition)) continue;

        if (board[newPosition.r][newPosition.c] > -1) continue;

        int dist = get_distance(newPosition, rudol);

        if (dist < val) {
            val = dist;
            dir = i;
        }
    }
    return dir;
}

void move_santa() {
    for (int i = 0; i < p; ++i) {

        if (!santa[i].lived) continue;
        if (santa[i].stun >= turn) continue;

        int dir = get_santa_dir(i);
        
        if (dir == -1) continue;
        // cout<<i<<' '<<dirR[dir]<<' '<<dirC[dir]<<'\n';

        struct pos newPosition = {santa[i].position.r + dirR[dir], santa[i].position.c + dirC[dir]};

        // cout<<"new Position: "<<newPosition.r<<' '<<newPosition.c<<'\n';

        board[santa[i].position.r][santa[i].position.c] = -1;
        board[newPosition.r][newPosition.c] = i;
        santa[i].position = newPosition;

        if (newPosition.r == rudol.r && newPosition.c == rudol.c) {
            knockback(i, -dirR[dir], -dirC[dir], d);
        }
    }
}


// 기절
// 루돌프와 충돌하면 기절
// 현재 k번째 턴을 기준으로 k + 2턴에 정상 동작 가능
// m 번 턴이 모두 끝나거나 산타가 모두 탈락하면 게임 끝

bool check_end() {
    bool check = true;
    for (int i = 0; i < p; ++i) {
        if (santa[i].lived) check = false;
    }
    return check;
}

void solution() {
    for (turn = 0; turn < m; ++turn) {
        // cout<<"turn: "<<turn<<'\n';
        move_rudol();

        // if (check_end()) break;

        move_santa();

        for (int i = 0; i < p; ++i) {
            if (santa[i].lived) ++santa[i].score;
        }
        // print_rudol();
        // cout<<'\n';
        // print_board();
        // cout<<'\n';
        // print_santa();
        // cout<<'\n';
        // cout<<'\n';
        if (check_end()) break;
    }
}

int main() {
    cin>>n>>m>>p>>c>>d;

    cin>>rudol.r>>rudol.c;
    --rudol.r;
    --rudol.c;

    board = vector<vector<int>>(n, vector<int>(n, -1));
    santa = vector<struct santa>(p, {{0,0}, true, 0, -1});

    for(int i = 0; i < p; ++i) {
        int index; struct pos position;
        cin>>index>>position.r>>position.c;
        --index; --position.r; --position.c;
        santa[index].position = position;
        board[position.r][position.c] = index;
    }

    solution();

    for(int i = 0; i < p; ++i) {
        cout<<santa[i].score<<' ';
    }
    cout<<'\n';
    return 0;
}