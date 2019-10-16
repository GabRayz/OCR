#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "window.h"

void init_window(){

	//Create Window
	SDL_Surface *screen = NULL;
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	
	int height = 530;
	int width = 843;
	int pixel = 32;

	screen = SDL_SetVideoMode(width,height,pixel,SDL_HWSURFACE);
	if(screen == NULL){
		//Error
	}

	SDL_WM_SetCaption("OCR - Les Croisillons", NULL);
	
	Uint32 color = SDL_MapRGB(screen->format, 75,75,75);

	SDL_FillRect(screen, NULL, color);

	//Create Gradien from img
	SDL_Surface *gradient;
	gradient = IMG_Load("Img/Gradient2.png");
	SDL_Rect gradient_pos;
	gradient_pos.x = 0;
	gradient_pos.y = 0;
	SDL_BlitSurface(gradient,NULL,screen, &gradient_pos);
	SDL_FreeSurface(gradient);

	//Create Drap&Drop side from img
	SDL_Surface *dragdrop;
	dragdrop = IMG_Load("Img/DragDrop2.png");
	SDL_Rect dragdrop_pos;
	dragdrop_pos.x = 375;
	dragdrop_pos.y = 0;
	SDL_BlitSurface(dragdrop,NULL,screen, &dragdrop_pos);
	SDL_FreeSurface(dragdrop);

	//Create GroupName from img
	SDL_Surface *group_name;
	group_name = IMG_Load("Img/GroupName2.png");
	SDL_Rect group_name_pos;
	group_name_pos.x = 80;
	group_name_pos.y = 30;
	SDL_BlitSurface(group_name,NULL,screen, &group_name_pos);
	SDL_FreeSurface(group_name);

	//Create Logo
	SDL_Surface *logo;
	logo = IMG_Load("Img/Logo2.png");
	SDL_Rect logo_pos;
	logo_pos.x = 135;
	logo_pos.y = 100;
	SDL_BlitSurface(logo,NULL,screen, &logo_pos);
	SDL_FreeSurface(logo);

	//Set buttons height and width
	int btn_height = 30;
	int btn_width = 160;

	//Create font
	TTF_Font *font;
	font = TTF_OpenFont("Font/Raleway-Medium.ttf",20);
	SDL_Color font_color = {255,255,255};

	//Create Text area
	int menu_x = 135;
	int menu_y = 260;
	int menu_step = 50;

	SDL_Surface *add;
	add = TTF_RenderText_Blended(font,"ADD FILE",font_color);
	SDL_Rect add_pos;
	add_pos.x = menu_x;
	add_pos.y = menu_y;
	SDL_BlitSurface(add,NULL,screen,&add_pos);
	SDL_FreeSurface(add);

	SDL_Surface *saved;
	saved = TTF_RenderText_Blended(font,"SAVED FILES",font_color);
	SDL_Rect saved_pos;
	saved_pos.x = menu_x;
	saved_pos.y = menu_y + menu_step;
	SDL_BlitSurface(saved,NULL,screen,&saved_pos);
	SDL_FreeSurface(saved);

	SDL_Surface *settings;
	settings = TTF_RenderText_Blended(font,"SETTINGS",font_color);
	SDL_Rect settings_pos;
	settings_pos.x = menu_x;
	settings_pos.y = menu_y + menu_step * 2;
	SDL_BlitSurface(settings,NULL,screen,&settings_pos);
	SDL_FreeSurface(settings);

	SDL_Surface *quit;
	quit = TTF_RenderText_Blended(font,"QUIT",font_color);
	SDL_Rect quit_pos;
	quit_pos.x = menu_x;
	quit_pos.y = menu_y + menu_step * 3;
	SDL_BlitSurface(quit,NULL,screen,&quit_pos);
	SDL_FreeSurface(quit);

	

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
			// If clicked on ADD FILE
			if(event.button.y > add_pos.y 
			   && event.button.y <= add_pos.y+btn_height
			   && event.button.x > add_pos.x 
			   && event.button.x <= add_pos.x+btn_width)
			   {
				   continuer = 0;
			   }
			// If clicked on SAVED FILES
			else if(event.button.y > saved_pos.y 
			   && event.button.y <= saved_pos.y+btn_height
			   && event.button.x > saved_pos.x 
			   && event.button.x <= saved_pos.x+btn_width)
			   {
				   continuer = 0;
			   }
			// If clicked on SETTINGS
			else if(event.button.y > settings_pos.y 
			   && event.button.y <= settings_pos.y+btn_height
			   && event.button.x > settings_pos.x 
			   && event.button.x <= settings_pos.x+btn_width)
			   {
				   continuer = 0;
			   }
			// If clicked on QUIT
			else if(event.button.y > quit_pos.y 
			   && event.button.y <= quit_pos.y+btn_height
			   && event.button.x > quit_pos.x 
			   && event.button.x <= quit_pos.x+btn_width)
			   {
				   continuer = 0;
			   }
			
        }
    }
	TTF_Quit();
	SDL_Quit();
}
