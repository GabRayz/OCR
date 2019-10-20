#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

void init_window(char *filepath);
int isClicked(SDL_MouseButtonEvent mouse, SDL_Rect btn, int btn_height, int btn_width );

#endif
