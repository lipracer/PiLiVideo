#include "src/LLWindow.h"
#include <iostream>
using namespace std;

LLWindow::LLWindow() 
{
	
}

LLWindow::~LLWindow() 
{
	
}

int LLWindow::init_window() 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return -1;
	}
	

	m_hwnd = SDL_CreateWindow("Player",100, 100, 500, 500, SDL_WINDOW_RESIZABLE);

	m_renderer = SDL_CreateRenderer(m_hwnd, -1, 0);

	char *buf = new char[500 * 500 * 3];

	SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(buf, 500, 500, 24, 3*500,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);



	SDL_Event event;
	bool quit = false;
	while (quit == false)
	{
		//While there's event to handle
		while (SDL_PollEvent(&event))
		{
			SDL_WaitEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				SDL_DestroyWindow(m_hwnd);
				quit = true;
				break;
			case SDL_USEREVENT:
				SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
				SDL_RenderPresent(m_renderer);
				cout << "SDL_USEREVENT" << endl;
				break;
			default:
				break;
			}
		}
	}	
	return true;
}

int LLWindow::test()
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	SDL_PushEvent(&event);
	return 0;
}