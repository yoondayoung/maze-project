#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <random>
#include <vector>
#include <map>
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
using namespace std;

typedef struct {
	int x, y;
}pos;

typedef struct edge_ {
	pos from; //x,y��ǥ
	int d; // ���� ����
}edge;

class Maze {
	private:
		int n, m; // �̷� ��, �� size
		vector < vector <int> > maze_select; // maze ���� ����
		vector < vector <char> > maze_form; // ������� form
		vector<edge> path; // ������ ����
	public:
		Maze(int n, int m) {
			vector<int> tempv1;
			vector<char> tempv2;

			this->n = n;
			this->m = m;
			for (int i = 0; i < m; i++) {
				tempv1.clear();
				for (int j = 0; j < n; j++) {
					tempv1.push_back(0);
				}
				maze_select.push_back(tempv1);
			}
				

			for (int i = 0; i < 2 * m + 1; i++) {
				tempv2.clear();
				for (int j = 0; j < 2 * n + 1; j++) {
					if (i % 2 == 0) {
						if (j % 2 == 0) tempv2.push_back('+');
						else tempv2.push_back('-');
					}
					else {
						if (j % 2 == 0) tempv2.push_back('|');
						else tempv2.push_back(' ');
					}
				}
				maze_form.push_back(tempv2);
			}
		} // ������
		void print_form();
		pos process_dir(int fx, int fy, int dir);
		bool check_move(int x, int y, int d);
		void hunt_kill(pos start); // ������ �޾Ƽ� ������ �� ��� Ȯ���ϸ� ���ư��� �Լ�
		void add_edge(int frox, int froy, int dir); // path�� ���� �߰�
		pos find_new(); // �� ���ٺ��� ���Ӱ� hunt_kill �� ĭ ã��
		bool check_complete(); // ��� ĭ�� visited �ƴ���
		void draw_form(); // ���� �̿��� ��� ������ִ� �Լ�
		void generate_maze(); // ���������� �̷� ���� ����� �Լ�
};

void Maze::print_form() {
	ofstream outF("maze.maz", ios::trunc);

	for (int i = 0; i < 2 * m + 1; i++) {
		for (int j = 0; j < 2 * n + 1; j++) {
			outF << this->maze_form[i][j];
		}
		outF << endl;
	}
	outF.close();
}

pos Maze::process_dir(int fx, int fy, int dir) {
	pos temp;
	switch (dir) {
	case UP: temp.x = fx; temp.y = fy - 1;
		break;
	case RIGHT: temp.x = fx + 1; temp.y = fy;
		break;
	case DOWN: temp.x = fx; temp.y = fy + 1;
		break;
	case LEFT: temp.x = fx - 1; temp.y = fy;
		break;
	}
	return temp;
}

bool Maze::check_move(int x, int y, int d)
{
	if (x <= 0 && d == LEFT) return false;
	else if (x >= this->n-1 && d == RIGHT) return false;
	else if (y <= 0 && d == UP) return false;
	else if (y >= this->m-1 && d == DOWN) return false;
	else return true;
}

void Maze::hunt_kill(pos start) {
	int fx = start.x;
	int fy = start.y;
	int dir;
	int dir_c = 0; // ��� ���� check�߳� Ȯ��
	bool find_road = true;
	pos temp;
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> rand(0, 3);
	
	while (find_road == true) { // �� �̻� �� �� �ִ� ���� ���� ������ ����
		find_road = false;
		bool check_exist[4] = { false, };
		while (dir_c < 4) { // �� ���� select = 0�̸�
			dir = rand(gen);
			if (check_exist[dir] == false) { // �ߺ� �ȵǸ� ���
				check_exist[dir] = true;
				dir_c++; // ���� �ϳ� check ����
			}
			else {
				continue; // �ߺ��Ǹ� �ٽ� dir ����
			}
			if (!check_move(fx, fy, dir)) continue;
			temp = process_dir(fx, fy, dir);
			if (maze_select[temp.y][temp.x] == 0) { // �� �� �ִ� ���̶��
				maze_select[temp.y][temp.x] = 1;
				find_road = true; // �� ã����
				add_edge(fx, fy, dir); // �����߰�
				fx = temp.x; fy = temp.y; // ��ġ ����
				break; // ���� ĭ�� ���� ĭ����(��ĭ �̵��� ��Ȳ)
			}
		}
	}
	
}

void Maze::add_edge(int frox, int froy, int dir) {
	edge e;
	e.from.x = frox;
	e.from.y = froy;
	e.d = dir;
	this->path.push_back(e);
}

pos Maze::find_new() {
	int dir;
	pos temp;
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> rand(0, 3);

	for (int i = 0; i < this->m; i++) {
		for (int j = 0; j < this->n; j++) {
			if (this->maze_select[i][j] == 0) {
				int dir_c = 0;
				bool check_exist[4] = { false, };
				while (dir_c < 4) { // �� ���� select = 0�̸�
					dir = rand(gen);
					if (check_exist[dir] == false) { // �ߺ� �ȵǸ� ���
						check_exist[dir] = true;
						dir_c++; // ���� �ϳ� check ����
					}
					else {
						continue; // �ߺ��Ǹ� �ٽ� dir ����
					}
					if (!check_move(j, i, dir)) continue;
					temp = process_dir(j, i, dir);
					if (maze_select[temp.y][temp.x] == 1) { // �� �� �ִ� ���̶��
						maze_select[i][j] = 1;
						temp.x = j; temp.y = i;
						add_edge(temp.x, temp.y, dir); // -> ����?
						return temp;
					}
				}
			}
		}
	}
}

bool Maze::check_complete()
{	
	for (int i = 0; i < this->m; i++) {
		for (int j = 0; j < this->n; j++) {
			if (this->maze_select[i][j] == 0) {
				return false;
			}
		}
	}
	return true;
}

void Maze::draw_form()
{	
	int x, y;

	for (int i = 0; i < this->path.size(); i++) {
		switch (this->path[i].d) {
			case UP: 
				x = this->path[i].from.x * 2 + 1;
				y = this->path[i].from.y * 2;
				break;
			case RIGHT:
				x = this->path[i].from.x * 2 + 2;
				y = this->path[i].from.y * 2 + 1;
				break;
			case DOWN:
				x = this->path[i].from.x * 2 + 1;
				y = this->path[i].from.y * 2 + 2;
				break;
			case LEFT:
				x = this->path[i].from.x * 2;
				y = this->path[i].from.y * 2 + 1;
				break;
		}
		this->maze_form[y][x] = ' ';
	}
}

void Maze::generate_maze()
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> ranm(0, this->m-1);
	uniform_int_distribution<int> rann(0, this->n-1);

	pos temp;
	temp.x = ranm(gen);
	temp.y = rann(gen);
	this->maze_select[temp.y][temp.x] = 1;

	while (1) {
		hunt_kill(temp);
		if (check_complete()) break;
		temp = find_new();
	}

	draw_form();
}
