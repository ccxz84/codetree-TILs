#include <iostream>
#include <fstream>
#include <queue>
#include <climits>
#include <stack>
using namespace std;

ifstream inputFile("./input.txt");
istream* input = &cin;
ofstream outputFile("./output.txt");
ofstream debugFile("./debug.txt");
ostream* output = &cout;

int k, n;

vector<vector<int>> matrix(5, vector<int>(5, 0));
queue<int> numberList;
int score;

inline int getXIndex(int x, int y, int size, int degree) {
    switch (degree) {
    case 90:
        return y;
    case 180:
        return size - 1 - x;
    case 270:
        return size - 1 - y;
    }
}
inline int getYIndex(int x, int y, int size, int degree) {
    switch (degree) {
    case 90:
        return size - 1 - x;
    case 180:
        return size - 1 - y;
    case 270:
        return x;
    }
}


void rotateMatrix(int x, int y, int size, int degree, vector<vector<int>>& mat) {
    int temp[5][5];

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            temp[getXIndex(i, j, size, degree)][getYIndex(i, j, size, degree)] = mat[i + x][j+ y];
        }
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            mat[x + i][y + j] = temp[i][j];
        }
    }
}

int dirX[4] = { -1, 1, 0, 0 };
int dirY[4] = { 0, 0, 1, -1 };

inline bool isValid(int x, int y) {
    return x >= 0 && x < 5 && y >= 0 && y < 5;
}

int BFS(vector<vector<int>>& mat, queue<int>& numQueue) {
    queue<pair<int, int>> qu;

    int count = 0;

    vector<vector<bool>> visited(5, vector<bool>(5, 0));
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (!visited[i][j]) {
                qu.push({ i, j });
                visited[i][j] = true;

                int num = mat[i][j];

                vector<pair<int, int>> posList = { {i,j} };

                while (!qu.empty()) {
                    auto pos = qu.front();
                    qu.pop();

                    for (int a = 0; a < 4; ++a) {
                        int nx = pos.first + dirX[a], ny = pos.second + dirY[a];

                        if (isValid(nx, ny) && !visited[nx][ny] && mat[nx][ny] == num) {
                            visited[nx][ny] = true;
                            qu.push({ nx, ny });
                            posList.push_back({ nx, ny });
                        }
                    }
                }

                if (posList.size() >= 3) {
                    for (int a = 0; a < posList.size(); ++a) {
                        mat[posList[a].first][posList[a].second] = -1;
                    }
                    count += posList.size();
                }
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        for (int j = 4; j >= 0; --j) {
            if (mat[j][i] == -1) {
                mat[j][i] = numQueue.front();
                numQueue.pop();
            }
        }
    }

    return count;
}

void printRotate(vector<vector<int>> mat, queue<int> numQueue) {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            debugFile << mat[i][j] << ' ';
        }
        debugFile << '\n';
    }
    debugFile << '\n';

    while (!numQueue.empty()) {
        debugFile << numQueue.front() << ' ';
        numQueue.pop();
    }

    debugFile << "\n";
}

//inline bool compare(int score1, int degree1, int x1, int y1, int score2, int degree2, int x2, int y2) {
//    if (score1 != score2) return score1 > score2;
//    if (degree1 != degree2) return degree1 < degree2;
//    if (x1 != x2) return x1 
//}

int find() {
    score = 0;
    queue<int> maxQueue;
    vector<vector<int>> maxMat;
    int minDegree;

    int debugX;
    int debugY;
    for (int a = 1; a <= 3; ++a) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                auto mat = matrix;
                auto numQueue = numberList;
                rotateMatrix(j, i, 3, a * 90, mat);
                int count = BFS(mat, numQueue);

                debugFile << "middle: " << j + 1 << ' ' << i + 1 << " rot: "<<a*  90 << '\n';
                printRotate(mat, numQueue);
                debugFile << "count : " << count << "\n\n";

                if (count > score) {
                    debugX = j;
                    debugY = i;
                    score = count;
                    maxQueue = numQueue;
                    maxMat = mat;
                }
            }
        }
    }

    if (score > 0) {
        while (true) {
            int co = BFS(maxMat, maxQueue);

            if (co > 0) {
                score += co;
            }
            else {
                break;
            }
        }
        
        numberList = maxQueue;
        matrix = maxMat;

        debugFile << debugX << ' ' << debugY << '\n';
        return score;
    }

    return -1;
}

void print() {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            debugFile << matrix[i][j] << ' ';
        }
        debugFile << '\n';
    }
    debugFile << '\n';
}

void solution() {
    for (int i = 0; i < k; ++i) {
        int s = find();
        if (s == -1) {
            print();
            break;
        }
        print();
        *output << s << ' ';
    }
    *output << '\n';
}

void init() {
    int m;
    *input >> k >> m;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            *input >> matrix[i][j];
        }
    }

    for (int i = 0; i < m; ++i) {
        int x;
        *input >> x;
        numberList.push(x);
    }
}

int main() {
    if (inputFile.is_open()) {
        input = &inputFile;  // 파일을 열었다면 파일로 입력을 받음
    }

    if (outputFile.is_open()) {
        output = &outputFile;  // 파일을 열었다면 출력도 파일로
    }

    init();
    solution();
}