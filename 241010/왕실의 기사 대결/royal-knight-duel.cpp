#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
#include <stack>
#include <algorithm>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define MAX_N 30
#define MAX_L 40

int matrix[MAX_L][MAX_L];

int l, n, q;
int w[MAX_N], h[MAX_N], x[MAX_N], y[MAX_N], k[MAX_N], dam[MAX_N];
bool out[MAX_N];

inline bool isValid(int x, int y, int w, int h) {
    return x >= 0 && x < l && y >= 0 && y < l && x + h - 1 >= 0 && x + h - 1 < l && y + w - 1 >= 0 && y + w - 1 < l;
}

// 밀려난 곳에 벽이 있는지, 함정이 몇 개인지 체크
pair<bool, int> overlapWall(int x, int y, int w, int h) {
    int ans = 0;
    for (int i = x; i < x + h; ++i) {
        for (int j = y; j < y + w; ++j) {
            if (matrix[i][j] == 2) {
                return { false, 0 };
            }
            else if (matrix[i][j] == 1) {
                ++ans;
            }
        }
    }

    return { true, ans };
}

inline bool doRectanglesOverlap(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    // 직사각형이 겹치지 않는 경우
    if (x1 + h1 <= x2 || x2 + h2 <= x1) {
        return false;  // 한 직사각형이 다른 직사각형의 왼쪽 또는 오른쪽에 있는 경우
    }
    if (y1 + w1 <= y2 || y2 + w2 <= y1) {
        return false;  // 한 직사각형이 다른 직사각형의 위쪽 또는 아래쪽에 있는 경우
    }
    // 겹치는 경우
    return true;
}

int dirX[4] = {-1, 0, 1, 0};
int dirY[4] = {0, 1, 0, -1};

void BFS(int idx, int dir) {
    queue<int> qu; //기사 index
    qu.push(idx);

    bool check = true;

    vector<bool> visited(n, false);
    vector<int> damage(n, 0);
    vector<pair<int, int>> pos(n, {-1, -1});
    visited[idx] = true;

    while (!qu.empty()) {
        int nextIdx = qu.front();
        qu.pop();

        int nx = x[nextIdx] + dirX[dir], ny = y[nextIdx] + dirY[dir];

        if (isValid(nx, ny, w[nextIdx], h[nextIdx])) {
            // 밀려난 좌표 추가
            pos[nextIdx] = { nx, ny };
            auto ret = overlapWall(nx, ny, w[nextIdx], h[nextIdx]);
            if (ret.first) {
                // 밀려난 기사가 벽에 겹치지 않은 경우
                for (int i = 0; i < n; ++i) {
                    // 방문하지 않았으며 겹치는 경우
                    if (!out[i] && !visited[i] && doRectanglesOverlap(nx, ny, w[nextIdx], h[nextIdx], x[i], y[i], w[i], h[i])) {
                        visited[i] = true;
                        qu.push(i);
                    }
                }
                // 함장 개수를 데미지에 추가함
                damage[nextIdx] = ret.second;
            }
            else {
                // 밀려난 기사가 벽에 겹치는 경우 이동 불가
                check = false;
                break;
            }
        }
        else {
            // 밀려난 기사가 맵 밖으로 나가는 경우 이동 불가
            check = false;
            break;
        }
    }

    // 벽에 박는 애 없이 다 밀려나면.
    if (check) {
        for (int i = 0; i < n; ++i) {
            // 이동한 본인이면 데미지 입지 않
            if (i != idx) {
                dam[i] += damage[i];
                k[i] -= damage[i];
            }

            if (pos[i] != make_pair(-1, -1)) {
                x[i] = pos[i].first;
                y[i] = pos[i].second;
            }

            if (k[i] <= 0) {
                out[i] = true;
            }
        }
    }
 }

void print()
{
    vector<vector<int>> a(l, vector<int>(l, 0));

    for (int i = 0; i < n; ++i) {
        if (out[i]) continue;
        for (int j = x[i]; j < x[i] + h[i]; ++j) {
            for (int k = y[i]; k < y[i] + w[i]; ++k) {
                a[j][k] = i + 1;
            }
        }
    }

    for (int i = 0; i < l; ++i) {
        for (int j = 0; j < l; ++j) {
            debugFile << a[i][j] << ' ';
        }
        debugFile << '\n';
    }
    debugFile << '\n';
}

void moveKnight() {
    int idx, dir;

    *input >> idx >> dir;
    --idx;

    if (out[idx]) {
        return;
    }

    BFS(idx, dir);

    print();
}

void init() {
    *input >> l >> n >> q;

    for (int i = 0; i < l; ++i) {
        for (int j = 0; j < l; ++j) {
            *input >> matrix[i][j];
        }
    }

    for (int i = 0; i < n; ++i) {
        *input >> x[i] >> y[i] >> h[i] >> w[i] >> k[i];
        --x[i]; --y[i];
        out[i] = false;
    }
}

void solution() {
    for (int i = 0; i < q; ++i) {
        moveKnight();
    }

    int ans = 0;

    for (int i = 0; i < n; ++i) {
        if (!out[i]) {
            ans += dam[i];
        }
    }

    *output << ans << '\n';
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