#include <iostream>
#include <cmath>
#include <windows.h>
#include <string>
#include <vector>
#include "mazegenerator.cpp"
#include <queue>

using namespace std;
#define PI 3.14159265359

/*
from:
	0x2588
	0x2593
	0x2592
	0x2591
*/

typedef struct {
	vector<int> way;
} vertex;

typedef struct {
	double x;
	double y;
} vec;

typedef struct {
	double x;
	char t;
} len;

class Screen {
	int WIDTH;
	int HEIGHT;
	double fov;
	int maze_width;
	int maze_height;
	double wall_height;
	wchar_t* sc;
	clock_t start;
	double dt = 0;
	vec* loc_ray = new vec;
	int winX;
	int winY;
	vertex* arr;

	vec* cam_pos = new vec;
	vec* cam_look_direction = new vec;

	len* length;
	char* map;
	char* s;
	double speed;
	double sensitivity;
	double dist_poject_plate;
	double cam_radius;
	double cam_scale;

	POINT mousePointlast;
	POINT mousePointlocal;
															//Windows
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	void rayCast() {
		double angle;
		double tanangle;
		int sx;
		int sy;
		double x;
		double y;
		char t;

		for (int i = 0; i < WIDTH; i++) {
			angle = ((-(fov / 2) + i * (fov / WIDTH)) * PI) / 180;
			loc_ray->x = cam_look_direction->x * cos(angle) + cam_look_direction->y * sin(angle);
			loc_ray->y = -cam_look_direction->x * sin(angle) + cam_look_direction->y * cos(angle);

			sx = (loc_ray->x > 0) ? 1. : -1.;
			sy = (loc_ray->y > 0) ? 1. : -1.;

			double j;
				//x crossing
			for (j = 0; j < maze_width && j > -maze_width; j += sx) {
				x = j + (1. - (cam_pos->x - int(cam_pos->x)));

				if (loc_ray->x != 0 && x * sx > 0) {
					y = (x / loc_ray->x) * loc_ray->y;

					t = isCatch(cam_pos->x + x, cam_pos->y + y, 0.001);
					if (t == '1' || t == '5') {
						length[i].x = sqrt(x * x + y * y) * cos(angle);
						length[i].t = t;
						break;
					}
				}
			}
				//y crossing
			for (j = 0; j < maze_height && j > -maze_height; j += sy) {
				y = j + (1. - (cam_pos->y - int(cam_pos->y)));

				if (loc_ray->y != 0 && y * sy > 0) {
					x = (y / loc_ray->y) * loc_ray->x;

					t = isCatch(cam_pos->x + x, cam_pos->y + y, 0.001);
					if (t == '1' || t == '5') {
						if (length[i].x < 0.01) {
							length[i].x = sqrt(x * x + y * y) * cos(angle);
							length[i].t = t;
						}
						else {
							if (length[i].x > sqrt(x * x + y * y) * cos(angle)) {
								length[i].x = sqrt(x * x + y * y) * cos(angle);
								length[i].t = t;
							}
						}	
						break;
					}
				}
			}
		}
	}

	void frame() {
		wchar_t s;
		for (int i = 0; i < WIDTH; i++) {
			length[i].x *= cam_scale;

			s = ' ';
			if (length[i].x < 20.25 && length[i].t == '1') s = 0x2591;
			if (length[i].x < 8.25 && length[i].t == '1') s = 0x2592;
			if (length[i].x < 3.25 && length[i].t == '1') s = 0x2593;
			if (length[i].x < 1.25 && length[i].t == '1') s = 0x2588;

			if (length[i].t == '5') s = 'Z';

			if (length[i].x < cam_radius - 0.01) s = ' ';

			double h = ((double)HEIGHT - (wall_height / length[i].x) * dist_poject_plate) / 2;
			for (int j = 0; j < HEIGHT; j++) {
					//floor
				if (j > HEIGHT - h) {
					sc[j * WIDTH + i] = ',';
				}
					//sky
				else if (j <= h) {
					sc[j * WIDTH + i] = ' ';
				}
					//walls
				else {
					sc[j * WIDTH + i] = s;
				}
			}
		}
	}

	void draw() {
														//Windows
		WriteConsoleW(console, sc, WIDTH * HEIGHT, NULL, NULL);
	}

	void control() {
		double x;
		double y;
		double vx;
		double vy;
		double mx;
		char t;

		vx = cam_look_direction->x * cos(PI / 2) - cam_look_direction->y * sin(PI / 2);
		vy = cam_look_direction->x * sin(PI / 2) + cam_look_direction->y * cos(PI / 2);

		if (GetCursorPos(&mousePointlocal)) {
			mx = mousePointlast.x - mousePointlocal.x;

			x = cam_look_direction->x * cos((PI / 180) * sensitivity * mx * dt) - cam_look_direction->y * sin((PI / 180) * sensitivity * mx * dt);
			y = cam_look_direction->x * sin((PI / 180) * sensitivity * mx * dt) + cam_look_direction->y * cos((PI / 180) * sensitivity * mx * dt);

			cam_look_direction->x = x;
			cam_look_direction->y = y;

			SetCursorPos(100, 100);
			GetCursorPos(&mousePointlast);
		}

		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000) {
			exit(1);
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			t = isCatch(cam_pos->x + vx * speed * dt, cam_pos->y, cam_radius);
			if (t != '1')
				cam_pos->x += vx * speed * dt;

			t = isCatch(cam_pos->x, cam_pos->y + vy * speed * dt, cam_radius);
			if (t != '1')
				cam_pos->y += vy * speed * dt;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			t = isCatch(cam_pos->x - vx * speed * dt, cam_pos->y, cam_radius);
			if (t != '1')
				cam_pos->x -= vx * speed * dt;

			t = isCatch(cam_pos->x, cam_pos->y - vy * speed * dt, cam_radius);
			if (t != '1')
				cam_pos->y -= vy * speed * dt;
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			t = isCatch(cam_pos->x + cam_look_direction->x * speed * dt, cam_pos->y, cam_radius);
			if (t != '1') 
				cam_pos->x += cam_look_direction->x * speed * dt;

			t = isCatch(cam_pos->x, cam_pos->y + cam_look_direction->y * speed * dt, cam_radius);
			if (t != '1')
				cam_pos->y += cam_look_direction->y * speed * dt;
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			t = isCatch(cam_pos->x - cam_look_direction->x * speed * dt, cam_pos->y, cam_radius);
			if (t != '1')
				cam_pos->x -= cam_look_direction->x * speed * dt;

			t = isCatch(cam_pos->x, cam_pos->y - cam_look_direction->y * speed * dt, cam_radius);
			if (t != '1')
				cam_pos->y -= cam_look_direction->y * speed * dt;
		}
	}

	void clearMap() {
		for (int i = 0; i < WIDTH * HEIGHT; i++) sc[i] = ' ';
		for (int i = 0; i < WIDTH; i++) { 
			length[i].x = 0;
			length[i].t = '0';
		}
	}

	char isCatch(double x, double y, double rad) {
		if (x < maze_width && y < maze_height && x >= 0 && y >= 0) {
			if (map[int(y + rad) * maze_width + int(x + rad)] != '0')
				return map[int(y + rad) * maze_width + int(x + rad)];

			if (map[int(y + rad) * maze_width + int(x - rad)] != '0')
				return map[int(y + rad) * maze_width + int(x - rad)];

			if (map[int(y - rad) * maze_width + int(x + rad)] != '0')
				return map[int(y - rad) * maze_width + int(x + rad)];

			if (map[int(y - rad) * maze_width + int(x - rad)] != '0')
				return map[int(y - rad) * maze_width + int(x - rad)];

			return '0';
		}
		return '1';
	}

	void makeWinText() {
		const wchar_t b = 0x2588;
		const wchar_t e = ' ';

		const int xScale = 40;
		const int yScale = 20;

		int xWs = (WIDTH / 2) - (7 * xScale / 2) - WIDTH / 4;
		int yWs = (HEIGHT / 2) - (7 * yScale / 2);
		int xIs = (WIDTH / 2) - (7 * xScale / 2);
		int yIs = (HEIGHT / 2) - (7 * yScale / 2);
		int xNs = (WIDTH / 2) - (7 * xScale / 2) + WIDTH / 4;
		int yNs = (HEIGHT / 2) - (7 * yScale / 2);

		double gSpeed = 0.1;
		double step = 70;

		int xW, yW, xI, yI, xN, yN;

		wchar_t lW[49] = {e, e, e, e, e, e, e,
						  e, e, e, e, e, e, e,
						  e, b, e, e, e, b, e,
						  e, b, e, b, e, b, e,
						  e, b, e, b, e, b, e,
						  e, b, e, b, e, b, e,
						  e, e, b, e, b, e, e };

		wchar_t lI[49] = {e, e, e, b, e, e, e,
						  e, e, e, e, e, e, e,
						  e, e, b, b, e, e, e,
						  e, e, e, b, e, e, e,
						  e, e, e, b, e, e, e,
						  e, e, e, b, e, e, e,
						  e, e, b, b, b, e, e };

		wchar_t lN[49] = {e, e, e, e, e, e, e,
						  e, e, e, e, e, e, e,
						  e, b, e, b, b, e, e,
						  e, b, b, e, e, b, e,
						  e, b, e, e, e, b, e,
						  e, b, e, e, e, b, e,
						  e, b, e, e, e, b, e };

		int x = 0;
		int y = 0;
		double t = 0;

		while (true) {
			xW = xWs + step * tan(t);
			yW = yWs + step * sin(t);
			xI = xIs + step * tan(t);
			yI = yIs + step * sin((t) + 2. * PI / 3.);
			xN = xNs + step * tan(t);
			yN = yNs + step * sin((t) + 4. * PI / 3.);

			t += gSpeed;

			for (int i = 0; i < WIDTH; i++) {
				for (int j = 0; j < HEIGHT; j++) {
					sc[j * WIDTH + i] = e;

					x = i - xW;
					y = j - yW;
					if (x >= 0 && x < 7 * xScale && y >= 0 && y < 7 * yScale)
						if (lW[(y / yScale) * 7 + x / xScale] == b)
							sc[j * WIDTH + i] = b;

					x = i - xI;
					y = j - yI;
					if (x >= 0 && x < 7 * xScale && y >= 0 && y < 7 * yScale)
						if (lI[(y / yScale) * 7 + x / xScale] == b)
							sc[j * WIDTH + i] = b;

					x = i - xN;
					y = j - yN;
					if (x >= 0 && x < 7 * xScale && y >= 0 && y < 7 * yScale)
						if (lN[(y / yScale) * 7 + x / xScale] == b)
							sc[j * WIDTH + i] = b;

				}
			}

			WriteConsoleW(console, sc, WIDTH * HEIGHT, NULL, NULL);
		}
	}

	bool isWin() {
		if (int(cam_pos->x) == winX && int(cam_pos->y) == winY)
			return true;
		return false;
	}

	void dtime() {
		clock_t end = clock();
		dt = (end - start);
		start = end;
	}

	bool isFindExit() {
		if (GetAsyncKeyState((unsigned short)'Z') & 0x8000)
			return true;
		return false;
	}

	void findExit() {
		for (int i = 0; i < maze_height * maze_width; i++)
			arr[i].way.clear();

		dijkstra();
		vector<int> currentWay = arr[winY * maze_width + winX].way;

		int locI = 0;
		double rotateSpeed = 15;
		double walkSpeed = 0.001;
		int locX = currentWay[locI] % maze_width;
		int locY = currentWay[locI] / maze_width;
		int nextX = locX;
		int nextY = locY;
		double lookMustBeX;
		double lookMustBeY;
		double x;
		double y;
		double lr;
		double tmpx;
		double tmpy;

		cam_pos->x = locX + .5;
		cam_pos->y = locY + .5;
		locI++;

		while (isFindExit()) {
			dtime();
			clearMap();
			
			if (locX == nextX && locY == nextY) {
				//rotate player
				lookMustBeX = currentWay[locI] % maze_width - locX;
				lookMustBeY = currentWay[locI] / maze_width - locY;

				if (lookMustBeX == 1 && lookMustBeY == 0)
					lr = (cam_look_direction->y < 0.) ? 1 : -1;
				else if (lookMustBeX == 0 && lookMustBeY == -1)
					lr = (cam_look_direction->x < 0.) ? 1 : -1;
				else if (lookMustBeX == -1 && lookMustBeY == 0)
					lr = (cam_look_direction->y < 0.) ? -1 : 1;
				else if (lookMustBeX == 0 && lookMustBeY == 1)
					lr = (cam_look_direction->x < 0.) ? -1 : 1;

				x = cam_look_direction->x * cos(lr * (PI / 180) * sensitivity * rotateSpeed * dt) -
					cam_look_direction->y * sin(lr * (PI / 180) * rotateSpeed * sensitivity * dt);
				y = cam_look_direction->x * sin(lr * (PI / 180) * sensitivity * rotateSpeed * dt) +
					cam_look_direction->y * cos(lr * (PI / 180) * rotateSpeed * sensitivity * dt);

				cam_look_direction->x = x;
				cam_look_direction->y = y;

				if (areEqual(cam_look_direction->x, lookMustBeX, 0.1) && areEqual(cam_look_direction->y, lookMustBeY, 0.15)) {
					nextX = currentWay[locI] % maze_width;
					nextY = currentWay[locI] / maze_width;
					cam_look_direction->x = lookMustBeX;
					cam_look_direction->y = lookMustBeY;
				}
			}
			else {
				if (areEqual(cam_pos->x, nextX + .5, 0.1) && areEqual(cam_pos->y, nextY + .5, 0.1)) {
					locX = nextX;
					locY = nextY;
					cam_pos->x = locX + .5;
					cam_pos->y = locY + .5;
					locI++;
					if (locI == currentWay.size()) break;
				}

				//move player
				cam_pos->x += cam_look_direction->x * walkSpeed * dt;
				cam_pos->y += cam_look_direction->y * walkSpeed * dt;
			}
			
			rayCast();
			frame();
			draw();
		}
	}

	bool areEqual(double x, double y, double eps) {
		if (x - eps < y && x + eps > y)
			return true;
		return false;
	}

	void dijkstra() {
		int startX = int(cam_pos->x);
		int startY = int(cam_pos->y);
		queue<int> qu;
		int loc;

		arr[startY * maze_width + startX].way.push_back(startY * maze_width + startX);
		qu.push(startY * maze_width + startX);

		while (qu.size() != 0) {
			loc = qu.front();
			qu.pop();

			if (map[loc - 1] != '1' && (arr[loc - 1].way.size() == 0 || arr[loc - 1].way.size() > arr[loc].way.size())){
				arr[loc - 1].way = arr[loc].way;
				arr[loc - 1].way.push_back(loc - 1);
				qu.push(loc - 1);
			}

			if (map[loc + 1] != '1' && (arr[loc + 1].way.size() == 0 || arr[loc + 1].way.size() > arr[loc].way.size())) {
				arr[loc + 1].way = arr[loc].way;
				arr[loc + 1].way.push_back(loc + 1);
				qu.push(loc + 1);
			}

			if (map[loc - maze_width] != '1' &&
				(arr[loc - maze_width].way.size() == 0 || arr[loc - maze_width].way.size() > arr[loc].way.size())) {
				arr[loc - maze_width].way = arr[loc].way;
				arr[loc - maze_width].way.push_back(loc - maze_width);
				qu.push(loc - maze_width);
			}

			if (map[loc + maze_width] != '1' &&
				(arr[loc + maze_width].way.size() == 0 || arr[loc + maze_width].way.size() > arr[loc].way.size())) {
				arr[loc + maze_width].way = arr[loc].way;
				arr[loc + maze_width].way.push_back(loc + maze_width);
				qu.push(loc + maze_width);
			}
		}
	}

public:
	Screen(int WIDTH, int HEIGHT, int fov, int maze_width, int maze_height, char* map,
		   double spawnX, double spawnY, int winX, int winY, double speed, double sensitivity) {
		this->WIDTH = WIDTH;
		this->HEIGHT = HEIGHT;
		this->fov = fov;
		this->speed = speed;
		this->sensitivity = sensitivity;
		this->dist_poject_plate = (double(WIDTH) / 2.) / tan((fov / 2.) * PI / 180.);

		this->cam_radius = 0.2;
		this->cam_scale = 2.;
		this->wall_height = 0.3;

		sc = new wchar_t[WIDTH * HEIGHT + 1];
		sc[WIDTH * HEIGHT] = '\0';

		length = new len[WIDTH]();
		this->map = map;

		this->maze_width = maze_width;
		this->maze_height = maze_height;

		arr = new vertex[maze_height * maze_width];

		cam_pos->x = spawnX;
		cam_pos->y = spawnY;
		this->winX = winX;
		this->winY = winY;
		cam_look_direction->x = 0.;
		cam_look_direction->y = 1.;

		GetCursorPos(&mousePointlast);

		clock_t start = clock();
		s = new char[WIDTH * HEIGHT + 1];
		s[WIDTH * HEIGHT] = '\0';
	}

	void run() {
		while (true) {
			dtime();
			clearMap();
			control();
			rayCast();
			frame();
			draw();
			if (isWin())
				makeWinText();
			if (isFindExit())
				findExit();
		}
	}

	~Screen() {
		delete[] sc;
		delete cam_pos;
		delete[] length;
		delete[] s;
		delete loc_ray;
		delete cam_look_direction;
		delete[] length;
		delete[] s;
		delete[] arr;
	}

};


int main() {
	const int WIDTH = 1500;
	const int HEIGHT = 300;
	const int fov = 70;

	const int maze_width = 30;
	const int maze_height = 30;
	double spawnX = 1.5;
	double spawnY = 1.5;
	int winX = maze_width - 2;
	int winY = maze_height - 2;
	double walkSpeed = 0.0015;
	double sensitivity = 0.0035;

	//char map[maze_width * maze_height] = 
	//  {'1','1','1','1','1','1','1','1','1','1',
	//   '1','0','1','0','0','0','0','0','0','1',
	//   '1','0','1','0','1','1','0','1','0','1',
	//   '1','0','1','1','0','0','0','1','0','1',
	//   '1','0','1','0','0','0','0','1','0','1',
	//   '1','0','1','0','0','0','0','1','0','1',
	//   '1','0','1','0','1','0','0','1','0','1',
	//   '1','0','1','0','1','0','1','0','0','1',
	//   '1','0','0','0','1','0','1','0','0','1',
	//   '1','1','1','1','1','1','1','1','1','1' };

	Labyrinth maze;
	
	int* t = maze.read_labyrinth(maze.create_labyrinth(maze_width, maze_height));
	char* map = new char[maze_width * maze_height];

	for (int i = 0; i < maze_width * maze_height; i++) {
		map[i] = t[i] + '0';
	}

	map[1 * maze_width + 1] = '0';
	map[winY * maze_width + winX] = '5';
	
	Screen sc(WIDTH, HEIGHT, fov, maze_width, maze_height, map, spawnX, spawnY, winX, winY, walkSpeed, sensitivity);
	
	sc.run();

	delete[] map;

	return 0;
}

