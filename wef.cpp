#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;
using namespace std::chrono;
const int FRAMESTOP = 20;
char a[40];
int MOVEXCONST = 6, MOVEYCONST = 6;
bool samt = false;
bool held[350];
int jump = 0, lstjump = 0;
SDL_Window* window = SDL_CreateWindow("Spiky jump", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 520, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
int x = 200, y = 400;
int cameray = 0;
bool spikeL[42], spikeR[42];
bool tL[42], tR[42];
int score = (0);
int Coins = 0;
bool lost = false, pause = true;
TTF_Font *font;
SDL_Rect rect1;	SDL_Rect leftborder, rightborder, downborder;
SDL_Surface *surface;
SDL_Color textColor = {14, 185, 25, 0};
SDL_Texture *texture1;
string s;
time_point<std::chrono::high_resolution_clock> framestart, frameend, allend;

void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

void check_col (int xx, int yy) {
	int ny = y - cameray;
	int dx = (xx - x);
	int dy = (yy - ny);
	if (dx * dx + dy * dy < 1600) {
		lost = true;
	}
}

void WriteScore () {
	if (font == NULL) {
		cout << "wef";
		exit(1);
	}
	s = to_string(score);
	int sz = (int)s.size();
	int C = 4 * sz;
	for (int i = 0; i < 40; i++)
		a[i] = '\0';
	for (int i = 0; i < (int)s.size(); i++)
		a[i] = s[i];
	if (score == -1) {
		C = 4 * 36;
		a[0] = 'Y';
		a[1] = 'o';
		a[2] = 'u';
		a[3] = ' ';
		a[4] = ' ';  
		a[5] = 'l';
		a[6] = 'o';
		a[7] = 's';
		a[8] = 't';
		a[9] = ' ';
		a[10] = ' ';
		a[11] = 'r';
		a[12] = ' ';
		a[13] = ' ';
		a[14] = 't';
		a[15] = 'o';
		a[16] = ' ';
		a[17] = ' ';
		a[18] = 'r';
		a[19] = 'e';
		a[20] = 't';
		a[21] = 'r';
		a[22] = 'y';
		a[23] = ' ';
		a[24] = ' ';
		a[25] = 'q';
		a[26] = ' ';
		a[27] = ' ';
		a[28] = 't';
		a[29] = 'o';
		a[30] = ' ';
		a[31] = ' ';
		a[32] = 'e';
		a[33] = 'x';
		a[34] = 'i';
		a[35] = 't';
	}
	get_text_and_rect(renderer, 260 - C, 70, a, font, &texture1, &rect1);
	SDL_RenderCopy(renderer, texture1, NULL, &rect1);
	SDL_DestroyTexture(texture1);
}

void Drawtriangle (SDL_Renderer *renderer, int x, int y1, int y2, int h) {
	long double few = (y2 - y1) / 2;
	for (int y = y1; y <= y2; y++) {
		long double wef = min(y - y1, y2 - y);
		long double l = ceil((h * wef) / few);
		int L = (int)l;
		SDL_RenderDrawLine (renderer, x, y, x + L, y);
		check_col (x + L, y);
	}
}

void Drawtriangle2 (SDL_Renderer *renderer, int y, int x1, int x2, int h) {
	long double few = (x2 - x1) / 2;
	for (int x = x1; x <= x2; x++) {
		long double wef = min(x - x1, x2 - x);
		long double l = ceil((h * wef) / few);
		int L = (int)l;
		SDL_RenderDrawLine (renderer, x, y, x, y + L);
		check_col (x, y + L);
	}
}

void Generate_new_spike () {
	mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
	for (int i = 0; i < 41; i++) {
		tL[i] = spikeL[i + 1];
		tR[i] = spikeR[i + 1];
	}
	if ((-cameray) / 50 <= 40) {
		tL[41] = ((rng() % 12) == 6);
		tR[41] = ((rng() % 12) == 3);
	} else {
		tL[41] = ((rng() % 9) == 2);
		tR[41] = ((rng() % 9) == 4);
	}
	for (int i = 0; i < 42; i++) {
		spikeL[i] = tL[i];
		spikeR[i] = tR[i];
	}
}

void draw_circle(SDL_Renderer *renderer, int x, int y, int radius)
{
    for (int w = 0; w < radius * 2 + 1; w++) {
		int dx = radius - w;
		int dy = ceil(sqrt (radius * radius - dx * dx));
		SDL_RenderDrawLine (renderer, x + dx, y - dy - cameray, x + dx, y + dy - cameray);
    }
}

void Calc () {
	if (lost) {
		score = -1;
		return;
	}
	if (pause) {
		return;
	}
	if (samt) {
		x -= 8;
	} else {
		x += 8;
	}
	y += 1;
	if (jump) {
		y -= jump;
		jump--;
		lstjump--;
	}
	int newcameray = y - 300;
	newcameray = min(cameray, newcameray);
	while (newcameray / 15 < cameray / 15) {
		Generate_new_spike();
		cameray -= 15;
	}
	cameray = newcameray;
	if (x < 50 || x > 470)
		samt = !samt;
	score = (-cameray) / 50 + Coins;
}

void Draw () {
	leftborder.x = 0;
	leftborder.y = 0;
	leftborder.h = 600;
	leftborder.w = 10;
	rightborder.x = 510;
	rightborder.y = 0;
	rightborder.h = 600;
	rightborder.w = 10;
	downborder.x = 0;
	downborder.y = 590;
	downborder.h = 10;
	downborder.w = 520;
	SDL_SetRenderDrawColor(renderer, 200, 10, 13, 255);
	for (int i = 0; i < 42; i++) {
		if (spikeL[i]) {
			Drawtriangle (renderer, 10, 600 - 15 * i - cameray % 15, 600 - 15 * i - cameray % 15 + 14, +20);
		}
		if (spikeR[i]) {
			Drawtriangle (renderer, 510, 600 - 15 * i - cameray % 15, 600 - 15 * i - cameray % 15 + 14, -20);
		}
	}
	for (int i = -2; i < 600; i += 25) {
		Drawtriangle2(renderer, 590, i, i + 24, -23);
	}
	SDL_RenderFillRect(renderer, &leftborder);
	SDL_RenderFillRect(renderer, &rightborder);
	SDL_RenderFillRect(renderer, &downborder);
	SDL_SetRenderDrawColor(renderer, 0, 122, 122, 255);
	draw_circle(renderer, x, y, 40);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	draw_circle (renderer, x, y, 5);
	if (samt) {
		draw_circle (renderer, x - 23, y, 5);
		if (lost) {
			SDL_RenderDrawLine (renderer, x - 12, y + 10 - cameray, x - 23, y + 20 - cameray);
			SDL_RenderDrawLine (renderer, x, y + 20 - cameray, x - 12, y + 10 - cameray);
		} else {
			SDL_RenderDrawLine (renderer, x - 23, y + 10 - cameray, x - 12, y + 20 - cameray);
			SDL_RenderDrawLine (renderer, x - 12, y + 20 - cameray, x, y + 10 - cameray);
		}
		if (lost) {
			SDL_RenderDrawLine (renderer, x - 12, y + 11 - cameray, x - 23, y + 21 - cameray);
			SDL_RenderDrawLine (renderer, x, y + 21 - cameray, x - 12, y + 11 - cameray);
		} else {
			SDL_RenderDrawLine (renderer, x - 23, y + 11 - cameray, x - 12, y + 21 - cameray);
			SDL_RenderDrawLine (renderer, x - 12, y + 21 - cameray, x, y + 11 - cameray);
		}
	} else {
		draw_circle (renderer, x + 23, y, 5);
		if (lost) {
			SDL_RenderDrawLine (renderer, x + 12, y + 10 - cameray, x + 23, y + 20 - cameray);
			SDL_RenderDrawLine (renderer, x, y + 20 - cameray, x + 12, y + 10 - cameray);
		} else {
			SDL_RenderDrawLine (renderer, x + 23, y + 10 - cameray, x + 12, y + 20 - cameray);
			SDL_RenderDrawLine (renderer, x + 12, y + 20 - cameray, x, y + 10 - cameray);
		}
		if (lost) {
			SDL_RenderDrawLine (renderer, x + 12, y + 11 - cameray, x + 23, y + 21 - cameray);
			SDL_RenderDrawLine (renderer, x, y + 21 - cameray, x + 12, y + 11 - cameray);
		} else {
			SDL_RenderDrawLine (renderer, x + 23, y + 11 - cameray, x + 12, y + 21 - cameray);
			SDL_RenderDrawLine (renderer, x + 12, y + 21 - cameray, x, y + 11 - cameray);
		}
	}
	WriteScore();
	SDL_SetRenderDrawColor(renderer, 0, 0, 20, 0);
}

int main(int argc, char* argv[]) {
	TTF_Init();
	font = TTF_OpenFont("data/pixelated.ttf", 24);

	while (true) {
		pause = true;
		samt = false;
 		held[350];
 		jump = 0, lstjump = 0;
	 	x = 200, y = 400;
 		cameray = 0;
	 	spikeL[42] = {}, spikeR[42] = {};
 		score = (0);
	 	Coins = 0;
 		lost = false;
		for (int i = 0; i < 21; i++)
			Generate_new_spike();
		SDL_Event e;
		while (true) {
			framestart = std::chrono::high_resolution_clock::now();
			bool fff = true;
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT) {
					exit(0);
				} else if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.sym == ' ') {
						jump = 10;
						lstjump = 10; 
						pause = false;
					} else if (e.key.keysym.sym == 'r') {
						fff = false;
						break;
					} else if (e.key.keysym.sym == 'q') {
						exit(0);
					} else if (e.key.keysym.sym == 27) {
						pause = true;
					}
				} 
			}
			if (!fff) {
				break;
			}
			Calc();
			SDL_RenderClear(renderer);
			Draw();
			SDL_RenderPresent(renderer);
			frameend = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> frameduration = frameend - framestart;
			int framepassed = ceil(frameduration.count() * 1000);
			int remainingframes = FRAMESTOP - framepassed;
			std::this_thread::sleep_for(std::chrono::milliseconds(remainingframes));
			allend = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> allduration = allend - framestart;
			int allframes = ceil(allduration.count() * 1000);
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
