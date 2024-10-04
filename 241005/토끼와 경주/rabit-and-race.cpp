#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

#define INF 1987654321

struct Rabbit {
    int id, jumpCount = 0, r = 1, c = 1;

    bool operator <(const Rabbit& b) const {
        if (jumpCount != b.jumpCount) return jumpCount > b.jumpCount;

        if (r + c != b.r + b.c) return r + c > b.r + b.c;

        if (c != b.c) return c > b.c;

        if (r != b.r) return r > b.r;

        return id > b.id;
    }
};

bool compareAddScore(const Rabbit& a, const Rabbit& b) {
    if (a.r + a.c != b.r + b.c) return a.r + a.c > b.r + b.c;

    if (a.c != b.c) return a.c > b.c;

    if (a.r != b.r) return a.r > b.r;

    return a.id > b.id;
}

int p, n, m;

int ans;

priority_queue<Rabbit> pq;

long long allScore = 0;
unordered_map<int, long long> minusScore;
unordered_map<int, int> rabbitDistance;
int minMInusScore = 0;
bool allMInusScore;
unordered_set<int> minusRabbit;


int dirR[4] = { 1, -1, 0, 0 };
int dirC[4] = { 0, 0, -1, 1 };

// 경주 시작 준비
// P 마리 토끼가 N X M 격자 위에서 경주
// 토끼에 고유 번호가 있다. (map?)
// 한번 움직일 때 꼭 이동해야 하는 거리도 정해져 있다. i 번 토끼는 고유한 id와 이동 거리 d가 존재
// 토끼는 전부 1,1 에 있음.
void initRace() {
    *input >> n >> m >> p;

    for (int i = 0; i < p; ++i) {
        Rabbit rab;
        int d;

        *input >> rab.id >> d;
        rabbitDistance[rab.id] = d;

        pq.push(rab);
    }
}

// 경주 진행
// 가장 우선순위가 높은 토끼를 뽑아 (우선순위 큐) 멀리 보내주는 것을 k번 반복
// 우선순위는 순서대로 (현재까지의 총 점프 횟수가 적은 토끼, 현재 서잇는 행 번호 + 열 번호가 작은 토끼, 행 번호가 작은 토끼, 열번호가 작은 토끼, 고유 번호가 작은 토끼) 순으로 이루어짐.
// 우선순위가 가장 높은 토끼가 결정되면 토끼를 i번 이라 할 때 상하좌우 d 거리 이동했을 때 위치를 구함. 이동하는 도중 격자를 벗어나면 방향을 (반대)로 바꿔 한 칸 이동
// 구한 4개의 위치 중 (행 번호 + 열번호가 큰 칸, 행 번호가 큰 칸, 열번호가 큰 칸) 순서로 우선순위를 두고 가장 우선 순위가 높은 칸을 골라 그 위치로 이동
// 
// 이 위치를 r, c라 할 때, 움직인 토끼 제외 나머지는 r+c 만큼 점수를 얻음
// k번 반복 후, p-1 토끼가 얻어야할 총 점수를 x라 두고 각 토끼 별로 점수를 받지 못한 것을 배열에 -로 기록 
// 
// k 번동안 우선순위 높은 토끼를 뽑아 멀리 이동시키기 (여러 토끼가 계속 선택 될 수 있음)
// 모든 턴이 진행되면 (현재 서있는 행 + 현재 서있는 열, 행, 열, 고유번호) 순으로 우선순위 두고 가장 우선순위가 높은 토끼를 골라 점수 S를 더함


// k 번의 턴 동안 꼭 한번 뽑혔던 토끼 중 우선 순위가 높은 것을 골라야 함.

bool comparePos(pair<int, int> a, pair<int, int> b) {
    if (a.first + a.second != b.first + b.second) return a.first + a.second > b.first + b.second;
    if (a.first != b.first) return a.first > b.first;
    if (a.second != b.second) return  a.second > b.second;
}

void move(Rabbit& rab) {
    int d = rabbitDistance[rab.id];

    pair<int, int> maxCR = { -1, -1 };

    for (int i = 0; i < 4; ++i) {
        int r, c;

        r = rab.r + dirR[i] * d;
        c = rab.c + dirC[i] * d;

        r = r % (2 * m - 2);
        c = c % (2 * n - 2);

        if (r <= 0) {
            r = r * (-1) + 2;
        }

        if (c <= 0) {
            c = c * (-1) + 2;
        }

        if (r > m) {
            r = 2 * m - r;
        }

        if (c > n) {
            c = 2 * n - c;
        }

        if (maxCR.first == -1) {
            maxCR = { c, r };
        }
        else if (comparePos({ c, r }, maxCR)) {
            maxCR = { c, r };
        }
    }

    rab.r = maxCR.second;
    rab.c = maxCR.first;
}

//bool comparePos(Rabbit a, Rabbit b) {
//    if (a.r + a.c != b.r + b.c) return a.r + a.c > b.r + b.c;
//    if (a.c != b.c ) return a.c  > b.c ;
//    if (a.r !=  b.r) return  a.r > b.r;
//}
//
//// 토끼를 위로 이동시킵니다.
//Rabbit GetUpRabbit(Rabbit cur_rabbit, int dis) {
//    Rabbit up_rabbit = cur_rabbit;
//    dis %= 2 * (n - 1);
//
//    if (dis >= up_rabbit.c - 1) {
//        dis -= (up_rabbit.c - 1);
//        up_rabbit.c = 1;
//    }
//    else {
//        up_rabbit.c -= dis;
//        dis = 0;
//    }
//
//    if (dis >= n - up_rabbit.c) {
//        dis -= (n - up_rabbit.c);
//        up_rabbit.c = n;
//    }
//    else {
//        up_rabbit.c += dis;
//        dis = 0;
//    }
//
//    up_rabbit.c -= dis;
//
//    return up_rabbit;
//}
//
//// 토끼를 아래로 이동시킵니다.
//Rabbit GetDownRabbit(Rabbit cur_rabbit, int dis) {
//    Rabbit down_rabbit = cur_rabbit;
//    dis %= 2 * (n - 1);
//
//    if (dis >= n - down_rabbit.c) {
//        dis -= (n - down_rabbit.c);
//        down_rabbit.c = n;
//    }
//    else {
//        down_rabbit.c += dis;
//        dis = 0;
//    }
//
//    if (dis >= down_rabbit.c - 1) {
//        dis -= (down_rabbit.c - 1);
//        down_rabbit.c = 1;
//    }
//    else {
//        down_rabbit.c -= dis;
//        dis = 0;
//    }
//
//    down_rabbit.c += dis;
//
//    return down_rabbit;
//}
//
//// 토끼를 왼쪽으로 이동시킵니다.
//Rabbit GetLeftRabbit(Rabbit cur_rabbit, int dis) {
//    Rabbit left_rabbit = cur_rabbit;
//    dis %= 2 * (m - 1);
//
//    if (dis >= left_rabbit.r - 1) {
//        dis -= (left_rabbit.r - 1);
//        left_rabbit.r = 1;
//    }
//    else {
//        left_rabbit.r -= dis;
//        dis = 0;
//    }
//
//    if (dis >= m - left_rabbit.r) {
//        dis -= (m - left_rabbit.r);
//        left_rabbit.r = m;
//    }
//    else {
//        left_rabbit.r += dis;
//        dis = 0;
//    }
//
//    left_rabbit.r -= dis;
//
//    return left_rabbit;
//}
//
//// 토끼를 오른쪽으로 이동시킵니다.
//Rabbit GetRightRabbit(Rabbit cur_rabbit, int dis) {
//    Rabbit right_rabbit = cur_rabbit;
//    dis %= 2 * (m - 1);
//
//    if (dis >= m - right_rabbit.r) {
//        dis -= (m - right_rabbit.r);
//        right_rabbit.r = m;
//    }
//    else {
//        right_rabbit.r += dis;
//        dis = 0;
//    }
//
//    if (dis >= right_rabbit.r - 1) {
//        dis -= (right_rabbit.r - 1);
//        right_rabbit.r = 1;
//    }
//    else {
//        right_rabbit.r -= dis;
//        dis = 0;
//    }
//
//    right_rabbit.r += dis;
//
//    return right_rabbit;
//}
//
//void move(Rabbit& rab) {
//    int d = rabbitDistance[rab.id];
//    pair<int, int> maxCR = { -1, -1 };
//
//    Rabbit nex_rabbit = rab;
//    nex_rabbit.r = 0; nex_rabbit.c = 0;
//
//    Rabbit up_rabbit = GetUpRabbit(rab, d);
//    // 지금까지의 도착지들보다 더 멀리 갈 수 있다면 도착지를 갱신합니다.
//    if (!comparePos(nex_rabbit, up_rabbit)) nex_rabbit = up_rabbit;
//
//
//    // 토끼를 아래로 이동시킵니다.
//    Rabbit down_rabbit = GetDownRabbit(rab, d);
//    // 지금까지의 도착지들보다 더 멀리 갈 수 있다면 도착지를 갱신합니다.
//    if (!comparePos(nex_rabbit, down_rabbit)) nex_rabbit = down_rabbit;
//
//
//    // 토끼를 왼쪽으로 이동시킵니다.
//    Rabbit left_rabbit = GetLeftRabbit(rab, d);
//    // 지금까지의 도착지들보다 더 멀리 갈 수 있다면 도착지를 갱신합니다.
//    if (!comparePos(nex_rabbit, left_rabbit)) nex_rabbit = left_rabbit;
//
//
//    // 토끼를 오른쪽으로 이동시킵니다.
//    Rabbit right_rabbit = GetRightRabbit(rab, d);
//    // 지금까지의 도착지들보다 더 멀리 갈 수 있다면 도착지를 갱신합니다.
//    if (!comparePos(nex_rabbit, right_rabbit)) nex_rabbit = right_rabbit;
//
//    rab.r = nex_rabbit.r; rab.c = nex_rabbit.c;
//}

void processRace() {
    int k, s;
    *input >> k >> s;

    Rabbit bestRabbit;
    bestRabbit.r = -1;
    bestRabbit.c = -1;

    // k번 이동 진행
    for (int i = 0; i < k; ++i) {
        if (!pq.empty()) {
            auto rab = pq.top();
            pq.pop();

            // 이동 진행
            ++rab.jumpCount;
            move(rab);

            pq.push(rab);

            int score = rab.r + rab.c;

            // 움직인 토끼를 set과 점수 업데이트
            minusRabbit.insert(rab.id);
            minusScore[rab.id] += score;

            // 총 점수 증가
            allScore += score;

            // 모든 토끼가 점수를 얻게되지 못한 경우가 있다면, 가장 적게 점수를 잃은 토끼가 최대 점수
            if (!allMInusScore && minusRabbit.size() >= p) {
                allMInusScore = true;
            }

            // 최소로 점수를 잃은 토끼 최신화
            if (minMInusScore > minusScore[rab.id]) {
                minMInusScore = minusScore[rab.id];
            }

            // 점수 S를 더하기 위한 토끼 선정
            if (compareAddScore(rab, bestRabbit)) {
                bestRabbit = rab;
            }
        }
    }

    // 마이너스 점수 된 것에서 얻은 s 점수 만큼 추가
    minusScore[bestRabbit.id] -= s;

    // 최소 마이너스 점수 업데이트
    if (allMInusScore) {
        if (minusScore[bestRabbit.id] < minMInusScore) {
            minMInusScore = minusScore[bestRabbit.id];
        }
    }
    else if (minusScore[bestRabbit.id] < 0) {
        // 이 경우는 총 점수보다 s를 더한 값이 크므로 모든 토끼가 점수를 잃었다고 가정해도 됨.
        minMInusScore = minusScore[bestRabbit.id];
        allMInusScore = true;
    }

}

// 이동거리 변경
// 고유 번호 id의 토끼의 이동거리를 L배 함. 최대 거리는 10억을 넘어가지 않음

void changeDistance() {
    int pid, d;

    *input >> pid >> d;

    rabbitDistance[pid] = rabbitDistance[pid] * d;
}

// 최고의 토끼 선정
// 각 토끼가 모든 경주를 진행하며 얻은 점수 중 가장 높은 점수 출력
void printBestScore() {
    // 총 점수에서 최소로 잃은 점수가 최대 점수
    // 만약 모든 토끼가 점수를 잃은 상황이 아니라면 총 점수가 가장 큼
    /*if (allMInusScore) {
        *output << allScore - minMInusScore << '\n';
    }
    else {
        *output << allScore << '\n';
    }*/

    long long ans = 0;

    for (auto val : minusScore) {
        auto minus = val.second;

        ans = max(ans, allScore - minus);
    }
    *output << ans << '\n';
}


int main() {
    int q;
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    *input >> q;

    allScore = 0;
    minMInusScore = INF;
    allMInusScore = false;

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        switch (code) {
        case 100:
            //cout << "100\n";
            initRace();
            break;
        case 200:
            //cout << "200\n";
            processRace();
            break;
        case 300:
            //cout << "300\n";
            changeDistance();
            break;
        case 400:
            printBestScore();
            break;
        }
    }
    return 0;
}