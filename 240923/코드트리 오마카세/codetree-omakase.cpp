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
    string name;
    int pos, tick;
};

struct people {
    int pos, tick, num;
};

vector<sushi> sushiList;
unordered_map<string, people> peopleList;

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
    *input >> newSushi.tick >> newSushi.pos >> newSushi.name;

    sushiList.push_back(newSushi);
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
        sushiPos = ((person.tick - su.tick) + su.pos) % L;
        offset = person.tick - su.tick;
    }
    else {
        sushiPos = su.pos;
    }

    //int ret = getDistance(sushiPos, person.pos) + offset;
    //return ret;

    return getDistance(sushiPos, person.pos) + offset;
}


bool compareSushi(sushi a, sushi b) {
    int disA, disB;

    if (peopleList.find(a.name) == peopleList.end()) {
        return false;
    }
    else {
        people person = peopleList[a.name];

        disA = getDistance(person, a);
    }

    if (peopleList.find(b.name) == peopleList.end()) {
        return true;
    }
    else {
        people person = peopleList[a.name];
        disB = getDistance(person, a);
    }

    return disA < disB;
}

//int runcount;

void printResult() {
    //++runcount;
    int tick;

    *input >> tick;

    vector<sushi> backup(sushiList);
    sushiList = vector<sushi>(0);

    //sort(sushiList.begin(), sushiList.end(), compareSushi);
    //debugFile <<"count: "<< runcount <<" tick : " << tick << '\n';

    for (int i = 0; i < backup.size(); ++i) {
        if (peopleList.find(backup[i].name) == peopleList.end()) {
            sushiList.push_back(backup[i]);
            continue;
        }

        people& person = peopleList[backup[i].name];
        int dis = getDistance(person, backup[i]);

        //debugFile << backup[i].name << ' ' << dis << '\n';

        if (backup[i].tick + dis > tick) {
            sushiList.push_back(backup[i]);
            continue;
        }

        --person.num;

        if (person.num == 0) {
            peopleList.erase(backup[i].name);
        }
    }
    //debugFile << '\n';
    *output << peopleList.size() << ' ' << sushiList.size() <<'\n';
}

int main() {
    int q;

    //runcount = 0;

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