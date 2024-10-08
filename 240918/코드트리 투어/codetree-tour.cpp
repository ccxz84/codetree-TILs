#include <iostream>
#include <sstream>
#include <queue>
#include <vector>
#include <climits>
#include <functional>
//#include <unordered_set>
#include <unordered_map>
using namespace std;

// 코드 트리 랜드 건설
// 100 n m v1 u1 w1 v2 u2 w2 ...

// 여행 상품 생성
// 200 id rev(매출) dest(목적)

// 여행 취소
// 300 id

// 최적 상품 판매
// 400 (매출 - 비용 이 가장 큰 것)

// 출발지 변경
// 500 s (모든 여행 상품의 출발지를 s로 변경) (cost 값 다시 계산 필요)

struct travel {
    int id, reve, dest;
};

int n;

int startPoint;

int dist[2000][2000];
vector<vector<pair<int, int>>> graph(2000);

bool heapCompare(struct travel a, struct travel b) {
    if (a.reve - dist[startPoint][a.dest] != b.reve - dist[startPoint][b.dest]) {
        return a.reve - dist[startPoint][a.dest] < b.reve - dist[startPoint][b.dest];
    }
    return a.id > b.id;

}

//bool heapCompare(struct travel a, struct travel b) {
//    // 매출 - 비용 값이 클수록 우선
//    if (a.reve - dist[startPoint][a.dest] != b.reve - dist[startPoint][b.dest]) {
//        return a.reve - dist[startPoint][a.dest] > b.reve - dist[startPoint][b.dest];
//    }
//    // 매출 - 비용 값이 같으면, ID가 작은 것이 우선
//    return a.id < b.id;
//}



unordered_map<int, pair<int, int>> travels;
vector<priority_queue<travel, vector<travel>, function<bool(travel, travel)>>> queues;

void initializeQueues() {
    // 각 우선순위 큐에 해당하는 비교 함수 설정
    for (int i = 0; i < 2000; ++i) {
        auto heapCompare = [i](struct travel a, struct travel b) {
            // 우선순위 큐 인덱스 i에 해당하는 비교 기준 설정
            if (a.reve - dist[i][a.dest] != b.reve - dist[i][b.dest]) {
                return a.reve - dist[i][a.dest] < b.reve - dist[i][b.dest];
            }
            return a.id > b.id;
            };

        // i번째 우선순위 큐에 맞는 비교 함수 적용
        queues.emplace_back(priority_queue<travel, vector<travel>, function<bool(travel, travel)>>(heapCompare));
    }
}

void dijkstra(int start) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    fill(dist[start], dist[start] + n, INT_MAX);
    dist[start][start] = 0;
    pq.push({ 0, start });

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;

        pq.pop();

        if (dist[start][u] < d) continue;

        for (auto edge : graph[u]) {
            int v = edge.first;
            int weight = edge.second;

            if (dist[start][u] + weight < dist[start][v]) {
                dist[start][v] = dist[start][u] + weight;
                pq.push({ dist[start][v], v });
            }
        }
    }
}

void insertNode() {
    int m;
    cin >> n >> m;

    for (int i = 0; i < m; ++i) {
        int v, u, w;
        cin >> v >> u >> w;

        graph[v].push_back({ u, w });
        graph[u].push_back({ v, w });
    }

    for (int i = 0; i < n; ++i) {
        dijkstra(i);
    }
}

//void insertTravel() {
//    struct travel newTravel;
//
//    cin >> newTravel.id >> newTravel.reve >> newTravel.dest;
//
//    travels[newTravel.id] = { newTravel.dest, newTravel.reve };
//
//    if (newTravel.reve - dist[startPoint][newTravel.dest] >= 0) {
//        qu.push(newTravel);
//    }
//}

void insertTravel() {
    travel newTravel;
    cin >> newTravel.id >> newTravel.reve >> newTravel.dest;

    travels[newTravel.id] = { newTravel.dest, newTravel.reve };

    // 각 출발 지점별 큐에 travel 삽입
    for (int i = 0; i < n; ++i) {
        if (newTravel.reve - dist[i][newTravel.dest] >= 0) {
            queues[i].push(newTravel);  // i번째 출발 지점 큐에 삽입
        }
    }
}

void removeTravel() {
    int id;
    cin >> id;
    travels.erase(id);
}

//int popTravel() {
//    while (!qu.empty() && travels.find(qu.top().id) == travels.end()) {
//        qu.pop();
//    }
//
//    if (!qu.empty()) {
//        int id = qu.top().id;
//        qu.pop();
//        travels.erase(id);
//        return id;
//    }
//
//    return -1;
//}

int popTravel() {
    // 현재 출발 지점에 맞는 큐에서 pop
    auto& currentQueue = queues[startPoint];

    while (!currentQueue.empty() && travels.find(currentQueue.top().id) == travels.end()) {
        currentQueue.pop();  // 유효하지 않은 항목은 제거
    }

    if (!currentQueue.empty()) {
        int id = currentQueue.top().id;
        currentQueue.pop();
        travels.erase(id);
        return id;
    }

    return -1;
}

void changeStart() {
    cin >> startPoint;

    //while (!qu.empty()) {
    //    qu.pop();
    //}

    //for (auto& t : travels) {
    //    struct travel tra = { t.first, t.second.second, t.second.first };

    //    if (tra.reve - dist[startPoint][tra.dest] >= 0) {
    //        qu.push(tra);
    //    }
    //}
}

int main() {
    int q;

    cin >> q;
    startPoint = 0;

    initializeQueues();

    for (int i = 0; i < q; ++i) {
        int code;
        cin >> code;

        switch (code) {
        case 100:
            insertNode();
            break;
        case 200:
            insertTravel();
            break;
        case 300:
            removeTravel();
            break;
        case 400:
            cout << popTravel()<<'\n';
            break;
        case 500:
            changeStart();
            break;
        }
    }

    return 0;
}