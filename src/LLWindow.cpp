#include "src/LLWindow.h"
#include <iostream>
#include <thread>
using namespace std;

LLWindow::LLWindow(int width, int height) : m_width(width), m_height(height), m_buf_mtx()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != -1)
    {
        m_hwnd = SDL_CreateWindow("Player",100, 100, m_width, m_height, SDL_WINDOW_RESIZABLE);
        
        m_renderer = SDL_CreateRenderer(m_hwnd, -1, 0);
        
        m_buf = new char[m_width * m_height * 3];
//        m_surface = SDL_CreateRGBSurfaceWithFormatFrom(m_buf,
//                                           m_width,
//                                           m_height,
//                                           24,
//                                           3 * m_width,
//                                        SDL_PIXELFORMAT_BGR24);

        //m_surface = SDL_CreateRGBSurfaceFrom(m_buf, m_width, m_height, 24, 3 * m_width, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
        //m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
        m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_TARGET, m_width, m_height);
    }
}

LLWindow::~LLWindow() 
{
    if(m_buf)
    {
        delete m_buf;
    }
}

int LLWindow::init_window() 
{
	SDL_Event event;
	bool quit = false;
    SDL_Rect rect = {0, 0, m_width, m_height};
    unsigned int format = 0;
    int access = 0;
    int w = 0;
    int h = 0;
    
	while (quit == false)
	{
		//SDL_WaitEvent(&event);
		if (SDL_WaitEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
                delete m_buf;
				SDL_DestroyWindow(m_hwnd);
				quit = true;
				break;
			case SDL_RENDER_TARGETS_RESET:
                m_buf_mtx.lock();

                SDL_UpdateTexture(m_texture, &rect, m_buf, m_width * 3);
                //SDL_QueryTexture(m_texture, &format, &access, &w, &h);
//                m_surface = SDL_CreateRGBSurfaceFrom(m_buf, m_width, m_height, 24, 3 * m_width, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);
                //m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
                //SDL_SetRenderTarget(m_renderer, m_texture);
                //SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0x00);
                SDL_RenderClear(m_renderer);
                //SDL_RenderDrawRect(m_renderer,&rect);
                //SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x00, 0x00, 0x00);
                //SDL_RenderFillRect(m_renderer, &rect);
                //SDL_SetRenderTarget(m_renderer, NULL);
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
    event.type = SDL_RENDER_TARGETS_RESET;
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
