#include <iostream>
#include <vector>

using namespace std;

#define MAX_N 100001
#define MAX_D 22

int n, q;
int a[MAX_N], p[MAX_N], val[MAX_N];
bool noti[MAX_N];
int nx[MAX_N][MAX_D];

// 초기 설정 값을 받아옵니다.
void init() {
    // 부모 채팅과 채팅의 권한 정보를 입력받습니다.
    for(int i = 1; i <= n; i++) {
        cin >> p[i];
    }
    for(int i = 1; i <= n; i++) {
        cin >> a[i];
        // 채팅의 권한이 20을 초과하는 경우 20으로 제한합니다.
        if(a[i] > 20) a[i] = 20;
    }
    
    // nx 배열과 val 값을 초기화합니다.
    for(int i = 1; i <= n; i++) {
        int cur = i;
        int x = a[i];
        nx[cur][x]++;
        // 상위 채팅으로 이동하며 nx와 val 값을 갱신합니다.
        while(p[cur] && x) {
            cur = p[cur];
            x--;
            if(x) nx[cur][x]++;
            val[cur]++;
        }
    }
}

// 채팅의 알림 상태를 토글합니다.
void toggle_noti(int chat) {
    if(noti[chat]) {
        int cur = p[chat];
        int num = 1;
        // 상위 채팅으로 이동하며 noti 값에 따라 nx와 val 값을 갱신합니다.
        while(cur) {
            for(int i = num; i <= 21; i++) {
                val[cur] += nx[chat][i];
                if(i > num) nx[cur][i - num] += nx[chat][i];
            }
            if(noti[cur]) break;
            cur = p[cur];
            num++;
        }
        noti[chat] = false;
    } else {
        int cur = p[chat];
        int num = 1;
        // 상위 채팅으로 이동하며 noti 값에 따라 nx와 val 값을 갱신합니다.
        while(cur) {
            for(int i = num; i <= 21; i++) {
                val[cur] -= nx[chat][i];
                if(i > num) nx[cur][i - num] -= nx[chat][i];
            }
            if(noti[cur]) break;
            cur = p[cur];
            num++;
        }
        noti[chat] = true;
    }
}

// 채팅의 권한의 크기를 변경합니다.
void change_power(int chat, int power) {
    int bef_power = a[chat];
    power = min(power, 20);  // 권한의 크기를 20으로 제한합니다.
    a[chat] = power;

    nx[chat][bef_power]--;
    if(!noti[chat]) {
        int cur = p[chat];
        int num = 1;
        // 상위 채팅으로 이동하며 nx와 val 값을 갱신합니다.
        while(cur) {
            if(bef_power >= num) val[cur]--;
            if(bef_power > num) nx[cur][bef_power - num]--;
            if(noti[cur]) break;
            cur = p[cur];
            num++;
        }
    }

    nx[chat][power]++;
    if(!noti[chat]) {
        int cur = p[chat];
        int num = 1;
        // 상위 채팅으로 이동하며 nx와 val 값을 갱신합니다.
        while(cur) {
            if(power >= num) val[cur]++;
            if(power > num) nx[cur][power - num]++;
            if(noti[cur]) break;
            cur = p[cur];
            num++;
        }
    }
}

// 두 채팅의 부모를 교체합니다.
void change_parent(int chat1, int chat2) {
    bool bef_noti1 = noti[chat1];
    bool bef_noti2 = noti[chat2];

    if(!noti[chat1]) toggle_noti(chat1);
    if(!noti[chat2]) toggle_noti(chat2);

    swap(p[chat1], p[chat2]);

    if(!bef_noti1) toggle_noti(chat1);
    if(!bef_noti2) toggle_noti(chat2);
}

// 해당 채팅의 val 값을 출력합니다.
void print_count(int chat) {
    cout << val[chat] << "\n";
}

int main() {
    cin >> n >> q;
    while(q--) {
        int query;
        cin >> query;
        if(query == 100) {
            init();
        }
        else if(query == 200) {
            int chat;
            cin >> chat;
            toggle_noti(chat);
        }
        else if(query == 300) {
            int chat, power;
            cin >> chat >> power;
            change_power(chat, power);
        }
        else if(query == 400) {
            int chat1, chat2;
            cin >> chat1 >> chat2;
            change_parent(chat1, chat2);
        }
        else if(query == 500) {
            int chat;
            cin >> chat;
            print_count(chat);
        }
    }
    return 0;
}