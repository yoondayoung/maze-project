/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
#include <stack>
#include <queue>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS",false,false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		if (isOpen) {
			freeMemory();
			this->isOpen = 0;
		}
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		if (isBFS) isBFS = 0;
		if (isOpen)
		{
			DFS(); 
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Show BFS") {
		if (isDFS) isDFS = 0;
		if (isOpen)
		{
			BFS();
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (isOpen) {
		ofSetColor(0, 0, 0);
		ofSetLineWidth(4);
		int x = 2, y = 2;
		for (int i = 0; i < HEIGHT; i++) {
			x = 2;
			for (int j = 0; j < WIDTH; j++) {
				if (input[i][j] == '+') {
					if (j == 0) {
						if (input[i][j + 1] == '-') {
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
							}
							ofDrawLine(x-2, y, x + 10, y);
							x += 10;
						}
						else {
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
								else ofDrawLine(x, y, x + 4, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
								else ofDrawLine(x, y, x + 4, y);
							}
							x += 10;
						}
					}
					else if (j == WIDTH-1) {
						if (input[i][j - 1] == '-') {
							ofDrawLine(x, y, x + 12, y);
							x += 10;
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
							}
						}
						else {
							x += 10;
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
								else ofDrawLine(x, y, x + 4, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
								else ofDrawLine(x, y, x + 4, y);
							}
						}
					}
					else {
						if (input[i][j - 1] == '-'&&input[i][j + 1] == '-') {
							ofDrawLine(x-2, y, x + 22, y);
							x += 10;
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
							}
							x += 10;
						}
						else if (input[i][j - 1] == '-') {
							ofDrawLine(x, y, x + 12, y);
							x += 10;
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
							}
							x += 10;
						}
						else if (input[i][j + 1] == '-') {
							x += 10;
							ofDrawLine(x-2, y, x+10, y);
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
							}
							x += 10;
						}
						else {
							x += 10;
							if (i != 0) {
								if (input[i - 1][j] == '|') ofDrawLine(x, y - 10, x, y);
								else ofDrawLine(x - 2, y, x + 2, y);
							}
							if (i != HEIGHT-1) {
								if (input[i + 1][j] == '|') ofDrawLine(x, y, x, y + 10);
								else ofDrawLine(x - 2, y, x + 2, y);
							}
							x +=10;
						}
					}
				}
				if (input[i][j] == '-') {
					ofDrawLine(x, y, x + 20, y);
					x += 20;
				}
				if (input[i][j] == '|') {
					if (j == 0) {
						ofDrawLine(x, y-10, x, y+10);
						x += 10;
					}
					else {
						x += 10;// x: 10 먼저 이동
						ofDrawLine(x, y-10, x, y + 10);
						x += 10;
					}
				}
				if (input[i][j] == ' ') {
					x += 20;
				}
			}
			y += 20;
		}
	}
	if (isDFS)
	{
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if (isBFS)
	{
		ofSetLineWidth(5);
		if (isOpen)
			bfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		ofSetColor(200);
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose( "User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;
			
			
			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			for (auto line : buffer.getLines()) {
				this->input.push_back(line);
				idx = line.length();
				cnt++;
			}
			HEIGHT = cnt;
			WIDTH = idx;
			target_x = WIDTH - 2; // target의 x좌표
			target_y = HEIGHT - 2; // target의 y좌표 
			// visited 초기화
			vector<int> temp;
			for (int i = 0; i < HEIGHT; i++) {
				for (int j = 0; j < WIDTH; j++) {
					temp.push_back(0);
				}
				this->visited.push_back(temp);
				temp.clear();
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
		}
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수
	vector<vector<int>>().swap(visited);
	vector<string>().swap(input);
	this->HEIGHT = 0;
	this->WIDTH = 0;
}

void ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	stack<pos> S;
	int dir, x, y, nextX, nextY;
	pos temp;
	temp.x = 1; temp.y = 1; temp.d = 0;
	S.push(temp);
	visited[1][1] = 1;
	while (S.empty() == false) {
		x = S.top().x; y = S.top().y; dir = S.top().d;
		// 처음 아니면 저장
		if (!(x == 1 && y == 1 && dir == 0)) {
			dfs_fail_path.push_back(S.top());
		}
		S.pop();
		dir++;
		while (dir < 4) {
			switch (dir) {
			case 0: nextX = x; nextY = y - 2; break;
			case 1: nextX = x + 2; nextY = y; break;
			case 2: nextX = x; nextY = y + 2; break;
			case 3: nextX = x - 2; nextY = y; break;
			}
			if (1 > nextX || nextX > WIDTH - 2 || 1 > nextY || nextY > HEIGHT - 2) {
				// 위치 유효성 검사 -> 유효하지 않으면 다음 방향으로
				dir++;
				continue;
			}
			if (nextX == target_x && nextY == target_y) { // 도착점에 도달했으면
				if ((dir == 0 && input[y - 1][x] == ' ') || (dir == 1 && input[y][x + 1] == ' ') || (dir == 2 && input[y + 1][x] == ' ') || (dir == 3 && input[y][x - 1] == ' ')) {
					temp.x = x; temp.y = y; temp.d = dir;
					S.push(temp);
					while (S.empty() == false) {
						dfs_path.push_back(S.top());
						S.pop(); // top에는 제일 처음 좌표
					}
					isDFS = 1;
					return;
				}
				else dir++; // 벽으로 막혀있어서 못감
			}
			else if (visited[nextY][nextX] == 0) {
				if ((dir == 0 && input[y - 1][x] == ' ') || (dir == 1 && input[y][x + 1] == ' ') || (dir == 2 && input[y + 1][x] == ' ') || (dir == 3 && input[y][x - 1] == ' ')) {
					visited[nextY][nextX] = 1;
					temp.x = x; temp.y = y; temp.d = dir;
					S.push(temp);
					x = nextX; y = nextY; dir = 0;
				}
				else dir++; // 벽으로 막혀있어서 못감
			}
			else dir++; // 이미 갔었던 곳
		}
	}
	// 길못찾았으면 여기로 옴
	isDFS = 1;
	return;
}
void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)
	// fail stack -> 붉은선, real path -> 녹색선
	int x1, x2, y1, y2, dir;
	int ox = 20, oy = 20;
	ofSetColor(0, 255, 0);
	for (int i = 0; i < dfs_path.size(); i++) {
		x1 = dfs_path[i].x;
		y1 = dfs_path[i].y;
		dir = dfs_path[i].d;
		switch (dir) {
		case 0: x2 = x1; y2 = y1 - 2; break;
		case 1: x2 = x1 + 2; y2 = y1; break;
		case 2: x2 = x1; y2 = y1 + 2; break;
		case 3: x2 = x1 - 2; y2 = y1; break;
		}
		ofDrawLine(ox+(x1/2)*40, oy+(y1/2)*40, ox+(x2/2)*40, oy+(y2/2)*40);
	}
	ofSetColor(255, 0, 0);
	for (int i = 0; i < dfs_fail_path.size(); i++) {
		x1 = dfs_fail_path[i].x;
		y1 = dfs_fail_path[i].y;
		dir = dfs_fail_path[i].d;
		switch (dir) {
		case 0: x2 = x1; y2 = y1 - 2; break;
		case 1: x2 = x1 + 2; y2 = y1; break;
		case 2: x2 = x1; y2 = y1 + 2; break;
		case 3: x2 = x1 - 2; y2 = y1; break;
		}
		ofDrawLine(ox + (x1 / 2) * 40, oy + (y1 / 2) * 40, ox + (x2 / 2) * 40, oy + (y2 / 2) * 40);
	}
}

void ofApp::BFS()
{	
	queue<pos> Q;
	vector<pos> T;
	int dir, pdir, x, y, nextX, nextY, px, py;
	int find = 0; // 길 찾았는지 여부
	pos temp;
	int	current;
	pair<int, int> par;
	// (1,1) queue에 넣기
	temp.x = 1; temp.y = 1; temp.d = -1;
	Q.push(temp);
	pdir = -1;
	while (Q.empty() == false) {
		x = Q.front().x; y = Q.front().y;
		T.push_back(Q.front());
		if (x == target_x && y == target_y) { // 도착점에 도달했으면
			// 현재좌표부터 처음까지 경로 찾기 -> success path에 저장
			current = T.size() - 1;
			while (T[current].d != -1) {
				bfs_path.push_back(T[current]);
				switch (T[current].d) {
				case 0: px = T[current].x; py = T[current].y + 2; break;
				case 1: px = T[current].x - 2; py = T[current].y; break;
				case 2: px = T[current].x; py = T[current].y - 2; break;
				case 3: px = T[current].x + 2; py = T[current].y; break;
				}
				T[current].d = -2; // fail_path에 추가하면 안됨
				for (int i = 0; i < T.size(); i++) {
					if (T[i].x == px && T[i].y == py) {
						current = i; //부모 위치로 이동
						break;
					}
				}
			}
			break;
		}
		if (0 <= Q.front().d) {
			pdir = (Q.front().d + 2) % 4;
		}
		Q.pop();
		dir = 0;
		while (dir < 4) {
			if (dir == pdir) {
				dir++; continue;
			} // 왔던 방향 -> check 안함
			switch (dir) {
			case 0: nextX = x; nextY = y - 2; break;
			case 1: nextX = x + 2; nextY = y; break;
			case 2: nextX = x; nextY = y + 2; break;
			case 3: nextX = x - 2; nextY = y; break;
			}
			if (1 > nextX || nextX > WIDTH - 2 || 1 > nextY || nextY > HEIGHT - 2) {
				// 위치 유효성 검사 -> 유효하지 않으면 다음 방향으로
				dir++;
				continue;
			}
			if ((dir == 0 && input[y - 1][x] == ' ') || (dir == 1 && input[y][x + 1] == ' ') || (dir == 2 && input[y + 1][x] == ' ') || (dir == 3 && input[y][x - 1] == ' ')) {
				temp.x = nextX; temp.y = nextY; temp.d = dir;
				Q.push(temp);
			}
			dir++; // 다음 방향 검사
		}
	}
	// while루프 끝나면 여기로 옴
	for (int i = 0; i < T.size(); i++) {
		if (T[i].d >= 0) bfs_fail_path.push_back(T[i]);
	}
	isBFS = 1;
	return;
}

void ofApp::bfsdraw()
{
	int x1, x2, y1, y2, dir;
	int ox = 20, oy = 20;
	ofSetColor(0, 255, 0);
	for (int i = 0; i < bfs_path.size(); i++) {
		x2 = bfs_path[i].x;
		y2 = bfs_path[i].y;
		dir = bfs_path[i].d;
		switch (dir) {
		case 0: x1 = x2; y1 = y2 + 2; break;
		case 1: x1 = x2 - 2; y1 = y2; break;
		case 2: x1 = x2; y1 = y2 - 2; break;
		case 3: x1 = x2 + 2; y1 = y2; break;
		}
		ofDrawLine(ox + (x1 / 2) * 40, oy + (y1 / 2) * 40, ox + (x2 / 2) * 40, oy + (y2 / 2) * 40);
	}
	ofSetColor(255, 0, 0);
	for (int i = 0; i < bfs_fail_path.size(); i++) {
		x2 = bfs_fail_path[i].x;
		y2 = bfs_fail_path[i].y;
		dir = bfs_fail_path[i].d;
		switch (dir) {
		case 0: x1 = x2; y1 = y2 + 2; break;
		case 1: x1 = x2 - 2; y1 = y2; break;
		case 2: x1 = x2; y1 = y2 - 2; break;
		case 3: x1 = x2 + 2; y1 = y2; break;
		}
		ofDrawLine(ox + (x1 / 2) * 40, oy + (y1 / 2) * 40, ox + (x2 / 2) * 40, oy + (y2 / 2) * 40);
	}
}