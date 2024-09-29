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

unordered_set<int> dfs(int id, int depth) {
    tree[id].depth = depth;
    auto& cur = tree[id];
    unordered_set<int> cids;

    for (auto child : cur.cids) {
        unordered_set<int> childs = dfs(child, depth + 1);

        for (auto index : childs) {
            cids.insert(index);
        }
    }

    for (auto index : cids) {
        auto& child = tree[index];
        if (child.depth - cur.depth <= child.power) {
            ++cur.availableSet[child.power - (child.depth - cur.depth)];
            //cur.availableSet.insert(index);
        }
    }
    cids.insert(id);

    return cids;
}

void initMessanger() {
    for (int i = 0; i < n; ++i) {
        *input >> tree[i + 1].pid;
        tree[tree[i + 1].pid].cids.insert(i + 1);
    }

    for (int i = 0; i < n; ++i) {
        *input >> tree[i + 1].power;
    }
       dfs(0, 0);
}

//void toggleParent(int id, bool vaule) {
//    queue<int> parents;
//
//    parents.push(tree[id].pid);
//
//    while (!parents.empty()) {
//        int pid = parents.front();
//        parents.pop();
//
//        if (pid == -1) break;
//
//        if (!tree[id].on) {
//            for (auto index : tree[id].availableSet) {
//                tree[pid].availableSet.erase(index);
//            }
//            tree[pid].availableSet.erase(id);
//        }
//        else if(tree[id].on) {
//            for (auto index : tree[id].availableSet) {
//                auto& child = tree[index];
//                if (child.depth - tree[pid].depth <= child.power) {
//                    tree[pid].availableSet.insert(index);
//                }
//            }
//            if (tree[id].depth - tree[pid].depth <= tree[id].power) {
//                tree[pid].availableSet.insert(id);
//            }
//        }
//
//        if (!tree[pid].on) break;
//
//        parents.push(tree[pid].pid);
//    }
//}

void toggleParent(int id) {
    queue<int> parents;
    bool value = !tree[id].on;

    parents.push(tree[id].pid);

    while (!parents.empty()) {
        int pid = parents.front();
        parents.pop();

        if (pid == -1) break;

        if (tree[id].on) {
            for (auto val : tree[id].availableSet) {
                int remain = val.first;
                int count = val.second;

                if (tree[id].depth - tree[pid].depth <= remain) {
                    tree[pid].availableSet[remain - (tree[id].depth - tree[pid].depth)] -= count;
                }
            }
            if (tree[id].depth - tree[pid].depth <= tree[id].power) {
                --tree[pid].availableSet[tree[id].power - (tree[id].depth - tree[pid].depth)];
            }
        }
        else {
            for (auto val : tree[id].availableSet) {
                int remain = val.first;
                int count = val.second;

                if (tree[id].depth - tree[pid].depth <= remain) {
                    tree[pid].availableSet[remain - (tree[id].depth - tree[pid].depth)] += count;
                }
            }
            if (tree[id].depth - tree[pid].depth <= tree[id].power) {
                ++tree[pid].availableSet[tree[id].power - (tree[id].depth - tree[pid].depth)];
            }
        }

        if (!tree[pid].on) break;

        parents.push(tree[pid].pid);
    }

    tree[id].on = !tree[id].on;
}

void toggleSwitch() {
    int id;

    *input >> id;

    toggleParent(id);
}

void changePowerParent(int id, int power) {
    queue<int> parents;
    int befPower = tree[id].power;

    if (!tree[id].on) return;

    parents.push(tree[id].pid);

    while (!parents.empty()) {
        int pid = parents.front();
        parents.pop();

        if (pid == -1) break;

        if (tree[id].depth - tree[pid].depth <= power) {
           ++tree[pid].availableSet[power - (tree[id].depth - tree[pid].depth)];
        }
        
        if (tree[id].depth - tree[pid].depth <= befPower) {
            --tree[pid].availableSet[befPower - (tree[id].depth - tree[pid].depth)];
        }

        if (!tree[pid].on) break;

        parents.push(tree[pid].pid);
    }

    tree[id].power = power;
}

void changePower() {
    int id, power;

    *input >> id >> power;

    changePowerParent(id, power);
}

//void swapParent(int c1, int c2) {
//    queue<int> parents;
//
//    parents.push(tree[c1].pid);
//
//    unordered_set<int> dataSet;
//
//    while (!parents.empty()) {
//        int pid = parents.front();
//        parents.pop();
//
//        if (pid == -1) break;
//
//        dataSet.insert(pid);
//
//        for (auto index : tree[c1].availableSet) {
//            tree[pid].availableSet.erase(index);
//        }
//
//        tree[pid].availableSet.erase(c1);
//
//        if (tree[c2].on) {
//            for (auto index : tree[c2].availableSet) {
//                auto& child = tree[index];
//                if (child.depth - tree[pid].depth <= child.power) {
//                    tree[pid].availableSet.insert(index);
//                }
//            }
//            if (tree[c2].depth - tree[pid].depth <= tree[c2].power) {
//                tree[pid].availableSet.insert(c2);
//            }
//        }
//
//        if (!tree[pid].on) break;
//
//        parents.push(tree[pid].pid);
//    }
//
//    parents.push(tree[c2].pid);
//
//    while (!parents.empty()) {
//        int pid = parents.front();
//        parents.pop();
//
//        if (pid == -1) break;
//
//        if (dataSet.count(pid) <= 0) {
//            for (auto index : tree[c2].availableSet) {
//                tree[pid].availableSet.erase(index);
//            }
//
//            tree[pid].availableSet.erase(c2);
//        }
//
//        if (tree[c1].on) {
//            for (auto index : tree[c1].availableSet) {
//                auto& child = tree[index];
//                if (child.depth - tree[pid].depth <= child.power) {
//                    tree[pid].availableSet.insert(index);
//                }
//            }
//            if (tree[c1].depth - tree[pid].depth <= tree[c1].power) {
//                tree[pid].availableSet.insert(c1);
//            }
//        }       
//
//        if (!tree[pid].on) break;
//
//        parents.push(tree[pid].pid);
//    }
//}

//void swap() {
//    int c1, c2;
//
//    *input >> c1 >> c2;
//
//    swapParent(c1, c2);
//
//    int c1Pid = tree[c1].pid;
//    int c2Pid = tree[c2].pid;
//
//    tree[c1Pid].cids.erase(c1);
//    tree[c1Pid].cids.insert(c2);
//
//
//    tree[c2Pid].cids.erase(c2);
//    tree[c2Pid].cids.insert(c1);
//
//    tree[c2].pid = c1Pid;
//    tree[c1].pid = c2Pid;
//
//}

int runcount;

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