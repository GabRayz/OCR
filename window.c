#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include "window.h"
#include "image.h"
#include "neuralnetwork.h"
#include "main.h"

void open_file_browser()
{
	//xdg-open /path	
}

int count_saved_files()
{
	int count = 0;
	DIR *src = opendir("res");
	struct dirent *file;
	while ((file = readdir(src)) != NULL)
    {
		count++;
	}
	return count;
}
void save_result(char *res, char *filepath)
{
    FILE *file = fopen("res/res.txt", "w");
    if (file == NULL)
    {
        printf("Failed to save the result\n");
        return;
    }
    fprintf(file, "%s", res);
    fclose(file);
    printf("Result saved at: %s\n", filepath);
}


char *call_nn(Img *img)
{
	LinkedList *chars = segmentation(img, true);
	NeuralNetwork *nn = nn_load("save/all");
	char* res = send_to_cerveau(img,chars,nn);
	return res;
}

void init_window(char *filepath)
{

	//Create Window
	SDL_Window *screen = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	

	int right = 563;
	int left = 702;
	int height = 795;
	int width = right + left;
	int margin = 10;

	screen = SDL_CreateWindow("OCR - Les Croisillons", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (screen == NULL)
	{
		SDL_Log("Could not create window: %s", SDL_GetError());
		SDL_Quit();
	}

	SDL_Surface *pScreen = SDL_GetWindowSurface(screen);

	Uint32 color = SDL_MapRGB(pScreen->format, 75, 75, 75);

	SDL_FillRect(pScreen, NULL, color);

	//Create Gradien from img
	SDL_Surface *gradient;
	gradient = IMG_Load("Img/Gradient3.png");
	SDL_Rect gradient_pos;
	gradient_pos.x = 0;
	gradient_pos.y = 0;
	SDL_BlitSurface(gradient, NULL, pScreen, &gradient_pos);

	//Create Drap&Drop side from img
	SDL_Surface *dragdrop;
	dragdrop = IMG_Load("Img/DragDrop3.png");
	SDL_Rect dragdrop_pos;
	dragdrop_pos.x = 563;
	dragdrop_pos.y = 0;
	SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);

	//Create rght area side from img
	SDL_Surface *right_area;
	right_area = IMG_Load("Img/RightArea.png");
	SDL_Rect right_area_pos;
	right_area_pos.x = 563;
	right_area_pos.y = 0;

	//Create GroupName from img
	SDL_Surface *group_name;
	group_name = IMG_Load("Img/GroupName3.png");
	SDL_Rect group_name_pos;
	group_name_pos.x = 120;
	group_name_pos.y = 35;
	SDL_BlitSurface(group_name, NULL, pScreen, &group_name_pos);

	//Create Logo
	SDL_Surface *logo;
	logo = IMG_Load("Img/Logo3.png");
	SDL_Rect logo_pos;
	logo_pos.x = 200;
	logo_pos.y = 150;
	SDL_BlitSurface(logo, NULL, pScreen, &logo_pos);

	//Set buttons height and width
	int btn_height = 30;
	int btn_width = 300; //160;

	//Create font
	TTF_Font *font;
	font = TTF_OpenFont("./Font/Raleway-Regular.ttf", 24);
	SDL_Color font_color = {255, 255, 255, 0};
	SDL_Color black = {0, 0, 0, 0};

	//Create Text area
	int menu_x = 130; //200;
	int menu_y = 400;
	int menu_step = 60;

	SDL_Surface *add;
	add = TTF_RenderUTF8_Blended(font, "ADD FILES", font_color);
	SDL_Rect add_pos;
	add_pos.x = menu_x;
	add_pos.y = menu_y;
	SDL_BlitSurface(add, NULL, pScreen, &add_pos);

	SDL_Surface *saved;
	saved = TTF_RenderUTF8_Blended(font, "SAVED FILES", font_color);
	SDL_Rect saved_pos;
	saved_pos.x = menu_x;
	saved_pos.y = menu_y + menu_step;
	SDL_BlitSurface(saved, NULL, pScreen, &saved_pos);

	SDL_Surface *settings;
	settings = TTF_RenderUTF8_Blended(font, "SETTINGS", font_color);
	SDL_Rect settings_pos;
	settings_pos.x = menu_x;
	settings_pos.y = menu_y + menu_step * 2;
	SDL_BlitSurface(settings, NULL, pScreen, &settings_pos);

	SDL_Surface *quit;
	quit = TTF_RenderUTF8_Blended(font, "CHARACTERS DETECTION", font_color);
	SDL_Rect quit_pos;
	quit_pos.x = menu_x;
	quit_pos.y = menu_y + menu_step * 3;
	SDL_BlitSurface(quit, NULL, pScreen, &quit_pos);

	SDL_Surface *process;
	process = TTF_RenderUTF8_Blended(font, "RECOGNITION", font_color);
	SDL_Rect process_pos;
	process_pos.x = menu_x;
	process_pos.y = menu_y + menu_step * 4;
	SDL_BlitSurface(process, NULL, pScreen, &process_pos);

	SDL_Surface *digitalize;
	digitalize = TTF_RenderUTF8_Blended(font, "DIGITALIZE!", font_color);
	SDL_Rect digitalize_pos;
	digitalize_pos.x = 220;
	digitalize_pos.y = 710;

	SDL_Surface *save_btn;
	save_btn = TTF_RenderUTF8_Blended(font, "SAVE", font_color);
	SDL_Rect save_btn_pos;
	save_btn_pos.x = 870;
	save_btn_pos.y = 710;

	SDL_Surface *save_at_btn;
	save_at_btn = TTF_RenderUTF8_Blended(font, "SAVED AT RES/", font_color);
	SDL_Rect save_at_btn_pos;
	save_at_btn_pos.x = 820;
	save_at_btn_pos.y = 710;


	//Process button
	SDL_Surface *digitalize_btn;
	digitalize_btn = IMG_Load("Img/Btn.png");
	SDL_Rect digitalize_btn_pos;
	digitalize_btn_pos.x = 100;
	digitalize_btn_pos.y = 650;
	int display_btn = 0;

	//Save buton
	SDL_Rect saved_btn_pos;
	saved_btn_pos.x = 720;
	saved_btn_pos.y = 650;
	int display_btn2 = 0;

	//Load neural network & run 
	SDL_Surface *image;
	image = IMG_Load(filepath);
	// Img *img = img_import(filepath);
    // img_save(img, "res/bw.png");
    // LinkedList *chars = segmentation(img, true);
	// NeuralNetwork *nn = nn_load("save/all");
	// char* res = send_to_cerveau(img,chars,nn);
	
	SDL_Surface *result;
	// result = TTF_RenderText_Blended_Wrapped(font, res, black,left-margin*2);
	SDL_Rect result_pos;
	result_pos.x = dragdrop_pos.x+margin;
	result_pos.y = dragdrop_pos.y+margin;
	// SDL_BlitSurface(result, NULL, pScreen, &result_pos);
	
	SDL_Rect display_img_pos;
	display_img_pos.x = 0;
	display_img_pos.y = 0;
	display_img_pos.w = right;
	display_img_pos.h = height;
	display_img_pos.y = 100;
	// display_img_pos.h = left * h / w;
	// display_img_pos.y = (height / 2) - (h / 4);
	// if(image != NULL)
	// {
	// 	SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);
	// }
	
	//Refresh the screen
	SDL_UpdateWindowSurface(screen);

	//Cursor
	SDL_Cursor *cursor = SDL_GetCursor();

	//Event Loop
	int run = 1;
	SDL_Event event;

	//Enable drop file
	char *dropped_filedir;
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	int file_dropped = 0;
	Img *img;
	char *res;
	char *nb_file = "0";
	while (run)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			run = 0;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE: // Esc to quit the program
				run = 0;
				break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			// If clicked on ADD FILE
			if (isClicked(event.button, add_pos, btn_height, btn_width))
			{
				// xdg-open /
				open_file_browser();
			}
			// If clicked on paragraphs
			else if (isClicked(event.button, saved_pos, btn_height, btn_width))
			{
				SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);
				SDL_Surface* paragraph = IMG_Load("res/paragraph.png");
				SDL_BlitSurface(paragraph, NULL, pScreen, &display_img_pos);
				SDL_FreeSurface(paragraph);
			}
			// If clicked on lines
			else if (isClicked(event.button, settings_pos, btn_height, btn_width))
			{
				SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);
				SDL_Surface* line = IMG_Load("res/line.png");
				SDL_BlitSurface(line, NULL, pScreen, &display_img_pos);
				SDL_FreeSurface(line);
			}
			// If clicked on characters
			else if (isClicked(event.button, quit_pos, btn_height, btn_width))
			{
				SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);
				SDL_Surface* character = IMG_Load("res/character.png");
				SDL_BlitSurface(character, NULL, pScreen, &display_img_pos);
				SDL_FreeSurface(character);
			}
			// If clicked on recognize
			else if (isClicked(event.button, process_pos, btn_height, btn_width))
			{
				// SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);
				// result_pos.w = 300;
				// SDL_BlitSurface(result, NULL, pScreen, &result_pos);
			}
			// If clicked on DIGITALIZE
			else if (isClicked(event.button, digitalize_btn_pos, 184, 368) & display_btn)
			{
				res = call_nn(img);
				SDL_BlitSurface(right_area, NULL, pScreen, &right_area_pos);
				result = TTF_RenderText_Blended_Wrapped(font, res, black,left-margin*2);
				SDL_BlitSurface(result, NULL, pScreen, &result_pos);

				display_btn2 = 1;
				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &saved_btn_pos);
				SDL_BlitSurface(save_btn,NULL,pScreen, &save_btn_pos);
			}
			// If clicked on SAVE
			else if (isClicked(event.button, saved_btn_pos, 184, 368) & display_btn2)
			{
				char *buffer[4];
				nb_file = SDL_itoa(count_saved_files(),buffer,2);
				// char *c = concat("res/", nb_file);
				// printf("%s",c);
				//save_result(res,c);

				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &saved_btn_pos);

				SDL_BlitSurface(save_at_btn,NULL,pScreen, &save_at_btn_pos);
				display_btn2 = 0;
			}
			break;
		case (SDL_DROPFILE):
		{
			// In case if dropped file
			dropped_filedir = event.drop.file;

			printf("%s\n", dropped_filedir);
			img = img_import(dropped_filedir);

			if (img)
			{
				SDL_BlitSurface(gradient, NULL, pScreen, &gradient_pos);
				SDL_BlitSurface(group_name, NULL, pScreen, &group_name_pos);

				//Resize and Display image to the left
				image = IMG_Load(dropped_filedir);
				float w = image->w;
				float h = image->h;
				display_img_pos.h = left * h / w;
				SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);
				SDL_FreeSurface(image);

				//Blit process_btn
				display_btn = 1;
				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &digitalize_btn_pos);
				SDL_BlitSurface(digitalize,NULL,pScreen, &digitalize_pos);

				//Refresh the screen
				SDL_UpdateWindowSurface(screen);
				file_dropped = 1;
			}

			// Free dropped_filedir memory
			SDL_free(dropped_filedir);
			break;
		}

		case (SDL_MOUSEWHEEL):
		{
			if (file_dropped)
			{
				if (event.wheel.y > 0) // scroll up
				{
					//SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);
				}
				else if (event.wheel.y < 0) // scroll down
				{
					// Put code for handling "scroll down" here!
				}
			}
			break;
		}

		case (SDL_MOUSEMOTION):
		{
			if(isOver(event.motion, digitalize_btn_pos, 184, 368) & display_btn)
			{
				//change cursor to the clickable one
			}
			break;
		}
		}
		//Refresh the screen
		SDL_UpdateWindowSurface(screen);
	}
	//Free everything
	SDL_FreeSurface(gradient);
	SDL_FreeSurface(dragdrop);
	SDL_FreeSurface(group_name);
	SDL_FreeSurface(logo);
	SDL_FreeSurface(add);
	SDL_FreeSurface(saved);
	SDL_FreeSurface(settings);
	SDL_FreeSurface(quit);
	SDL_FreeSurface(process);
	SDL_FreeSurface(digitalize_btn);
	SDL_FreeSurface(image);
	SDL_FreeSurface(right_area);
	//SDL_FreeSurface(result);

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}

int isClicked(SDL_MouseButtonEvent mouse, SDL_Rect btn, int btn_height, int btn_width)
{
	return mouse.y > btn.y && mouse.y <= btn.y + btn_height && mouse.x > btn.x && mouse.x <= btn.x + btn_width;
}

int isOver(SDL_MouseMotionEvent mouse, SDL_Rect btn, int btn_height, int btn_width)
{
	return mouse.y > btn.y && mouse.y <= btn.y + btn_height && mouse.x > btn.x && mouse.x <= btn.x + btn_width;
}
