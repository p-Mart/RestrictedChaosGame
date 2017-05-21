#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 900;

struct Vertex {
	int x;
	int y;
	Vertex(int sx, int sy) {
		x = sx;
		y = sy;
	}
};

int mergeChannel(int a, int b, float amount) {
	float result = ((1 - amount)*a) + (b*amount);
	return int(result);
}

SDL_Color lerpColor(SDL_Color a, SDL_Color b, float amount) {
	SDL_Color result;
	result.r = mergeChannel(a.r, b.r, amount);
	result.g = mergeChannel(a.g, b.g, amount);
	result.b = mergeChannel(a.b, b.b, amount);
	result.a = mergeChannel(a.a, b.a, amount);
	return result;
}

vector<Vertex> calculateVertices(int n) {
	int radius = SCREEN_WIDTH / 2;
	double central_angle = 2 * M_PI / n;
	double start_angle;
	if (n % 2 == 0)
		start_angle = M_PI / 2;
	else
		start_angle = M_PI / 2 - central_angle / 2;

	vector<Vertex> vertices;
	for (int i = 1; i < n+1; i++) {
		double angle = start_angle + i*central_angle;
		int x = SCREEN_WIDTH / 2 + radius*cos(angle);
		int y = SCREEN_HEIGHT / 2 + radius*sin(angle);
		vertices.push_back(Vertex(x, y));
	}
	return vertices;
}

int main(int argc, char** argv) {
	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *win = SDL_CreateWindow("Chaos Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == nullptr) {
		SDL_DestroyWindow(win);
		SDL_Quit();
		return 1;
	}

	bool quit = false;
	SDL_Event e;

	//File for debug logging.
	ofstream ofs("log.txt", ofstream::out);

	//Initialize Vertices.
	int number_vertices = 4;

	vector<Vertex> vertices;
	vertices = calculateVertices(number_vertices);
	
	Vertex prev_vertex = Vertex(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT);
	int prev_vertex_index = rand() % number_vertices;
	int next_vertex_index = 0;

	//Initialize Colors for drawing.
	SDL_Color start_color;
	start_color.r = 0;
	start_color.g = 255;
	start_color.b = 255;
	start_color.a = 255;

	SDL_Color end_color;
	end_color.r = 255;
	end_color.g = 20;
	end_color.b = 147;
	end_color.a = 255;

	//Make the background black.
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);

	//Control variables
	double jump_distance = 2;

	//Main loop
	while (!quit) {
		//Event catching
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_RIGHT:
						jump_distance = jump_distance + 0.01;
						break;
					case SDLK_LEFT:
						if (jump_distance > 1)
							jump_distance = jump_distance - 0.01;
						break;
					case SDLK_SPACE:
						SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
						SDL_RenderClear(ren);
						break;
					default:
						break;
				}
			}
			else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_UP:
						number_vertices++;
						vertices = calculateVertices(number_vertices);
						break;
					case SDLK_DOWN:
						if (number_vertices > 3) {
							number_vertices--;
							vertices = calculateVertices(number_vertices);
						}
						break;
					default:
						break;
				}
			}
		}

		//Get the next vertex as per chaos game rule.
		if (prev_vertex_index >= number_vertices) {
			prev_vertex_index = rand() % vertices.size();
			next_vertex_index = 0;
			continue;
		}
		ofs << prev_vertex_index << "\n";

		Vertex vert = vertices[prev_vertex_index];
		//Chaos Rule 1
		
		//next_vertex_index = rand() % vertices.size();
		//while (next_vertex_index == prev_vertex_index)
		//	next_vertex_index = rand() % vertices.size();
		
		bool prob = rand() % 2;
		bool step = rand() % 2; //0 or 1
		//Chaos Rule 2
		/*
		if (!step) {
			next_vertex_index = ((((next_vertex_index - 1) % number_vertices) + number_vertices) % number_vertices);
		}
		else {
			next_vertex_index = ((((next_vertex_index + 1) % number_vertices) + number_vertices) % number_vertices);
		}
		*/
		//Chaos Rule 3
		
		if (!step) {
			next_vertex_index = ((((next_vertex_index - 2) % number_vertices) + number_vertices) % number_vertices);
		}
		else {
			next_vertex_index = ((((next_vertex_index + 2) % number_vertices) + number_vertices) % number_vertices);
		}
		
		


		//Set drawing color of points.
		//SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_Color chosen_color;
		chosen_color = lerpColor(start_color, end_color, float(prev_vertex_index) / float(number_vertices - 1));
		SDL_SetRenderDrawColor(ren, chosen_color.r, chosen_color.g, chosen_color.b, chosen_color.a);

		//Calculate the drawing position of the chosen vertex.
		int xdiff = prev_vertex.x - vert.x;
		int ydiff = prev_vertex.y - vert.y;

		//Select new point based on a restricted random walk.
		Vertex next_vertex = Vertex(prev_vertex.x - xdiff / jump_distance, prev_vertex.y - ydiff / jump_distance);

		//Update the previously chosen vertex to the currently chosen vertex.
		prev_vertex = next_vertex;
		prev_vertex_index = next_vertex_index;

		//Draw point
		SDL_RenderDrawPoint(ren, next_vertex.x, next_vertex.y);

		//Update Screen
		SDL_RenderPresent(ren);
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	ofs.close();

	return 0;
}
