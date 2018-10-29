#ifndef _LLWINDOW_H_
#define _LLWINDOW_H_
#include "SDL/include/SDL.h"
#include <chrono>
#include <mutex>

using namespace std;

class LLWindow 
{
public:
	LLWindow(int width, int height);
	~LLWindow();
	int init_window();
	int test(char *data);
    void wait_times(long long int duration);
private:

    
public:
    chrono::time_point<chrono::steady_clock> m_video_start_tp;
private:
    SDL_Window *m_hwnd;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;
    SDL_Surface * m_surface;
    int m_width, m_height;
    mutex m_buf_mtx;
    char *m_buf;
    
};

#endif
