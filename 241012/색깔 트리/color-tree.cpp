#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
using namespace std;

// 노드 추가
// id, pid, maxdepth, color
// max depth 넘어가면 삽입 안됨

// 색깔 변경
// 서브 트리의 색이 모두 바뀜

// 색 조회
// 특정 노드의 색 조회

// 점수 조회
// 모든 노드의 가치 제곱의 합
// 가치는 자기 자신과 서브 트리의 색 종류

ifstream inputFile("./input.txt");
istream* input = &cin;

int tick;

vector<int> roots;

struct node {
    int id, pid, maxDepth, color, useDepth, lastColor = 0, lastTick = -1;
    vector<int> cids;
};

struct node tree[100001];
int score;

// 삽입
// pid의 노드를 조회해서 useDepth의 값이 0 보다 큰 경우에 삽입 가능
// pid 노드의 useDepth - 1이 삽입되는 노드의 maxDepth보다 크다면 삽입 노드의 useDepth는 maxdepth -1
// 반대의 경우 삽입 노드의 useDepth는 부모의 useDepth - 1 
// lastTick을 전역 변수 tick으로 업데이트
// pid가 -1 이면 root 배열에 값 추가
void insertNode() {
    struct node newNode;
    *input >> newNode.id >> newNode.pid >> newNode.color >> newNode.maxDepth;
    newNode.lastTick = tick;

    if (newNode.pid == -1) {
        roots.push_back(newNode.id);
        newNode.useDepth = newNode.maxDepth - 1;
        tree[newNode.id] = newNode;
    }
    else {
        struct node &pNode = tree[newNode.pid];

        if (pNode.useDepth <= 0) {
            return;
        }

        if (pNode.useDepth - 1 < newNode.maxDepth) {
            newNode.useDepth = pNode.useDepth - 1;
        }
        else {
            newNode.useDepth = newNode.maxDepth - 1;
        }

        pNode.cids.push_back(newNode.id);
        tree[newNode.id] = newNode;
    }
}

// 색 변경
// 해당 노드에 lastchangeColor, lasttick 변경
// lastTick을 전역 변수 tick으로 업데이트
void changeColor() {
    int id, color;
    *input >> id >> color;

    struct node& change = tree[id];

    change.lastColor = color;
    change.lastTick = tick;
}

void checkParent(int id, int& lastColor, int& lastTick) {
    struct node& cur = tree[id];

    if (cur.lastColor != 0 && cur.lastTick >= lastTick) {
        lastColor = cur.lastColor;
        lastTick = cur.lastTick;
    }

    if (cur.pid == -1 || lastTick == tick - 1) {
        return;
    }

    checkParent(cur.pid, lastColor, lastTick);
}

// 색 조회
// 부모를 순회 하며 tick이 노드의 tick보다 lastChangeColor가 0이 아니며 큰 경우의 색으로 변경
// 아니면 자기 색 리턴
int viewColor() {
    int id;
    *input >> id;

    struct node& cur = tree[id];

    int lastColor = cur.color, lastTick = cur.lastTick;

    checkParent(id, lastColor, lastTick);
    
    return lastColor;
}

void checkChild(int id, int lastColor, int lastTick, unordered_set<int>& curSet) {
    struct node& cur = tree[id];

    int color = cur.color;

    // 자신이 부모보다 늦게 색상 변경이 됐나 확인
    if (cur.lastColor != 0 && cur.lastTick > lastTick) {
        lastColor = cur.lastColor;
        lastTick = cur.lastTick;
    }

    // 최신 색상으로 값 변경
    if (cur.lastTick <= lastTick ) {
        color = lastColor;
    }

    // 자식 순회
    for (auto child : cur.cids) {
        unordered_set<int> childSet;
        checkChild(child, lastColor, lastTick, childSet);

        curSet.insert(childSet.begin(), childSet.end());
    }

    curSet.insert(color);

    score += curSet.size() * curSet.size();
}

// 점수 조회
// 루트 부터 아래로 내려가면서 colorSet 업데이트
// 부모의 lastTick이 가장 크면서 lastChangeColor가 0이 아닌 경우 색을 set에 추가
// set에 추가한 다음 가치 계산
void viewScore() {
    int answer = 0; 
    for (int i = 0; i < roots.size(); ++i) {
        score = 0;
        struct node& root = tree[roots[i]];
        int lastColor = 0, lastTick = -1;

        unordered_set<int> curSet;

        checkChild(root.id, lastColor, lastTick, curSet);

        answer += score;
    }

    cout << answer << '\n';
}

int main() {
    tick = 0;
    int q;

    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    *input >> q;

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        ++tick;

        switch (code) {
        case 100:
            insertNode();
            break;
        case 200:
            changeColor();
            break;
        case 300:
            cout<<viewColor()<<'\n';
            break;
        case 400:
            viewScore();
            break;
        }
    }

    // 여기에 코드를 작성해주세요.
    return 0;
}