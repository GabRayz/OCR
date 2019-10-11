#include <stdio.h>
#include <SDL/SDL.h>

void init_window(){
	SDL_Init(SDL_INIT_VIDEO);
	int height = 640;
	int width = 480;
	int pixel = 32;
	SDL_SetVideoMode(width,height,pixel,SDL_HWSURFACE | SDL_RESIZABLE);

	// pause();
	
	SDL_Quit();
}

void pause()
{
    int continuer = 1;
    SDL_Event event;
 
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }
    }
}
