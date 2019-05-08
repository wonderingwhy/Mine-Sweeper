#pragma comment(linker, "/STACK:102400000,102400000")
#include <windows.h>   
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <queue>
#include <cstdlib>
#include <set>
#include <stack>
#include <vector>
#include <omp.h>
using namespace std;
const int DX[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
const int DY[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
const int ROWS = 16;
const int COLUMNS = 16;
const int MINES = 40;
int cell[ROWS][COLUMNS];
clock_t readBegin, readEnd;
clock_t calcBegin, calcEnd;
double readTime, calcTime;
struct MineSweeper {
	static const int GRAPHSIZE = (ROWS * COLUMNS) << 1;
	bool graph[GRAPHSIZE][GRAPHSIZE];
	set<pair<int, int> > mines;
	set<pair<int, int> > notMines;
	void AddEdge(int u, int v) {
		//printf("(%d, %d)\n", u, v);
		graph[u][v] = true;
	}
	bool CheckNumber(int x, int y) {
		return x >= 0 && y >= 0 && x < ROWS && y < COLUMNS && cell[x][y] > 0;
	}
	bool CheckCovered(int x, int y){
		return x >= 0 && y >= 0 && x < ROWS && y < COLUMNS && cell[x][y] == -1;
	}
	int GetVertexID(pair<int, int> index, bool isMine) {
		return (index.first * COLUMNS + index.second) << 1 | isMine;
	}
	void BuildGraph() {
		//mines.insert(make_pair(2, 0));
		//mines.insert(make_pair(2, 2));
		memset(graph, 0, sizeof(graph));
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLUMNS; ++j) {
				if (CheckNumber(i, j)) {
					vector<pair<int, int> > unknown;
					int cnt = 0;
					for (int k = 0; k < 8; ++k) {
						if (CheckCovered(i + DX[k], j + DY[k])) {
							if (mines.find(make_pair(i + DX[k], j + DY[k])) == mines.end()) {
								if (notMines.find(make_pair(i + DX[k], j + DY[k])) == notMines.end()) {
									unknown.push_back(make_pair(i + DX[k], j + DY[k]));
								}
							}
							else {
								++cnt;
							}
						}
					}
					if (cell[i][j] == 1 && unknown.size() > 0) {
						for (int k = 0; k < unknown.size(); ++k) {
							for (int l = 0; l < unknown.size(); ++l) {
								if (k != l) {
									int u = GetVertexID(unknown[k], true);
									int v = GetVertexID(unknown[l], false);
									AddEdge(u, v);
								}
							}
						}
					}
					if (cell[i][j] == cnt) {
						for (int k = 0; k < unknown.size(); ++k) {
							int u = GetVertexID(unknown[k], true);
							int v = GetVertexID(unknown[k], false);
							AddEdge(u, v);
						}
					}
					if (unknown.size() + cnt == cell[i][j]) {
						for (int k = 0; k < unknown.size(); ++k) {
							int u = GetVertexID(unknown[k], false);
							int v = GetVertexID(unknown[k], true);
							AddEdge(u, v);
						}
					}
					if (unknown.size() + cnt == cell[i][j] + 1) {
						for (int k = 0; k < unknown.size(); ++k) {
							for (int l = 0; l < unknown.size(); ++l) {
								if (k != l) {
									int u = GetVertexID(unknown[k], false);
									int v = GetVertexID(unknown[l], true);
									AddEdge(u, v);
								}
							}
						}
					}
				}
			}
		}
		
		vector<pair<int, int> > unknown;
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLUMNS; ++j) {
				if (CheckCovered(i, j) && mines.find(make_pair(i, j)) == mines.end() && notMines.find(make_pair(i, j)) == notMines.end()) {
					unknown.push_back(make_pair(i, j));
				}
			}
		}
		if (MINES - mines.size() == 0) {
			for (int i = 0; i < unknown.size(); ++i) {
				int u = GetVertexID(unknown[i], true);
				int v = GetVertexID(unknown[i], false);
				AddEdge(u, v);
			}
		}
		if (MINES - mines.size() == 1) {
			for (int i = 0; i < unknown.size(); ++i) {
				for (int j = 0; j < unknown.size(); ++j) {
					if (i != j) {
						int u = GetVertexID(unknown[i], true);
						int v = GetVertexID(unknown[j], false);
						AddEdge(u, v);
					}
				}
			}
		}
		if (unknown.size() == MINES - mines.size()) {
			for (int i = 0; i < unknown.size(); ++i) {
				int u = GetVertexID(unknown[i], false);
				int v = GetVertexID(unknown[i], true);
				AddEdge(u, v);
			}
		}
		if (unknown.size() == MINES - mines.size() + 1) {
			for (int i = 0; i < unknown.size(); ++i) {
				for (int i = 0; i < unknown.size(); ++i) {
					for (int j = 0; j < unknown.size(); ++j) {
						if (i != j) {
							int u = GetVertexID(unknown[i], false);
							int v = GetVertexID(unknown[j], true);
							AddEdge(u, v);
						}
					}
				}
			}
		}
	}
	bool vis[GRAPHSIZE];
	bool BFS(int u, int v) {
		queue<int> q;
		q.push(u);
		while (!q.empty()) {
			int t = q.front();
			q.pop();
			for (int i = 0; i < GRAPHSIZE; ++i) {
				if (vis[i] == false && graph[t][i]) {
					vis[i] = true;
					q.push(i);
				}
			}
		}
		return vis[v];
	}
	bool IsMine(pair<int, int> index) {
		memset(vis, 0, sizeof(vis));
		int u = GetVertexID(index, false);
		int v = GetVertexID(index, true);
		if (u < GRAPHSIZE&&v < GRAPHSIZE) {
			return BFS(u, v);
		}
		else {
			return false;
		}
	}
	bool IsNotMine(pair<int, int> index) {
		memset(vis, 0, sizeof(vis));
		int u = GetVertexID(index, true);
		int v = GetVertexID(index, false);
		if (u < GRAPHSIZE&&v < GRAPHSIZE) {
			return BFS(u, v);
		}
		else {
			return false;
		}
	}
	MineSweeper() {
		notMines.clear();
		bool flag = true;
		calcBegin = clock();
		while (flag) {
			BuildGraph();
			flag = false;
			for (int i = 0; i < ROWS; ++i) {
				for (int j = 0; j < COLUMNS; ++j) {
					if (CheckCovered(i, j) && mines.find(make_pair(i, j)) == mines.end()) {
						if (IsNotMine(make_pair(i, j))) {
							flag = true;
							notMines.insert(make_pair(i, j));
						}
						if (IsMine(make_pair(i, j))) {
							flag = true;
							mines.insert(make_pair(i, j));
						}
					}
				}
			}
		}
		calcEnd = clock();
		calcTime += calcEnd - calcBegin;
		//Print();
	}
	void Print() {
		printf("mines = %d\n", mines.size());
		if (mines.size() > 0) {
			for (auto it = mines.begin(); it != mines.end(); ++it) {
				printf("(%d, %d) ", it->first, it->second);
			}
		}
		printf("\nnot mines = %d\n", notMines.size());
		if (notMines.size() > 0) {
			for (auto it = notMines.begin(); it != notMines.end(); ++it) {
				printf("(%d, %d) ", it->first, it->second);
			}
		}
		puts("");
	}
};
RECT rect;
int GetCellValue(int x, int y) {
	swap(x, y);
	int beginX = rect.left + 16 + x * 16;
	int beginY = rect.top + 102 + y * 16;
	COLORREF color = GetPixel(GetDC(NULL), beginX, beginY);
	if (color == RGB(255, 255, 255)) {
		return -1;
	}
	color = GetPixel(GetDC(NULL), beginX + 7, beginY + 7);
	switch (color) {
	case RGB(0, 0, 255):return 1; break;
	case RGB(0, 128, 0):return 2; break;
	case RGB(255, 0, 0):return 3; break;
	case RGB(0, 0, 128):return 4; break;
	case RGB(128, 0, 0):return 5; break;
	case RGB(0, 128, 128):return 6; break;
	case RGB(128, 128, 128):return 8; break;
	case RGB(192, 192, 192): break;
	default:
		printf("(%d, %d, %d)\n", GetRValue(color), GetGValue(color), GetBValue(color));
		return -2; break;
	}
	color = GetPixel(GetDC(NULL), beginX + 7, beginY + 8);
	if (color == RGB(0, 0, 0)) {
		return 7;
	}
	else if (color == RGB(192, 192, 192)) {
		return 0;
	}
	else {
		puts("oh no!");
		return -2;
	}
}
int readCount;
bool GetData() {
	++readCount;
	bool flag = true;
	readBegin = clock();
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLUMNS; ++j) {
			if (cell[i][j] == -1) {
				
				cell[i][j] = GetCellValue(i, j);

			}
			if (cell[i][j] == -2) {
				flag &= false;
			}
		}
	}				
	readEnd = clock();
	printf("%d Read Time = %.3f s\n", readCount, (readEnd - readBegin) / 1000.0);
	readTime += readEnd - readBegin;
	/*
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			if (cell[i][j] == -1) {
				printf("- ");
			}
			else {
				printf("%d ", cell[i][j]);
			}
		}
		puts("");
	}
	*/
	return flag;
}
void Click(int x, int y) {
	swap(x, y);
	int beginX = rect.left + 16 + x * 16;
	int beginY = rect.top + 102 + y * 16;
	SetCursorPos(beginX + 7, beginY + 7);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	Sleep(200);
}
void RandomClick(MineSweeper mn) {
	vector<pair<int, int> > unknown;
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLUMNS; ++j) {
			if (cell[i][j] == -1 && mn.mines.find(make_pair(i, j)) == mn.mines.end()) {
				unknown.push_back(make_pair(i, j));
			}
		}
	}
	srand(time(0));
	if (unknown.size() > 0) {
		int r = rand() % unknown.size();
		Click(unknown[r].first, unknown[r].second);
		printf("Random Click = (%d, %d)\n", unknown[r].first, unknown[r].second);
	}
}
void init() {
	memset(cell, -1, sizeof(cell));
	//cell[1][0] = 4;
	//cell[1][2] = 5;
}
int main(){
	while (true) {
		system("pause");
		init();
		MineSweeper mn;
		//mn.Print();
		
		readTime = calcTime = 0;
		readCount = 0;
		//system("pause");
		//Sleep(5000);
		HWND hwnd = FindWindow("É¨À×", 0);
		GetWindowRect(hwnd, &rect);
		memset(cell, -1, sizeof(cell));
		bool ok = false;
		while (GetData()) {
			MineSweeper mn;
			if (mn.notMines.size() > 0) {
				for (auto it = mn.notMines.begin(); it != mn.notMines.end(); ++it) {
					printf("Safe Click = (%d, %d)\n", it->first, it->second);
					Click(it->first, it->second);
				}
			}
			else {
				RandomClick(mn);
			}
			if (mn.mines.size() == MINES) {
				puts("win!!!");
				ok = true;
				break;
			}
		}
		if (ok == false) {
			puts("lose...");
		}
		printf("Read Time = %.3f s\n", readTime / readCount / 1000);
		//printf("Calc Time = %.3f s\n", calcTime / 1000);
		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLUMNS; ++j) {
				if (cell[i][j] == -1) {
					printf("X");
				}
				else if (cell[i][j] == -2) {
					printf("?");
				}
				else {
					printf("%d", cell[i][j]);
				}
			}
			puts("");
		}
		
	}
	return 0;
}
/*
0: 192 192 192
1: 0 0 255
2: 0 128 0
3: 255 0 0
4: 0 0 128
5: 128 0 0
6: 0 128 128
7: 0 0 0
8: 128 128 128
*/