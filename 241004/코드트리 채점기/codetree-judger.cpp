#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <map>
using namespace std;

#define INF 1987654321
#define MAX_N 50000
#define MAX_D 300

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

// N개의 채점기 준비
// 문제는 도메인/문제ID 꼴로 주어짐

// 채점 요청
// T 초에 우선순위 P면서 U에 대한 문제 채점 요청 들어옴
// 문제는 채점 대기 큐로 들어감

// 채점 시도
// T초에 대기 큐에서 채점이 가능한 문제를 뽑아 채점 진행
// 도메인이 이미 채점기에서 채점 중인 경우, 도메인이 START + 3 X GAP 전에 채점이 끝난 문제인 경우 채점 불가
// 우선순위가 작으면 높음
// 대기 큐에 들어온 시간이 빠르면 높음
// 쉬고 있는 채점기가 있으면 시작

// 채점 종료
// T초에 J 번 채점기가 종료됨

// 채점 대기큐 조회
// 대기 중인 TASK 수 출력

struct problem {
    int id, prority;
    int tick;
};

bool compare(problem a, problem b) {
    if (a.prority != b.prority) {
        return a.prority > b.prority;
    }

    return a.tick > b.tick;
}

using PriorityQueue = priority_queue<problem, vector<problem>, decltype(&compare)>;

string idxToDomain[MAX_D + 1] = { "" };
map<string, int> domainToIdx;

vector<PriorityQueue> waitingProblem = vector<PriorityQueue>(MAX_D + 1, PriorityQueue(compare));
int judgementDomain[MAX_N + 1] = { 0 };
bool isJudgementDomain[MAX_D + 1] = { false };

int s[MAX_D + 1];
int g[MAX_D + 1];
int e[MAX_D + 1];

priority_queue<int, vector<int>, greater<int>> waitingJudger;
unordered_set<int> waitingProblemSet[MAX_D + 1];


pair<string, int> splitDomain(string s) {
    std::istringstream ss(s);
    std::string domain;
    std::string number_str;
    int number;

    if (std::getline(ss, domain, '/') && std::getline(ss, number_str)) {
        number = std::stoi(number_str);
    }

    return { domain, number };
}

int n, ans, domIndex;

void initJudge() {
    string url;

    *input >> n >> url;

    auto dom = splitDomain(url);

    int idx = domainToIdx[dom.first];

    if (idx == 0) {
        ++domIndex;
        idx = domIndex;
        idxToDomain[idx] = dom.first;
        domainToIdx[dom.first] = idx;
    }

    problem prom = { dom.second, 1, 0 };

    waitingProblem[idx].push(prom);
    waitingProblemSet[idx].insert(prom.id);

    for (int i = 1; i <= n; ++i) {
        waitingJudger.push(i);
    }
    ++ans;
}

void requestJudge() {
    problem prom;
    string url;

    *input >> prom.tick >> prom.prority >> url;

    auto dom = splitDomain(url);

    int idx = domainToIdx[dom.first];

    if (idx == 0) {
        ++domIndex;
        idx = domIndex;
        idxToDomain[idx] = dom.first;
        domainToIdx[dom.first] = idx;
    }

    if (waitingProblemSet[idx].count(dom.second) > 0) {
        return;
    }

    prom.id = dom.second;

    waitingProblem[idx].push(prom);
    waitingProblemSet[idx].insert(prom.id);
    ++ans;
}

void tryJudge() {
    int tick;

    *input >> tick;

    int minIdx = -1;
    int min = INF;
    problem url;

    if (waitingJudger.empty()) return;

    for (int i = 1; i <= domIndex; ++i) {
        if (waitingProblem[i].empty()) {
            continue;
        }

        auto prom = waitingProblem[i].top();

        if (tick < e[i]) {
            continue;
        }

        if (min > prom.prority) {
            min = prom.prority;
            minIdx = i;
            url = prom;
        }
        else if (min == prom.prority && url.tick > prom.tick) {
            min = prom.prority;
            minIdx = i;
            url = prom;
        }
    }

    if (min != INF) {
        --ans;
        waitingProblem[minIdx].pop();
        waitingProblemSet[minIdx].erase(waitingProblemSet[minIdx].find(url.id));
        int judgementId = waitingJudger.top();
        waitingJudger.pop();

        //isJudgementDomain[minIdx] = true;
        judgementDomain[judgementId] = minIdx;
        s[minIdx] = tick;
        e[minIdx] = INF;
    }
}

void stopJudge() {
    int tick, judgementId;

    *input >> tick >> judgementId;

    if (judgementDomain[judgementId] == 0) return;

    int domainIdx = judgementDomain[judgementId];

    int gap = tick - s[domainIdx];

    e[domainIdx] = s[domainIdx] + (3 * gap);
    g[domainIdx] = gap;
    waitingJudger.push(judgementId);
    judgementDomain[judgementId] = 0;
    //isJudgementDomain[domainIdx] = false;
}

int runcount;

void printWaiting() {
    int tick;

    *input >> tick;

    *output << ans << '\n';
}

int main() {
    int q;
    ans = 0; domIndex = 1;
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    *input >> q;

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        switch (code) {
        case 100:
            //cout << "100\n";
            initJudge();
            break;
        case 200:
            //cout << "200\n";
            requestJudge();
            break;
        case 300:
            //cout << "300\n";
            tryJudge();
            break;
        case 400:
            stopJudge();
            break;
        case 500:
            printWaiting();
            break;
        }
    }
    return 0;
}