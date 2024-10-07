#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <list>
using namespace std;

#define MAX_N 100000
#define MAX_M 10

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

int n, m;

struct Present {
    int id;
    int weight;
    int beltId;

    Present* next = nullptr;
    Present* prev = nullptr;
};

struct Belt {
    bool status = false;
    Present* head;
    Present* tail;
};

Belt belts[MAX_M+1];
//int currentBelt[MAX_M + 1];
unordered_map<int, Present*> presents;

void removePresent(Present* pre, bool clear) {
    int beltNum = pre->beltId;
    Belt& curBelt = belts[beltNum];

    // 벨트에 원소가 하나라면
    if (curBelt.head == curBelt.tail) {
        curBelt.head = curBelt.tail = nullptr;
    }
    // 삭제 되는게 head인 경우
    else if (curBelt.head == pre) {
        curBelt.head = pre->next;
        pre->next->prev = nullptr;
    }
    // 삭제 되는게 prev인 경우
    else if (curBelt.tail == pre) {
        curBelt.tail = pre->prev;
        pre->prev->next = nullptr;
    }
    // 중간에서 삭제되는 경우
    else {
        pre->prev->next = pre->next;
        pre->next->prev = pre->prev;
    }

    if (clear) {
        free(pre);
    }
}

// 맨 뒤에 추가하는 경우 밖에 없음
void pushbackPresent(Present* targetPresent, Present* pre) {
    targetPresent->next = pre;
    pre->prev = targetPresent;

    Belt& belt = belts[targetPresent->beltId];

    belt.tail = pre;

    pre->next = nullptr;
}

// 리스트 단위로 추가
void pushbackPresents(Present* targetPresent, Present* startPre, Present* endPre) {
    targetPresent->next = startPre;
    startPre->prev = targetPresent;

    Belt& belt = belts[targetPresent->beltId];

    belt.tail = endPre;
    endPre->next == nullptr;
}

void pushFrontPresents(Present* targetPresent, Present* pre) {
    targetPresent->prev = pre;
    pre->next = targetPresent;
}


// 공장 설립
// 선물 공장에 m개의 벨트를 설치하고 n개의 선물을 준비
// 각 벨트에 n/m 개씩 선물이 놓임
// 각 물건에는 고유 id와 무게가 있음.

// 처음 들어오는 물건을 n/m개씩 잘라 1번 벨트부터 놓으면 됨.
void initFactory() {
    *input >> n >> m;

    for (int i = 1; i <= m; ++i) {
        belts[i].status = true;
        for(int j = 0; j < n/m; ++j) {
            Present* newPresent = new Present();

            *input >> newPresent->id;
            newPresent->beltId = i;

            if (!belts[i].tail) {
                belts[i].head = newPresent;
                belts[i].tail = newPresent;
            }
            else {
                belts[i].tail->next = newPresent;
                newPresent->prev = belts[i].tail;
                belts[i].tail = newPresent;
            }
            presents[newPresent->id] = newPresent;
        }
    }

    for (int i = 1; i <= m; ++i) {
        Present* cur = belts[i].head;
        for (int j = 0; j < n / m; ++j) {
            *input >> cur->weight;
            cur = cur->next;
        }
    }
}

// 물건 하차
// 상자의 최대 무게 w_max가 주어짐.
// 1번부터 m번까지 벨트를 보며 맨 앞에 있는 물건이 w_max 이하이면 하차 아니면 맨 뒤로 보냄
// 하차된 무게 총합을 출력
void downPresent() {
    int weight;

    *input >> weight;

    long long allWeight = 0;

    for (int i = 1; i <= m; ++i) {
        if (belts[i].head && belts[i].status) {
            Present* frontP = belts[i].head;

            if (frontP->weight <= weight) {
                allWeight += frontP->weight;
                presents[frontP->id] = nullptr;
                removePresent(frontP, true);
            }
            else {
                removePresent(frontP, false);

                pushbackPresent(belts[i].tail, frontP);
            }
        }
    }
    *output << allWeight << '\n';
}


// 물건 제거
// 물건 r_id가 주어짐. 해당 고유 번호에 해당하는 상자가 놓여있는 벨트가 있다면, 해당 벨트에서 상자 제거하고 뒤에 있던 상자들은 앞으로 한 칸씩 내려옴
// r_id에 해당하는 상자가 있다면 r_id를 아니면 -1을 출력
void removePresent() {
    int id;

    *input >> id;

    if (presents[id]) {
        removePresent(presents[id], true);
        presents[id] = nullptr;
        *output << id << '\n';
    }
    else {
        *output << -1 << '\n';
    }
}

// 물건 확인
// 물건 고유번호 f_id가 주어짐.
// 물건이 놓여있는 벨트가 있다면 벨트 번호 출력, 아니면 -1
// 상자가 있으면 해당 상자 위에 있는 모든 상자를 앞으로 가져옴. 순서유지
void checkPresent() {
    int id;

    *input >> id;

    if (presents[id]) {
        Present* curP = presents[id];

        pushFrontPresents(belts[presents[id]->beltId].head, belts[presents[id]->beltId].tail);
        belts[presents[id]->beltId].head = curP;
        curP->prev->next = nullptr;
        belts[presents[id]->beltId].tail = curP->prev;
        *output << presents[id]->beltId << '\n';
    }
    else {
        *output << -1 << '\n';
    }
}


// 벨트 고장
// id를 받으면 해당 벨트는 고장나고 오른쪽 벨트부터 순서대로 사용 가능한 벨트를 골라 발견하면 그 위로 모든 상자를 옮김
// m을 넘어가면 1부터 다시 시작
void brokenBelt() {
    int beltId;

    *input >> beltId;

    if (!belts[beltId].status) {
        *output << "-1\n";
        return;
    }


    for (int i = 1; i < m; ++i) {
        int index = (beltId + i) % m == 0 ? m : (beltId + i) % m;
        if (belts[index].status) {
            Belt& dst = belts[index];
            Belt& src = belts[beltId];

            if (src.head) {
                if (dst.tail) {
                    pushbackPresents(dst.tail, src.head, src.tail);
                }
                else {
                    dst.tail = src.tail;
                    dst.head = src.head;
                }
            }

            Present* cur = src.head;

            while (cur != nullptr) {
                cur->beltId = index;
                cur = cur->next;
            }

            src.status = false;
            src.head = src.tail = nullptr;
            *output << beltId << '\n';
            break;
        }
    }
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
            initFactory();
            break;
        case 200:
            //cout << "200\n";
            downPresent();
            break;
        case 300:
            //cout << "300\n";
            removePresent();
            break;
        case 400:
            checkPresent();
            break;
        case 500:
            brokenBelt();
            break;
        }
    }

    return 0;
}