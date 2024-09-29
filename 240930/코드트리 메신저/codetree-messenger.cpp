#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

// 메신저 준비
// N은 노드 갯수 1 ~ N
// 각 노드는 부모를 하나씩 가지고 있음
// 권한 메시지 보내면 c만큼 위로 올라가서 알람 전해짐
// 0은 메인이므로 권한, 알람 설정 영향 안받음

// 알림망 설정
// 알림망 설정을 토글 하는 기능
// OFF 면 해당 노드를 지나가는 알람을 전송하지 않음

// 권한 세기 변경
// 특정 노드의 권한 값을 변경

// c1번 채팅방과 c2번 채팅방으로 부모를 서로 변경함
// c1, c2는 같은 depth에 있음.

// 알림 받을 수 있는 채팅방 조회
// 해당 노드가 몇개의 자식으로 부터 알림을 받을 수 있는지 체크

int n;

struct node {
    int pid, power;
    bool on = true;
    unordered_set<int> cids;
    unordered_map<int, int> availableSet;
    int depth = 0;
};

vector<node> tree;

//unordered_set<int> dfs(int id, int depth) {
//    tree[id].depth = depth;
//    auto& cur = tree[id];
//    unordered_set<int> cids;
//
//    for (auto child : cur.cids) {
//        unordered_set<int> childs = dfs(child, depth + 1);
//
//        for (auto index : childs) {
//            cids.insert(index);
//        }
//    }
//
//    for (auto index : cids) {
//        auto& child = tree[index];
//        if (child.depth - cur.depth <= child.power) {
//            ++cur.availableSet[child.power - (child.depth - cur.depth)];
//            //cur.availableSet.insert(index);
//        }
//    }
//    cids.insert(id);
//
//    return cids;
//}

void initMessanger() {
    for (int i = 0; i < n; ++i) {
        *input >> tree[i + 1].pid;
        tree[tree[i + 1].pid].cids.insert(i + 1);
    }

    for (int i = 0; i < n; ++i) {
        *input >> tree[i + 1].power;
        if (tree[i + 1].power > 20) tree[i + 1].power = 20;
    }
       /*dfs(0, 0);*/

    for (int i = 1; i <= n; i++) {
        int cur = i;
        int x = tree[i].power;

        while (x) {
            cur = tree[cur].pid;
            if (cur == -1) break;
            x--;
            if (x >= 0) tree[cur].availableSet[x]++;
        }
    }
}

void adjustAvailableSet(int id, int delta) {
    int num = 1;
    int cur = tree[id].pid;

    while (cur != -1) {
        for (auto val : tree[id].availableSet) {
            int remain = val.first;
            int count = val.second;
            int adjusted_remain = remain - num;
            if (adjusted_remain >= 0) {
                tree[cur].availableSet[adjusted_remain] += delta * count;
            }
        }
        int adjusted_power = tree[id].power - num;
        if (adjusted_power >= 0) {
            tree[cur].availableSet[adjusted_power] += delta * 1;
        }
        if (!tree[cur].on) break;
        cur = tree[cur].pid;
        num++;
    }
}

void toggleParent(int id) {
    bool value = !tree[id].on;
    int delta = value ? 1 : -1;

    adjustAvailableSet(id, delta);

    tree[id].on = value;
}

void toggleSwitch() {
    int id;

    *input >> id;

    toggleParent(id);
}

void changePowerParent(int id, int power) {
    int befPower = tree[id].power;

    if (!tree[id].on) {
        tree[id].power = power;
        return;
    }

    // 이전 권한 값의 영향 제거
    //tree[id].availableSet[befPower]--;
    adjustAvailableSet(id, -1);

    // 권한 값 업데이트
    tree[id].power = power;
    //tree[id].availableSet[power]++;

    // 새로운 권한 값의 영향 추가
    adjustAvailableSet(id, 1);
}


//void changePowerParent(int id, int power) {
//    queue<int> parents;
//    int befPower = tree[id].power;
//
//    if (!tree[id].on) return;
//
//    int num = 1;
//
//    parents.push(tree[id].pid);
//
//    while (!parents.empty()) {
//        int pid = parents.front();
//        parents.pop();
//
//        if (pid == -1) break;
//
//        if (num <= befPower) {
//            --tree[pid].availableSet[befPower - num];
//        }
//
//        if (num <= power) {
//           ++tree[pid].availableSet[power - num];
//        }
//
//        if (!tree[pid].on) break;
//
//        parents.push(tree[pid].pid);
//        ++num;
//    }
//
//    tree[id].power = power;
//}

void changePower() {
    int id, power;

    *input >> id >> power;

    changePowerParent(id, power);
}

void swap() {
    int c1, c2;
    
    *input >> c1 >> c2;

    bool befC1 = tree[c1].on;
    bool befC2 = tree[c2].on;

    if (tree[c1].on) toggleParent(c1);
    if (tree[c2].on) toggleParent(c2);

    swap(tree[c1].pid, tree[c2].pid);

    if (befC1) toggleParent(c1);
    if (befC2) toggleParent(c2);
}

void printChatroom() {
    int id, count = 0;

    *input >> id;
    for (auto val : tree[id].availableSet) {
        count += val.second;
    }

    *output << count << '\n';
}

int getCount(int id) {
    int count = 0;
    for (auto val : tree[id].availableSet) {
        count += val.second;
    }

    return count;
}

int main() {
    int q;

    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    *input >> n >> q;
    tree = vector<node>(n+1);
    tree[0] = { -1, 0, true };

    for (int i = 0; i < q; ++i) {
        int code;
        *input >> code;
        switch (code) {
        case 100:
            //cout << "100\n";
            initMessanger();
            break;
        case 200:
            //cout << "200\n";
            toggleSwitch();
            break;
        case 300:
            //cout << "300\n";
            changePower();
            break;
        case 400:
            swap();
            break;
        case 500:
            printChatroom();
            break;
        }
    }
    
    return 0;
}