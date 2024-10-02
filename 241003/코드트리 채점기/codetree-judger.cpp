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
    string domain;
    int id, prority;
    int tick;
};

bool compare(problem a, problem b) {
    if (a.prority != b.prority) {
        return b.prority < a.prority;
    }

    return a.tick > b.tick;
}

using PriorityQueue = priority_queue<problem, vector<problem>, decltype(&compare)>;

unordered_map<int, pair<int, int>> history;
priority_queue<int, vector<int>, greater<int>> availableJudge;
vector<problem> judge;
unordered_set<int> isJudgeDomain;
unordered_map<int, unordered_set<int>> waitingDomain; // 해당 url이 대기 중인지
unordered_map<int, PriorityQueue > pq;
unordered_map<int, string> idxToDomain;
map<string, int> domainToIdx;

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
    string s;

    *input >> n >> s;

    auto dom = splitDomain(s);
    int idx = domainToIdx[dom.first];

    if (!idx) {
        ++domIndex;
        idx = domIndex;
        domainToIdx[dom.first] = domIndex;
        idxToDomain[domIndex] = dom.first;
    }

    pq[idx] = PriorityQueue(compare);

    pq[idx].push({ dom.first, dom.second, 1, 0 });

    waitingDomain[idx].insert(domIndex);

    judge = vector<problem>(n);

    for (int i = 0; i < n; ++i) {
        availableJudge.push(i);
    }
    ++ans;
}

void requestJudge() {
    int tick, prio;
    string s;

    *input >> tick >> prio >> s;

    auto dom = splitDomain(s);

    auto idx = domainToIdx[dom.first];

    if (!idx) {
        domainToIdx[dom.first] = domIndex;
        idxToDomain[domIndex] = dom.first;
        ++domIndex;
    } 
    if (idx && waitingDomain[idx].count(dom.second) > 0) {
        return;
    }

    waitingDomain[idx].insert(dom.second);

    if (pq.find(idx) == pq.end()) {
        pq[idx] = PriorityQueue(compare);
    }

    pq[idx].push({ dom.first, dom.second, prio, tick });
    ++ans;
    
}

void tryJudge() {

    int tick;

    *input >> tick;

    if (availableJudge.empty()) {
        return;
    }

    vector<problem> remain;

    int min = 999999999;
    problem url;
    int minIdx = -1;

    for (auto val : pq) {
        int idx = val.first;

        if (val.second.empty()) {
            continue;
        }
        auto top = val.second.top();

        if (isJudgeDomain.count(idx) > 0) {
            continue;
        }
        else if (tick < history[idx].second) {
            continue;
        }

        if (min > top.prority) {
            min = top.prority;
            url = top;
            minIdx = idx;
        }
        else if (min == top.prority && url.tick > top.tick) {
            min = top.prority;
            url = top;
            minIdx = idx;
        }
    }

    if (min != 999999999) {
        pq[minIdx].pop();
        int jud = availableJudge.top();
        availableJudge.pop();

        url.tick = tick;
        judge[jud] = url;
        isJudgeDomain.insert(minIdx);
        waitingDomain[minIdx].erase(url.id);
        --ans;
    }
}

void stopJudge() {
    int tick, num;

    *input >> tick >> num;

    availableJudge.push(num - 1);
    
    auto prom = judge[num - 1];
    int gap = tick - prom.tick;

    auto idx = domainToIdx[prom.domain];

    isJudgeDomain.erase(idx);

    history[idx] = { prom.tick, prom.tick + gap * 3 };
}

void printWaiting() {
    int tick;

    *input >> tick;

    *output << ans << '\n';
}


int main() {
    int q;
    ans = 0; domIndex = 0;
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