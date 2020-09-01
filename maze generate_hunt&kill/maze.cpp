#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "maze.h"
using namespace std;

int main() {
	int n, m;
	cin >> n >> m;
	Maze maze = Maze(n, m);
	maze.generate_maze();
	maze.print_form();
}
