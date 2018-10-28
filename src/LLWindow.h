#ifndef _LLWINDOW_H_
#define _LLWINDOW_H_
#include "SDL/include/SDL.h"

class LLWindow 
{
public:
	LLWindow();
	~LLWindow();
	int init_window();
	int test();
private:
	SDL_Window *m_hwnd;
	SDL_Renderer *m_renderer;
	SDL_Texture *m_texture;
public:
private:


};

#endif