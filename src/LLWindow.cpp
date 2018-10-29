#include "src/LLWindow.h"
#include <iostream>
#include <thread>
using namespace std;

LLWindow::LLWindow(int width, int height) : m_width(width), m_height(height), m_buf_mtx()
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
	

	m_hwnd = SDL_CreateWindow("Player",100, 100, m_width, m_height, SDL_WINDOW_RESIZABLE);

	m_renderer = SDL_CreateRenderer(m_hwnd, -1, 0);

	m_buf = new char[m_width * m_height * 3];

	SDL_Event event;
	bool quit = false;
	while (quit == false)
	{
		SDL_WaitEvent(&event);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				SDL_DestroyWindow(m_hwnd);
				quit = true;
				break;
			case SDL_USEREVENT:
                m_buf_mtx.lock();
                m_surface = SDL_CreateRGBSurfaceFrom(m_buf, m_width, m_height, 24, 3 * m_width, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
                m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
                SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
                SDL_RenderPresent(m_renderer);
				cout << "SDL_USEREVENT" << endl;
                m_buf_mtx.unlock();
				break;
			default:
				break;
			}            
		}
	}	
	return true;
}

int LLWindow::test(char *data)
{

    m_buf_mtx.lock();
    memcpy(m_buf, data, m_width * m_height * 3);
    m_buf_mtx.unlock();
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
	return 0;
}

void LLWindow::wait_times(long long int duration)
{
    cout << "ffmpeg:" << duration << endl;
    long long int duration_ = 0;
    do
    {
        auto cur_point = chrono::steady_clock::now();
        duration_ = (chrono::duration_cast<chrono::milliseconds>(cur_point - m_video_start_tp)).count();
        
        cout << "My:" << duration_ << endl;
        //this_thread::sleep_for(chrono::milliseconds(1));
        
    } while (duration_ < duration);
    cout << "start play" << endl;
}
