#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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

unordered_map<string, pair<int, int>> history;
priority_queue<int, vector<int>, greater<int>> availableJudge;
vector<problem> judge;
unordered_set<string> isJudgeDomain;
unordered_set<string> waitingDomain;
priority_queue<problem, vector<problem>, function<bool(problem, problem)>> pq(compare);

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

int n;

void initJudge() {
    string s;

    *input >> n >> s;

    auto dom = splitDomain(s);

    pq.push({ dom.first, dom.second, 1, 0 });

    waitingDomain.insert(s);

    judge = vector<problem>(n);

    for (int i = 0; i < n; ++i) {
        availableJudge.push(i);
    }
}

void requestJudge() {
    int tick, prio;
    string s;

    *input >> tick >> prio >> s;

    if (waitingDomain.count(s) > 0) {
        return;
    }

    waitingDomain.insert(s);

    auto dom = splitDomain(s);

    pq.push({ dom.first, dom.second, prio, tick });
    
}

void tryJudge() {

    int tick;

    *input >> tick;

    if (availableJudge.empty()) {
        return;
    }

    vector<problem> remain;
    while (!pq.empty()) {
        auto cur = pq.top();
        pq.pop();

        if (isJudgeDomain.count(cur.domain) > 0) {
            remain.push_back(cur);
            continue;
        }
        else if (tick < history[cur.domain].second) {
            remain.push_back(cur);
            continue;
        }
        else if (availableJudge.empty()) {
            remain.push_back(cur);
            continue;
        }

        int jud = availableJudge.top();
        availableJudge.pop();

        cur.tick = tick;
        judge[jud] = cur;
        isJudgeDomain.insert(cur.domain);
        string s = cur.domain + "/";
        s += to_string(cur.id);
        waitingDomain.erase(s);

        break;
    }
    
    for (int i = 0; i < remain.size(); ++i) {
        pq.push(remain[i]);
    }
}

void stopJudge() {
    int tick, num;

    *input >> tick >> num;

    availableJudge.push(num - 1);
    
    auto prom = judge[num - 1];
    int gap = tick - prom.tick;

    isJudgeDomain.erase(prom.domain);

    history[prom.domain] = { prom.tick, prom.tick + gap * 3 };
}

void printWaiting() {
    int tick;

    *input >> tick;

    *output << pq.size() << '\n';
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