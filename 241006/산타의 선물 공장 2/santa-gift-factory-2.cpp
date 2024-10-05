#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <list>
using namespace std;

#define MAX_N 100000
#define MAX_M 100000

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

int n, m;

struct Present {
    int id;
    Present* prev;  // 이전 노드를 가리킴
    Present* next;  // 다음 노드를 가리킴

    Present(int id) : id(id), prev(nullptr), next(nullptr) {}
};

// 각 벨트는 두 개의 포인터(head, tail)를 가지고 있음
struct Belt {
    Present* head;  // 벨트의 첫 번째 원소
    Present* tail;  // 벨트의 마지막 원소
    int count;      // 벨트에 있는 선물의 개수

    Belt() : head(nullptr), tail(nullptr), count(0) {}
};

Belt belts[MAX_N];  // 각 벨트는 Belt 구조체를 사용
Present* presents[MAX_M];  // 각 선물에 대한 포인터 저장

// 공장 설립
// 선물 공장에 n개의 벨트를 설치하고 m개의 선물을 준비
// 각각 선물의 번호는 오름차순으로 벨트에 쌓임. (우선 순위 큐)
void initFactory() {
    *input >> n >> m;  // n개의 벨트, m개의 선물

    for (int i = 1; i <= m; ++i) {
        int belt;
        *input >> belt;  // i번째 선물이 추가될 벨트 번호 입력

        Present* present = new Present(i);  // 새로운 선물 생성
        presents[i] = present;  // presents 배열에 저장 (해당 선물의 위치를 추적)

        auto& b = belts[belt];  // 벨트 참조

        if (b.head == nullptr) {
            // 벨트가 비어 있는 경우, head와 tail을 모두 이 선물로 설정
            b.head = b.tail = present;
        }
        else {
            // 벨트에 선물이 있는 경우, tail 뒤에 새로운 선물을 추가
            b.tail->next = present;  // 기존 tail의 next를 새 선물로 연결
            present->prev = b.tail;  // 새 선물의 prev를 기존 tail로 설정
            b.tail = present;        // tail을 새 선물로 업데이트
        }

        b.count++;  // 벨트에 있는 선물 개수 증가
    }
}

// 물건 모두 옮기기
// m_src 번째 벨트에 있는 선물을 모두 m_dst번째 벨트의 선물로 옮김.
// 옮겨진 선물은 m_dst 벨트 앞에 위치 (번호로 우선순위가 아님)
// m_src에 선물 없으면 옮기지 않아도 됨.
// 옮긴 뒤 m_dst에 있는 선물 갯수 출력
void movePresent() {
    int src, dst;
    *input >> src >> dst;

    auto& bSrc = belts[src];
    auto& bDst = belts[dst];

    // src 벨트에 선물이 없으면 아무 작업도 하지 않음
    if (bSrc.head == nullptr) {
        *output << bDst.count << '\n';
        return;
    }

    // src 벨트의 모든 선물을 dst 벨트 앞에 붙임
    if (bDst.head == nullptr) {
        // dst가 비어 있는 경우 그냥 연결
        bDst.head = bSrc.head;
        bDst.tail = bSrc.tail;
    }
    else {
        // dst가 비어 있지 않다면, 기존 head의 앞에 src 벨트의 tail을 연결
        bSrc.tail->next = bDst.head;
        bDst.head->prev = bSrc.tail;
        bDst.head = bSrc.head;
    }

    // src 벨트 초기화
    bSrc.head = bSrc.tail = nullptr;
    bDst.count += bSrc.count;
    bSrc.count = 0;

    *output << bDst.count << '\n';
}


// 앞 물건만 교체하기
// m_src 번째 벨트에 있는 선물 중 가장 앞에 있는 선물을 m_dst 번째 벨트의 선물들 중 가장 앞에 있는 것과 교체
// 만약 빈 것이 있다면 그냥 옮기기만 한다. (없는 것을 교체한다.)
void moveFrontPresent() {
    int src, dst;
    *input >> src >> dst;

    auto& bSrc = belts[src];
    auto& bDst = belts[dst];

    // 둘 다 비어 있는 경우 아무 작업도 하지 않음
    if (bSrc.head == nullptr && bDst.head == nullptr) {
        *output << bDst.count << '\n';
        return;
    }

    // src가 비어 있는 경우 -> dst의 첫 번째 원소를 src로 옮김
    if (bSrc.head == nullptr) {
        Present* firstDst = bDst.head;
        bDst.head = firstDst->next;
        if (bDst.head) bDst.head->prev = nullptr;

        firstDst->next = nullptr;
        bSrc.head = bSrc.tail = firstDst;
        bSrc.count++;
        bDst.count--;

        if (bDst.count == 0) bDst.tail = nullptr;
    }
    // dst가 비어 있는 경우 -> src의 첫 번째 원소를 dst로 옮김
    else if (bDst.head == nullptr) {
        Present* firstSrc = bSrc.head;
        bSrc.head = firstSrc->next;
        if (bSrc.head) bSrc.head->prev = nullptr;

        firstSrc->next = nullptr;
        bDst.head = bDst.tail = firstSrc;
        bDst.count++;
        bSrc.count--;

        if (bSrc.count == 0) bSrc.tail = nullptr;
    }
    // 둘 다 원소가 있는 경우 -> 첫 번째 원소를 교환
    else {
        Present* firstSrc = bSrc.head;
        Present* firstDst = bDst.head;

        bSrc.head = firstDst;
        bDst.head = firstSrc;

        if (firstSrc->next) firstSrc->next->prev = firstDst;
        if (firstDst->next) firstDst->next->prev = firstSrc;

        swap(firstSrc->next, firstDst->next);
    }

    *output << bDst.count << '\n';
}

// 물건 나누기
// m_src번재 벨트에 있는 선물을 n이라 하면 가장 앞에서 floor(n/2) 번째 선물을 m_dst 앞으로 옮김
// 만약 m_src에 1개 이하면 옮기지 않는다. 왜? floor(1/2)는 0이니까.
// 옮긴 뒤 m_dst 벨트의 갯수 출력
void moveHalfPresent() {
    int src, dst;
    *input >> src >> dst;

    auto& bSrc = belts[src];  // src 벨트에 대한 참조
    auto& bDst = belts[dst];  // dst 벨트에 대한 참조

    // src 벨트에 있는 선물 개수가 1 이하라면 옮기지 않음
    if (bSrc.count <= 1) {
        *output << bDst.count << '\n';
        return;
    }

    // 옮길 개수: src 벨트에서 절반의 개수 (floor(n/2))
    int sizeToMove = bSrc.count / 2;

    // src 벨트에서 floor(n/2)번째 선물의 위치를 찾음
    Present* current = bSrc.head;
    for (int i = 0; i < sizeToMove - 1; ++i) {
        current = current->next;  // 앞에서부터 sizeToMove 번째 선물까지 이동
    }

    Present* newHeadSrc = current->next;  // src에서 남은 부분의 새 head
    Present* oldHeadDst = bDst.head;      // dst의 기존 head

    // src에서 분리한 선물들을 dst 앞에 추가
    if (bDst.head == nullptr) {
        // dst가 비어 있는 경우
        bDst.head = bSrc.head;  // src의 절반을 dst로 이동
        bDst.tail = current;    // 이동한 부분의 tail은 current
    }
    else {
        // dst에 원소가 있는 경우, src의 절반을 dst의 앞에 연결
        bSrc.head->prev = nullptr;  // src의 절반의 head의 prev는 nullptr
        current->next = oldHeadDst;  // src의 절반 tail의 next를 dst의 head로 연결
        oldHeadDst->prev = current;  // dst의 기존 head의 prev를 src의 tail로 연결
        bDst.head = bSrc.head;  // dst의 새로운 head는 src의 절반 head
    }

    // src 벨트의 head 및 tail 업데이트
    bSrc.head = newHeadSrc;
    if (newHeadSrc != nullptr) {
        newHeadSrc->prev = nullptr;  // 남은 부분의 head는 이전 연결을 끊음
    }
    else {
        // src에 남은 선물이 없는 경우
        bSrc.tail = nullptr;
    }

    // 선물 개수 업데이트
    bDst.count += sizeToMove;
    bSrc.count -= sizeToMove;

    *output << bDst.count << '\n';
}


// 선물 정보 얻기
// 선물 번호 p_num에서 해당 선물의 앞 번호 a, 뒤 번호 b이면 a + (2 * b)를 출력합니다. 앞 선물이나 뒤 선물이 없으면 그 숫자는 -1
void getPresentInfo() {
    int p_num;
    *input >> p_num;

    // presents 배열에서 해당 선물 번호에 대한 포인터를 가져옴
    Present* present = presents[p_num];

    // 앞, 뒤 선물 번호를 저장할 변수
    int a = -1, b = -1;

    // 앞 선물이 있으면 그 번호를 a에 저장
    if (present->prev != nullptr) {
        a = present->prev->id;
    }

    // 뒤 선물이 있으면 그 번호를 b에 저장
    if (present->next != nullptr) {
        b = present->next->id;
    }

    // 결과 출력: a + (2 * b), 앞뒤 선물이 없으면 -1 사용
    *output << a + (2 * b) << endl;
}

// 벨트 정보 얻기
// 벨트 번호 b_num에서 해당 벨트 맨 앞 번호를 a, 맨뒤 번호를 n, 선물 개수를 c라고 하면 a + 2 * b + 3 * c의 값을 출력
// 선물이 없는 경우 a b는 -1이 됨. c는 0 일듯?
void getBeltInfo() {
    int b_num;
    *input >> b_num;

    // 해당 벨트 참조
    auto& belt = belts[b_num];

    // 첫 번째 선물과 마지막 선물의 번호를 저장할 변수
    int a = -1, b = -1, c = belt.count;

    // 벨트가 비어 있지 않으면 head와 tail의 번호를 가져옴
    if (belt.head != nullptr) {
        a = belt.head->id;
    }
    if (belt.tail != nullptr) {
        b = belt.tail->id;
    }

    // 결과 출력: a + (2 * b) + (3 * c)
    *output << a + (2 * b) + (3 * c) << endl;
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
            movePresent();
            break;
        case 300:
            //cout << "300\n";
            moveFrontPresent();
            break;
        case 400:
            moveHalfPresent();
            break;
        case 500:
            getPresentInfo();
            break;
        case 600:
            getBeltInfo();
            break;
        }
    }

    return 0;
}