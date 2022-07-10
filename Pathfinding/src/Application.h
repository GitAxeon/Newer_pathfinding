#pragma once
#include <SDL\SDL.h>
#include "Pathfinder.h"

class Application
{
public:
	Application();
	~Application();

	void FindPath(const char* file_path);
	void Visualize(std::vector<ColorBGR> pixels);

private:
	std::vector<ColorBGR> LoadMap(const char* filepath);

private:
	int m_window_width;
	int m_window_height;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	Pathfinder m_path_finder;
};