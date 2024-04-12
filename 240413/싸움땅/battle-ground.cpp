#include <iostream>
#include <vector>
#include <queue>

using namespace std;

int n,m,k;

struct pos {
    int r, c;
};

struct player {
    struct pos position;
    int abil, dirX, dirY, gun, point;
};

vector<vector<priority_queue<int>>> board; // 총이 여러 개 놓일 수 있기 때문에
vector<vector<int>> player_board;// 플레이어 인덱스 기록할거기 때문에 -1로
vector<struct player>players;

bool is_range(struct pos a) {
    return a.r >= 0 && a.r < n && a.c >= 0 && a.c < n;
}

void print_board() {
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cout<<board[i][j].top()<<' ';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void print_player_board() {
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cout<<player_board[i][j]<<' ';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void print_players() {
    for(int i = 0; i < m; ++i) {
        cout<<"player "<<i<<" pos: "<<players[i].position.r<<' '<<players[i].position.c<<" dir: "<<players[i].dirX<<' '<<players[i].dirY<<" abil: "<<players[i].abil<<" gun: "<<players[i].gun<<" point: "<<players[i].point<<'\n';
    }
    cout<<'\n';
}

// 플레이어가 놓인 다음, 총이 놓임
// 플레이어는 초기 능력치를 가짐

// 더 능력치 좋은 총을 먹으면 기존 총을 내려 놓음
void down_gun(int idx) {
    int gun = players[idx].gun;
    players[idx].gun = 0;
    struct pos position = players[idx].position;

    board[position.r][position.c].push(gun);
}

// 플레이어가 총을 먹으면 총 + 초기 능력치가 공격력이 됨
void get_gun(int idx) {
    struct pos position = players[idx].position;
    int prev_gun = players[idx].gun;

    int board_gun = board[position.r][position.c].empty()? 0 : board[position.r][position.c].top();

    if (prev_gun < board_gun) {
        board[position.r][position.c].pop();
        players[idx].gun = board_gun;
        board[position.r][position.c].push(prev_gun);
    }
}

// 벽을 만나면 반대 방향으로 이동
void turn_wall(int idx) {
    players[idx].dirX = -players[idx].dirX; players[idx].dirY = -players[idx].dirY;
}

struct pos move_loser(int idx) {
    // cout<<"old position: "<<players[idx].position.r<<' '<<players[idx].position.c<<'\n';
    
    int dirX = -players[idx].dirY;
    int dirY = players[idx].dirX;
    for(int i = 0; i < 4; ++i) {
        int temp = dirX;
        dirX = dirY;
        dirY = -temp;

        // cout<<"dir : "<<dirX<<' '<<dirY<<'\n';

        struct pos newPosition = {players[idx].position.r + dirX, players[idx].position.c + dirY};

        // cout<<"new position: "<<newPosition.r<<' '<<newPosition.c<<'\n';

        if (!is_range(newPosition)) continue;
        if (player_board[newPosition.r][newPosition.c] > 0) continue;

        players[idx].dirX = dirX;
        players[idx].dirY = dirY;

        // cout<<"new position: "<<newPosition.r<<' '<<newPosition.c<<'\n';

        return newPosition;
    }
}

bool compare(struct player a, struct player b) {
    if (a.abil + a.gun != b.abil + b.gun) return a.abil + a.gun > b.abil + b.gun;
    return a.abil > b.abil;
}

// 이동한 칸에 적이 있으면 싸움
void fight_player(int idx, struct pos& newPosition) {
    // 이동한 플레이어가 attacker
    int target = player_board[newPosition.r][newPosition.c];
    // cout<<newPosition.r<<' '<<newPosition.c<<'\n';

    if (compare(players[idx], players[target])) {
        players[idx].point += players[idx].abil + players[idx].gun - players[target].abil - players[target].gun;

        down_gun(target);
        get_gun(idx);
        struct pos prev_location = players[target].position;
        struct pos target_position = move_loser(target);

        player_board[prev_location.r][prev_location.c] = -1;
        players[target].position = target_position;
        player_board[target_position.r][target_position.c] = target;

        get_gun(target);
    } else {
        // cout<<"패배 \n";
        players[target].point += players[target].abil + players[target].gun - players[idx].abil - players[idx].gun;
        down_gun(idx);
        get_gun(target);
        newPosition = move_loser(idx);
        // cout<<"turn move: "<<newPosition.r<<' '<<newPosition.c<<'\n';
    }
}

// 플레이어는 자기가 보는 방향으로 한 칸 이동
void player_move(int idx) {
    struct pos newPosition = {players[idx].position.r + players[idx].dirX, players[idx].position.c + players[idx].dirY};

    if (!is_range(newPosition)) {
        turn_wall(idx);
        newPosition = {players[idx].position.r + players[idx].dirX, players[idx].position.c + players[idx].dirY};
    }

    player_board[players[idx].position.r][players[idx].position.c] = -1;
    players[idx].position = newPosition;

    // cout<<idx<<' '<<newPosition.r<<' '<<newPosition.c<<"\n";
    // cout<<player_board[newPosition.r][newPosition.c]<<"\n\n";

    if (player_board[newPosition.r][newPosition.c] > -1) {
        // cout<<idx<<' '<<"find?\n\n";
        fight_player(idx, newPosition);
        players[idx].position = newPosition;
        
    }

    player_board[newPosition.r][newPosition.c] = idx;
    
    down_gun(idx);
    get_gun(idx);
}

void solution() {
    for(int i = 0; i < k; ++i) {
        // cout<<"turn : "<<i+1<<'\n';
        for(int j = 0; j < m; ++j) {
            player_move(j);
        }  
        // print_board();
        // print_player_board();
        // print_players();
    }
}

int dirX[4] = {-1, 0, 1, 0};
int dirY[4] = {0, 1, 0, -1};

int main() {
    cin>>n>>m>>k;

    players = vector<struct player>(m);
    board = vector<vector<priority_queue<int>>>(n, vector<priority_queue<int>>(n)); // 총이 여러 개 놓일 수 있기 때문에
    player_board = vector<vector<int>>(n, vector<int>(n, -1));    

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            int gun;
            cin>>gun;
            board[i][j].push(gun);
        }
    }

    for(int i = 0; i < m; ++i) {
        int x,y,d,s;
        cin>>x>>y>>d>>s;

        players[i].position.r = x-1;
        players[i].position.c = y-1;
        players[i].dirX = dirX[d];
        players[i].dirY = dirY[d];
        players[i].abil = s;
        player_board[x-1][y-1] = i;
    }

    // cout<<"turn : 0\n";

    // print_board();
    // print_player_board();
    // print_players();

    solution();

    for(int i = 0; i < m; ++i) {
        cout<<players[i].point<<' ';
    }
    cout<<'\n';
    return 0;
}