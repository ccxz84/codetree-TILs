#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

int n,m,k;

struct pos {
    int r, c;
};

// 최단 거리는 차이 절대 값의 합으로 나타냄
int get_distance(struct pos a, struct pos b) {
    return abs(a.r - b.r) + abs(a.c - b.c);
}

bool is_range(struct pos a) {
    return a.r >= 0 && a.r < n && a.c >= 0 && a.c < n;
}

vector<struct pos> people;
vector<int> move_distance(10, 0);
vector<vector<int>> board;
vector<bool> out(10, false);
struct pos extraction;

void print_board() {
    for (int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cout<<board[i][j]<<' ';
        }
        cout<<'\n';
    }
    cout<<'\n';
}

void print_people() {
    for(int i = 0; i < m; ++i) {
        cout<<"people: "<<people[i].r<<' '<<people[i].c<<" out: "<<out[i]<<" move distance: "<<move_distance[i]<<'\n';
    }
    cout<<'\n';
}

void print_extraction() {
    cout<<"extraction: "<<extraction.r<<' '<<extraction.c<<"\n\n";
}

// 상하좌우로 이동 가능 상하 우선시
// 움직인 칸은 현재 칸 보다 출구까지의 거리가 가까워야
// 움직이지 못하면 가만히
// 겹칠 수 있음

int dirX[4] = {-1, 1, 0, 0};
int dirY[4] = {0, 0, -1, 1};

int get_people_dir(int index) {
    struct pos position = people[index];
    int val = get_distance(position, extraction);
    int dir = -1;
    
    for (int i = 0; i < 4; ++i) {
        struct pos newPosition = {position.r + dirX[i], position.c + dirY[i]};

        if (!is_range(newPosition)) continue;

        if (board[newPosition.r][newPosition.c] > 0) continue;

        int dist = get_distance(newPosition, extraction);

        if (dist < val) {
            dir = i;
            val = dist;
        }
    }

    return dir;
}

void move_people() {
    for (int i = 0; i < m; ++i) {
        if (out[i]) continue;

        int dir = get_people_dir(i);

        if (dir == -1) continue;

        struct pos newPosition = {people[i].r + dirX[dir], people[i].c + dirY[dir]};
        people[i] = newPosition;

        if (people[i].r == extraction.r && people[i].c == extraction.c) {
            out[i] = true;
        } 
        ++move_distance[i];
    }
}

// 회전
// 한 명 이상의 참가자와 출구를 포함한 가장 작은 정사각형
// 가장 작은 크기가 2개 이상이면 좌상단 r 좌표가 작은 것, 그래도 같으면 c 좌표가 작은 것 우선

pair<int, struct pos> get_smallest_rectangle(struct pos a) {
    int diffX = abs(extraction.r - a.r) + 1, diffY = abs(extraction.c - a.c) + 1;
    int size = max(diffX, diffY);

    struct pos topLeft = { max(a.r, extraction.r) - (size - 1) < 0 ? 0 : max(a.r, extraction.r) - (size - 1),
     max(a.c, extraction.c) - (size - 1) < 0 ? 0 : max(a.c, extraction.c) - (size - 1) };

    return {size, topLeft};
}

bool compare(pair<int, struct pos> a, pair<int, struct pos> b) {
    if (a.first != b.first) return a.first < b.first;
    if (a.second.r != b.second.r) return a.second.r < b.second.r;
    if (a.second.c != b.second.c) return a.second.c < b.second.c;
}

pair<int, struct pos> find_smallest_rectangle() {
    pair<int, struct pos> small = {INT_MAX, {0, 0}};

    for (int i = 0; i < m; ++i) {
        if (out[i]) continue;
        pair<int, struct pos> diff = get_smallest_rectangle(people[i]);
        if (compare(diff, small)) {
            small = diff;
        } 
    }

    return small;
}

bool is_range_rectangle(int size, struct pos topLeft, struct pos position) {
    return topLeft.r <= position.r && topLeft.c <= position.c && topLeft.r + size > position.r && topLeft.c + size > position.c;
}

struct pos rotation_position(int size, struct pos topLeft, struct pos position) {
    // 상대 위치 계산
    int relativeR = position.r - topLeft.r;
    int relativeC = position.c - topLeft.c;
    
    // 회전된 위치 계산
    int newR = topLeft.r + relativeC;
    int newC = topLeft.c + (size - 1 - relativeR);

    return {newR, newC};
}

// 시계방향으로 90도 회전, 회전 되면 내구도 1 감소
void rotation(int size, struct pos position) {
    vector<vector<int>> temp (size, vector<int>(size, 0));

    for(int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            // cout<<"check "<<j<<' '<<size - i - 1<<'\n';
            temp[j][size - i - 1] = board[i + position.r][j + position.c];
        }
    }

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            board[i + position.r][j + position.c] = --temp[i][j];
        }
    }

    if (is_range_rectangle(size, position, extraction)) {
        extraction = rotation_position(size, position, extraction);
    }

    for(int i = 0; i < m; ++i) {
        if (is_range_rectangle(size, position, people[i])) {
            people[i] = rotation_position(size, position, people[i]);
        }
    }
}

bool check_end() {
    bool check = true;
    for(int i = 0; i < m; ++i) {
        if (!out[i]) {
            check = false;
            break;
        }
    }
    return check;
}

void solution() {
    for (int i = 0; i < k; ++i) {
        // cout<<"turn: "<<i+1<<'\n';
        move_people();

        if (check_end()) break;

        pair<int, struct pos> small = find_smallest_rectangle();
        
        rotation(small.first, small.second);

        // print_board();
        // print_extraction();
        // print_people();
    }
}

int main() {
    cin>>n>>m>>k;
    board = vector<vector<int>>(n, vector<int>(n, 0));
    people = vector<struct pos>(m);

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            cin>>board[i][j];
        }
    }

    for(int i = 0; i < m; ++i) {
        cin>>people[i].r>>people[i].c;
        --people[i].r; --people[i].c;
    }

    cin>>extraction.r>>extraction.c;
    --extraction.r;--extraction.c;

    // cout<<"turn 0: \n";
    // print_board();
    // print_extraction();
    // print_people();

    solution();

    int ans = 0;
    for(int i = 0; i < m; ++i) {
        ans += move_distance[i];
    }

    cout<<ans<<'\n'<<extraction.r+1<<' '<<extraction.c+1<<'\n';
    return 0;
}