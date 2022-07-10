#include "Application.h"
#include "Log.hpp"

Application::Application()
{
	m_window_width = 640;
	m_window_height = 480;

	Log("Initializing SDL");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		throw std::runtime_error("Failed to initialize SDL");
	}

	Log("SDL initialized");

	m_window = SDL_CreateWindow(
		"Pathfinding",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_window_width,
		m_window_height,
		SDL_WINDOW_ALLOW_HIGHDPI);
	if (m_window == nullptr)
	{
		throw std::runtime_error("Window creation failed.");
	}

	Log("Window created");

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == nullptr)
	{
		throw std::runtime_error("Failed to create renderer");
	}

	Log("Renderer created");
}

Application::~Application()
{
	Log("Destroying renderer");
	SDL_DestroyRenderer(m_renderer);
	Log("Destorying window");
	SDL_DestroyWindow(m_window);
	Log("Quitting SDL");
	SDL_Quit();
}

void Application::Visualize(std::vector<ColorBGR> pixels)
{
	SDL_Texture* texture = SDL_CreateTexture(
		m_renderer,
		SDL_PIXELFORMAT_BGR24,
		SDL_TEXTUREACCESS_STATIC,
		m_path_finder.m_map_width,
		m_path_finder.m_map_height
	);

	if (SDL_UpdateTexture(texture, nullptr, pixels.data(), m_path_finder.m_map_width * 3) != 0)
	{
		throw std::runtime_error("Failed to update texture");
	}

	SDL_SetRenderDrawColor(m_renderer, 117, 117, 117, 255);
	SDL_RenderClear(m_renderer);

	int scale = 6;
	int width = 64;
	int height = 64;
	SDL_Rect r = { m_window_width / 2 - width * scale / 2, m_window_height / 2 - height * scale / 2, width * scale, height * scale };
	SDL_RenderCopy(m_renderer, texture, nullptr, &r);
	SDL_RenderPresent(m_renderer);


	Log("Entering main loop");
	SDL_Event e;
	bool open = true;
	while (open)
	{
		while (SDL_WaitEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				open = false;
			}
		}
	}

	Log("Exiting main loop");
}

void Application::FindPath(const char* file_path)
{
	std::vector<ColorBGR> pixels = LoadMap(file_path);
	m_path_finder.InitializeMap(pixels); 
	std::vector<Vec2i> path = m_path_finder.Search(DirectionEnum::EIGHT);
	
	std::cout << "Path length: " << path.size()  << std::endl;
	for (auto& elem : path)
	{
		int index = m_path_finder.IndexFromVec2(elem);
		pixels[index] = ColorBGR(255, 0, 180);
	}

	pixels[m_path_finder.IndexFromVec2(path[0])] = ColorBGR(255, 0, 0);
	pixels[m_path_finder.IndexFromVec2(path[path.size() - 1])] = ColorBGR(0, 0, 255);
	
	Visualize(pixels);
}

std::vector<ColorBGR> Application::LoadMap(const char* file_path)
{
	SDL_Surface* surface = SDL_LoadBMP(file_path);

	if (surface == nullptr)
	{
		throw std::runtime_error("Failed to load map");
	}

	if (surface->format->format != SDL_PIXELFORMAT_BGR24)
	{
		SDL_FreeSurface(surface);
		throw std::runtime_error("PixelFormat not supported. Supported formats: BGR24");
	}

	m_path_finder.m_map_width = surface->w;
	m_path_finder.m_map_height = surface->h;
	
	ColorBGR* ptr = reinterpret_cast<ColorBGR*>(surface->pixels);
	std::vector<ColorBGR> pixels(ptr, ptr + surface->w * surface->h);

	SDL_FreeSurface(surface);

	return (pixels);
}