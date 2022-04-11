#include <iostream>
#include <cmath>
#include <windows.h>
#include <string>

using namespace std;
#define PI 3.141592

/*
from:
	0x2588
	0x2593
	0x2592
	0x2591
*/

typedef struct {
	double x;
	double y;
} vec;

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

	vec* cam_pos = new vec;
	vec* cam_look_direction = new vec;

	double *length;
	char* map;
	char* s;
	double speed;
	double sensitivity;
	double dist_poject_plate;
	double cam_radius;
	double cam_scale;

															//Windows
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	void rayCast() {
		double angle;
		double tanangle;
		int sx;
		int sy;
		double x;
		double y;

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

					if (isCatch(cam_pos->x + x, cam_pos->y + y, 0.001)) {
						length[i] = sqrt(x * x + y * y) * cos(angle);
						break;
					}
				}
			}
				//y crossing
			for (j = 0; j < maze_height && j > -maze_height; j += sy) {
				y = j + (1. - (cam_pos->y - int(cam_pos->y)));

				if (loc_ray->y != 0 && y * sy > 0) {
					x = (y / loc_ray->y) * loc_ray->x;

					if (isCatch(cam_pos->x + x, cam_pos->y + y, 0.001)) {
						if (length[i] < 0.01)
							length[i] = sqrt(x * x + y * y) * cos(angle);
						else 
							length[i] = min(sqrt(x * x + y * y) * cos(angle), length[i]);
						break;
					}
				}
			}
		}
	}

	void frame() {
		wchar_t s;
		for (int i = 0; i < WIDTH; i++) {
			length[i] *= cam_scale;

			s = ' ';
			if (length[i] < 20) s = 0x2591;
			if (length[i] < 8) s = 0x2592;
			if (length[i] < 3) s = 0x2593;
			if (length[i] < 1) s = 0x2588;
			if (length[i] < cam_radius - 0.01) s = ' ';

			double h = ((double)HEIGHT - (wall_height / length[i]) * dist_poject_plate) / 2;
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

														//Linux
		/*for (int i = 0; i < WIDTH * HEIGHT; i++) {
			s[i] = ' ';
			if (sc[i] == 0x2591) s[i] = '.';
			if (sc[i] == 0x2592) s[i] = '/';
			if (sc[i] == 0x2593) s[i] = '#';
			if (sc[i] == 0x2588) s[i] = '@';
		}
		cout << s;*/
		
	}

	void control() {
		double x;
		double y;
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			x = cam_look_direction->x * cos((PI / 180) * sensitivity * dt) - cam_look_direction->y * sin((PI / 180) * sensitivity * dt);
			y = cam_look_direction->x * sin((PI / 180) * sensitivity * dt) + cam_look_direction->y * cos((PI / 180) * sensitivity * dt);

			cam_look_direction->x = x;
			cam_look_direction->y = y;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			x = cam_look_direction->x * cos(-(PI / 180) * sensitivity * dt) - cam_look_direction->y * sin(-(PI / 180) * sensitivity * dt);
			y = cam_look_direction->x * sin(-(PI / 180) * sensitivity * dt) + cam_look_direction->y * cos(-(PI / 180) * sensitivity * dt);

			cam_look_direction->x = x;
			cam_look_direction->y = y;
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			if (!isCatch(cam_pos->x + cam_look_direction->x * speed * dt, cam_pos->y, cam_radius)) {
				cam_pos->x += cam_look_direction->x * speed * dt;
			}
			if (!isCatch(cam_pos->x, cam_pos->y + cam_look_direction->y * speed * dt, cam_radius)) {
				cam_pos->y += cam_look_direction->y * speed * dt;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			if (!isCatch(cam_pos->x - cam_look_direction->x * speed * dt, cam_pos->y, cam_radius)) {
				cam_pos->x -= cam_look_direction->x * speed * dt;
			}
			if (!isCatch(cam_pos->x, cam_pos->y - cam_look_direction->y * speed * dt, cam_radius)) {
				cam_pos->y -= cam_look_direction->y * speed * dt;
			}
		}
	}

	void clearMap() {
		for (int i = 0; i < WIDTH * HEIGHT; i++) sc[i] = ' ';
		for (int i = 0; i < WIDTH; i++) length[i] = 0;
	}

	bool isCatch(double x, double y, double rad) {
		if (x < maze_width && y < maze_height && x >= 0 && y >= 0) {
			if (map[int(y + rad) * maze_width + int(x + rad)] == '1' ||
				map[int(y + rad) * maze_width + int(x - rad)] == '1' ||
				map[int(y - rad) * maze_width + int(x + rad)] == '1' ||
				map[int(y - rad) * maze_width + int(x - rad)] == '1')
				return true;
			return false;
		}
		return true;
	}

	void dtime() {
		clock_t end = clock();
		dt = (end - start);
		start = end;
	}

public:
	Screen(int WIDTH, int HEIGHT, int fov, int maze_width, int maze_height, char* map, double spawnX, double spawnY, double speed, double sensitivity) {
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

		length = new double[WIDTH]();
		this->map = map;

		this->maze_width = maze_width;
		this->maze_height = maze_height;

		cam_pos->x = spawnX;
		cam_pos->y = spawnY;
		cam_look_direction->x = 0.6;
		cam_look_direction->y = 0.6;

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
		}
	}

	~Screen() {
		delete[] sc;
		delete cam_pos;
		delete[] length;
		delete[] s;
	}

};


int main() {
							//Linux		(100, 40)
							//Windows	(600, 170)
	const int WIDTH = 1200;
	const int HEIGHT = 300;
	const int fov = 70;

	int maze_width = 10;
	int maze_height = 10;
	double spawnX = 3.5;
	double spawnY = 3.5;
	double speed = 0.003;
	double sensitivity = 0.11;
	
	char* map = (char*)
"\
1111111111\
1010000001\
1010011001\
1010001001\
1011111001\
1000000001\
1110111011\
1010110111\
1000000001\
1111111111";

	Screen sc(WIDTH, HEIGHT, fov, maze_width, maze_height, map, spawnX, spawnY, speed, sensitivity);
	
	sc.run();

	delete[] map;

	return 0;
}

