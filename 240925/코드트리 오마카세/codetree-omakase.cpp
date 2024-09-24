#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

int L;

struct Sushi {
    int pos, tick;
    bool operator<(const Sushi& other) const {
        return tick > other.tick; // 우선순위 큐에서 작은 tick이 우선
    }
};

struct Person {
    int pos, tick, num;
};

// 초밥은 우선순위 큐로 관리
unordered_map<string, priority_queue<Sushi>> sushiMap;
unordered_map<string, Person> personMap;

int sushiCount = 0;

// 초밥 추가
void addSushi() {
    Sushi newSushi;
    string name;
    *input >> newSushi.tick >> newSushi.pos >> name;

    sushiMap[name].push(newSushi);
    ++sushiCount;
}

// 사람 추가
void addPerson() {
    Person newPerson;
    string name;
    *input >> newPerson.tick >> newPerson.pos >> name >> newPerson.num;

    personMap[name] = newPerson;
}

// 초밥 위치와 사람 위치의 거리 계산
inline int getDistance(int sushiPos, int personPos) {
    if (personPos >= sushiPos) {
        return personPos - sushiPos;
    }
    return L - (sushiPos - personPos);
}

inline int getEffectiveDistance(const Person& person, const Sushi& sushi) {
    // 초밥이 놓인 시점에서 얼마나 돌아갔는지 계산
    int offset = person.tick - sushi.tick;
    int sushiPos = (sushi.pos + offset) % L;
    return getDistance(sushiPos, person.pos);
}

// 결과 출력 및 처리
void printResult(int currentTime) {
    int remainingSushi = sushiCount;
    int peopleEating = 0;

    // 사람들을 순차적으로 처리
    vector<string> toRemove;
    for (auto& entry : personMap) {
        string personName = entry.first;
        Person& person = entry.second;

        // 해당 사람이 먹을 초밥이 있는지 확인
        auto& sushiQueue = sushiMap[personName];
        while (!sushiQueue.empty() && person.num > 0) {
            Sushi topSushi = sushiQueue.top();
            int dist = getEffectiveDistance(person, topSushi);

            // 해당 초밥을 먹을 수 있는지 확인
            if (topSushi.tick + dist > currentTime) break;

            // 초밥을 먹음
            sushiQueue.pop();
            --person.num;
            --remainingSushi;

            if (person.num == 0) {
                toRemove.push_back(personName);
            }
        }
    }

    // 초밥을 모두 먹은 사람 제거
    for (const string& name : toRemove) {
        personMap.erase(name);
    }

    // 결과 출력
    *output << personMap.size() << " " << remainingSushi << '\n';
}

int main() {

    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }
    int q;
    *input >> L >> q;

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        switch (code) {
        case 100:
            addSushi();
            break;
        case 200:
            addPerson();
            break;
        case 300: {
            int currentTime;
            *input >> currentTime;
            printResult(currentTime);
            break;
        }
        }
    }

    return 0;
}