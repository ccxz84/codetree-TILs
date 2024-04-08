#include <iostream>
#include <vector>
#include <cmath>
#include <climits>

using namespace std;

vector<int> solution(int n, int m, int p, int c, int d, vector<struct pos>& santa, struct pos rudol);

int dirR[4] = {1, 0, -1, 0};
int dirC[4] = {0, 1, 0, -1};

#define ru -2
#define none -1

struct pos {
    int r;
    int c;
};

int main() {
    int n,m,p,c,d;
    vector<struct pos> santa;
    struct pos rudol;
    vector<int> score;

    cin>>n>>m>>p>>c>>d;

    santa = vector<struct pos>(p);

    cin>>rudol.r>>rudol.c;
    rudol.r -= 1;
    rudol.c -= 1;
    for(int i = 0; i < p; ++i) {
        int index, row, column;
        cin>>index>>row>>column;
        santa[index-1].r = row - 1;
        santa[index-1].c = column - 1;
    }

    score = solution(n, m, p, c, d, santa, rudol);

    for(int i = 0; i < p; ++i) {
        cout<<score[i]<<' ';
    }
    cout<<'\n';
    return 0;
}

int find_santa(vector<struct pos>& santa, struct pos rudol, vector<bool>& out) {
    int index = -1;
    int dis = INT_MAX;

    for (int i = 0; i < santa.size(); ++i) {
        if (out[i]) continue;

        bool qe = false;
        int diff = (santa[i].r - rudol.r) * (santa[i].r - rudol.r) 
        + (santa[i].c - rudol.c) * (santa[i].c - rudol.c);

        if (diff < dis) {
            qe = true;
        } else if (diff == dis) {
            if (santa[index].r < santa[i].r) {
                qe = true;
            } else if (santa[index].r == santa[i].r && santa[index].c < santa[i].c) {
                qe = true;
            }
        }

        if (qe) {
            dis = diff;
            index = i;
        }
    }
    return index;
}

bool isOutOfBounds(int r, int c, int n) {
    return r >= n || r < 0 || c >= n || c < 0;
}

void check_knockback(vector<struct pos>& santa, vector<vector<int>>& board, vector<bool>& out, struct pos dir, int index, int n) {
    int next = board[santa[index].r][santa[index].c];
    while(next > 0) {
        int i = next;
        int newR = santa[i].r - dir.r;
        int newC = santa[i].c - dir.c;

        if (!isOutOfBounds(newR, newC, n)) {
            board[santa[i].r][santa[i].c] = none;
            santa[i].r = newR;
            santa[i].c = newC;
            next = board[newR][newC];
            board[newR][newC] = i;
        } else {
            board[santa[i].r][santa[i].c] = none;
            out[i] = true;
            next = 0;
        }
    }
}

vector<int> solution(int n, int m, int p, int c, int d, vector<struct pos>& santa, struct pos rudol) {
    vector<vector<int>> board(n, vector<int>(n, none));
    vector<int> stun(p, 0);
    vector<bool> out(p, false);
    vector<int> score(p, 0);

    board[rudol.r][rudol.c] = ru;

    for(int i = 0; i < p; ++i) {
        board[santa[i].r][santa[i].c] = i;
    }

    for(int z = 0; z < m; ++z) {
        int index = find_santa(santa, rudol, out);
        struct pos dir;

        if (index == -1) break;

        dir.r = (santa[index].r == rudol.r) ? 0 : ((santa[index].r - rudol.r) / abs(santa[index].r - rudol.r));
        dir.c = (santa[index].c == rudol.c) ? 0 : ((santa[index].c - rudol.c) / abs(santa[index].c - rudol.c));

        if (board[rudol.r + dir.r][rudol.c + dir.c] > none) {
            // 새로운 위치를 계산
            int newR = santa[index].r + dir.r * d;
            int newC = santa[index].c + dir.c * d;

            // 경계를 벗어나는지 체크
            if (isOutOfBounds(newR, newC, n)) {
                board[santa[index].r][santa[index].c] = none;
                out[index] = true;
            } else {
                // 위치 업데이트
                board[santa[index].r][santa[index].c] = none;
                santa[index].r = newR;
                santa[index].c = newC;
                check_knockback(santa, board, out, dir, index, n); // 이 함수의 세부 구현은 여기서 생략
                board[newR][newC] = index;
            }
            score[index] += c;
            stun[index] += 2;
        } 

        board[rudol.r][rudol.c] = none;
        rudol.r += dir.r;
        rudol.c += dir.c;
        board[rudol.r][rudol.c] = ru;

        for (int i = 0; i < p; ++i) {
            if (out[i]) continue;
            if (stun[i] > 0) {
                --stun[i];
                ++score[i];
                continue;
            }

            struct pos dir;
            int diff = (santa[i].r - rudol.r) * (santa[i].r - rudol.r) + (santa[i].c - rudol.c) * (santa[i].c - rudol.c);

            for(int j = 0; j < 4; ++j) {
                int newR = santa[i].r + dirR[j];
                int newC = santa[i].c + dirC[j];

                if (isOutOfBounds(newR, newC, n) || board[newR][newC] > none) continue;

                int temp = (newR - rudol.r) * (newR - rudol.r) + (newC - rudol.c) * (newC - rudol.c);

                if (temp < diff) {
                    dir.r = dirR[j];
                    dir.c = dirC[j];
                    diff = temp;
                }
            }

            if (board[santa[i].r + dir.r][santa[i].c + dir.c] == ru) {
                int newR = santa[i].r - dir.r * (d - 1);
                int newC = santa[i].c - dir.c * (d - 1);

                if (isOutOfBounds(newR, newC, n)) {
                    board[santa[i].r][santa[i].c] = none;
                    out[i] = true;
                    score[i] -= 1;

                } else {
                    // 위치 업데이트
                    board[santa[i].r][santa[i].c] = none;
                    santa[i].r = newR;
                    santa[i].c = newC;
                    check_knockback(santa, board, out, dir, i, n);
                    board[newR][newC] = i;
                }
                score[i] += d + 1;
                stun[i] += 1;
            } else {
                int newR = santa[i].r + dir.r;
                int newC = santa[i].c + dir.c;

                board[santa[i].r][santa[i].c] = none;
                santa[i].r = newR;
                santa[i].c = newC;
                board[newR][newC] = i;
                ++score[i];
            }
        }
    }

    return score;
}