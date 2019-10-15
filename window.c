#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "window.h"

void init_window(){

	SDL_Surface *screen = NULL;
	SDL_Surface *menu = NULL;
	SDL_Rect menu_p;
	SDL_Surface *gradient[256]= {NULL};

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	
	int height = 500;
	int width = 800;
	int pixel = 32;

	screen = SDL_SetVideoMode(width,height,pixel,SDL_HWSURFACE );
	if(screen == NULL){
		//Error
	}

	SDL_WM_SetCaption("OCR - Les Croisillons", NULL);
	
	Uint32 color = SDL_MapRGB(screen->format, 75,75,75);

	SDL_FillRect(screen, NULL, color);



	//Create buttons
	int btn_height = 30;
	int btn_width = 160;
	Uint32 btn_color = SDL_MapRGBA(screen->format,0,20,78,0.1);
	SDL_Surface *btn;
	btn = SDL_CreateRGBSurface(SDL_HWSURFACE,btn_width,btn_height,32,0,0,0,0);
	SDL_FillRect(btn,NULL,btn_color);
	SDL_Rect btn_pos;
	btn_pos.x = 100;
	btn_pos.y = 100;
	SDL_BlitSurface(btn,NULL,screen, &btn_pos);
	SDL_FreeSurface(btn);

	//Create font
	TTF_Font *font;
	font = TTF_OpenFont("Font/Raleway-Medium.ttf",20);
	SDL_Color font_color = {23,56,67};

	//Create Text area
	SDL_Surface *text;
	text = TTF_RenderText_Blended(font,"Upload File",font_color);
	SDL_Rect text_pos;
	text_pos.x = 100;
	text_pos.y = 100;
	SDL_BlitSurface(text,NULL,screen,&text_pos);
	SDL_FreeSurface(text);

	//Create Logo
	SDL_Surface *logo;
	logo = IMG_Load("Img/Logo.png");
	SDL_Rect logo_pos;
	logo_pos.x = 10;
	logo_pos.y = 10;
	SDL_BlitSurface(logo,NULL,screen, &logo_pos);
	SDL_FreeSurface(logo);
	

	//Refresh the screen
	SDL_Flip(screen); 

	//Event Loop
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

			case SDL_MOUSEBUTTONUP:
			if(event.button.y > btn_pos.y 
			   && event.button.y <= btn_pos.y+btn_height
			   && event.button.x > btn_pos.x 
			   && event.button.x <= btn_pos.x+btn_width)
			   {
				   continuer = 0;
			   }
        }
    }
	TTF_Quit();
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
