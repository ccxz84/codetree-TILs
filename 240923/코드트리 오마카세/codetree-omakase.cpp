#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// L개의 원형 칸이 있음.
// L개의 의자가 있음.
// 시각 t가 존재

// 모든 동작은 회전이 일어난 직후에 발생

// 초밥 제작
// 시각 t에 벨트 위에 name을 부착한 초밥을 x에 올려놓음
// 같은 위치에 여러 초밥 있을 수 있음.

// 손님 입장
// name 사람이 시각 t에 x에 앉음.
// x 앞으로 오는 자신 이름의 초밥을 n개 먹고 떠남.
// 명령이 주어진 때에 해당 위치에는 사람이 없음. (사람이 같은 위치에 겹칠 수 없음)
// 시각 t에 x에 같은 name의 초밥이 있으면 그 자리에서 먹게 됨 (복수 개도 가능) (초밥 먹는 시간은 0)

// 사진 촬영
// t 시간에 오마카세 집 촬영
// 이 동작은 회전 -> 식사 가 끝난 후 진행됨.
// 사람 수와 초밥 수를 출력

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

int L;

struct sushi {
    int pos, tick;
};

struct people {
    int pos, tick, num;
};

//vector<sushi> sushiList;
unordered_map <string, vector<sushi>> sushiList;
unordered_map<string, people> peopleList;

int sushiCount;

// 초밥 리스트를 우선 순위 큐로 저장
// 정렬 식 사람.tick > 초밥.tick >= 0 ? ((사람.tick - 초밥.tick) + 초밥.pos) % L : 초밥.pos = 초밥 위치
// 위치 계산 (y-x) >= 0 ? y-x : L - abs(y-x)
// x = 초밥 위치, y = 사람 위치
// 사람이 초밥을 먹는 시간은 사람.tick + 사람, 초밥 거리

// 사람, 초밥을 마구잡이로 입력 받고
// 출력 때, 정렬해서 사람 먹이고 한번에 내보내기
// 먹는데 걸리는 시간보다 출력하는 tick이 작은 경우 패스

void addSushi() {
    sushi newSushi;
    string name;
    *input >> newSushi.tick >> newSushi.pos >> name;

    ++sushiCount;

    sushiList[name].push_back(newSushi);
}

void addPerson() {
    people newPeople;
    string name;

    *input >> newPeople.tick >> newPeople.pos >> name >> newPeople.num;

    peopleList[name] = newPeople;
}

inline int getDistance(int sushiPos, int personPos) {
    if (personPos >= sushiPos) {
        return personPos - sushiPos;
    }
    return L - (sushiPos - personPos);
}

inline int getDistance(people person, sushi su) {
    int sushiPos, offset = 0;
    if (person.tick - su.tick >= 0) {
        sushiPos = ((person.tick - su.tick) + su.pos) % 5L;
        offset = person.tick - su.tick;
    }
    else {
        sushiPos = su.pos;
    }

    int ret = getDistance(sushiPos, person.pos) + offset;
    return ret;

    //return getDistance(sushiPos, person.pos) + offset;
}

//int runcount;

void printResult() {
    //++runcount;
    int tick;

    *input >> tick;
    
    unordered_map<string, people> newPeople;

    for (auto it = peopleList.begin(); it != peopleList.end(); ++it) {
        string name = it->first;
        people person = it->second;

        vector<sushi> backup(sushiList[name]);
        vector<sushi> newList(0);
        int i = 0;

        for (; i < backup.size(); ++i) {
            int dis = getDistance(person, backup[i]);

            // debugFile << backup[i].name << ' ' << dis << '\n';

            if (backup[i].tick + dis > tick) {
                newList.push_back(backup[i]);
                continue;
            }
            --sushiCount;
            --person.num;

            // 여기서 사람의 초밥이 다 먹었을 경우 삭제
            if (person.num == 0) {
                ++i;
                break;
            }
        }

        if (person.num != 0) {
            newPeople[name] = person;
        }

        newList.insert(newList.end(), backup.begin() + i, backup.end());
        

        sushiList[name] = newList;
    }

    peopleList = newPeople;

    //debugFile << '\n';
    *output << peopleList.size() << ' ' << sushiCount <<'\n';
}

int main() {
    int q;

    //runcount = 0;
    sushiCount = 0;

    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    *input >> L >> q;

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        switch (code) {
        case 100:
            //cout << "100\n";
            addSushi();
            break;
        case 200:
            //cout << "200\n";
            addPerson();
            break;
        case 300:
            //cout << "300\n";
            printResult();
            break;
        }
    }
    return 0;
}