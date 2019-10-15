#include <stdio.h>
#include <SDL/SDL.h>
#include "window.h"

void init_window(){

	SDL_Surface *screen = NULL;
	SDL_Surface *menu = NULL;
	SDL_Rect menu_p;
	SDL_Surface *gradient[256]= {NULL};

	SDL_Init(SDL_INIT_VIDEO);
	
	int height = 640;
	int width = 480;
	int pixel = 32;

	screen = SDL_SetVideoMode(width,height,pixel,SDL_HWSURFACE );
	if(screen == NULL){
		//Error
	}

	SDL_WM_SetCaption("OCR - Les Croisillons", NULL);
	
	Uint32 color = SDL_MapRGB(screen->format, 75,75,75);

	SDL_FillRect(screen, NULL, color);

	menu = SDL_CreateRGBSurface(SDL_HWSURFACE,width/2,height,pixel,0,0,0,0);
	//SDL_FillRect(menu, NULL, SDL_MapRGB(screen->format,0,0,255));
	
	for(int i = 0; i<=255; i++){
		gradient[i] = SDL_CreateRGBSurface(SDL_HWSURFACE,width,height,32,0,0,0,0);
	}

	for(int i =0; i<= 255; i++){
		menu_p.x = 0;
		menu_p.y = i;

		SDL_FillRect(gradient[i], NULL, SDL_MapRGB(screen->format,i,i,i));
		SDL_BlitSurface(gradient[i],NULL,screen,&menu_p);
	}

	menu_p.x = 0;
	menu_p.y =0;

	SDL_BlitSurface(menu, NULL, screen, &menu_p);

	SDL_Flip(screen); //Refresh the screen
	pause();
	
	for(int i=0; i<=255;i++){
		SDL_FreeSurface(gradient[i]);
	}
	SDL_FreeSurface(menu); //Free element "menu"
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
				break;
			case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE: // Esc to quit the program
                    continuer = 0;
                    break;
            }
            break;
        }
    }
}
