#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include "window.h"
#include "image.h"
#include "neuralnetwork.h"
#include "main.h"
#include "spellchek.h"
#include "hough.h"
#include "ccl.h"


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
    FILE *file = fopen(filepath, "w");
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
	printf("call_nn: %s\n", img->filepath);
	//char **argv = malloc(sizeof(char) * 4);
	//argv[0] = "";
	//argv[1] = "";
	//argv[2] = "save/all";
	//argv[3] = img->filepath;
	//printf("Calling CCL\n");
	// char *argv = {"./ocr", "ccl", "save/nn20", img->filepath};
	//char *res = ccl(4, argv);
	//printf("Result get\n");
	Img *rotated = hough(img);
	img_save(rotated, "res/rotated.png");
	Img *new = img_import("res/rotated.png");


	LinkedList *chars = ccl_segmentation(new, true);
	NeuralNetwork *nn = nn_load("save/all");
	char* res = send_images_to_cerveau(chars,nn);
	res = spellcheck(res);
	// free(argv);
	return res;
}

int isClicked(SDL_MouseButtonEvent mouse, SDL_Rect btn, int btn_height, int btn_width)
{
	return mouse.y > btn.y && mouse.y <= btn.y + btn_height && mouse.x > btn.x && mouse.x <= btn.x + btn_width;
}

int isOver(SDL_Rect btn, int btn_height, int btn_width)
{
	int x;
	int y;
	SDL_GetMouseState(&x, &y);
	return y > btn.y && y <= btn.y + btn_height && x > btn.x && x <= btn.x + btn_width;
}

void init_window()
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

	//Create font
	TTF_Font *font;
	font = TTF_OpenFont("./Font/Raleway-Regular.ttf", 24);
	SDL_Color font_color = {255, 255, 255, 0};
	SDL_Color black = {0, 0, 0, 0};

	//Create Text area
	// int menu_x = 130; //200;
	int menu_y = 400;
	int menu_step = 60;


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

	//Return button
	SDL_Surface *return_btn;
	return_btn = IMG_Load("Img/Return3.png");
	SDL_Rect return_btn_pos;
	return_btn_pos.x = 0;
	return_btn_pos.y = 661;
	
	//Quit button
	SDL_Surface *quit_btn;
	quit_btn = IMG_Load("Img/Quit3.png");
	SDL_Rect quit_btn_pos;
	quit_btn_pos.x = 400;
	quit_btn_pos.y = 661;
	SDL_Rect quit_pos;
	quit_pos.x = 190;
	quit_pos.y = menu_y + menu_step * 4;
	SDL_BlitSurface(quit_btn, NULL, pScreen, &quit_pos);
	

	//Save buton
	SDL_Rect saved_btn_pos;
	saved_btn_pos.x = 720;
	saved_btn_pos.y = 650;

	// imported image 
	SDL_Surface *image = NULL;
	
	SDL_Surface *result;
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
	
	//Refresh the screen
	SDL_UpdateWindowSurface(screen);

	//Event Loop
	int run = 1;
	SDL_Event event;

	//Enable drop file
	char *dropped_filedir;
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	Img *img = NULL;
	char *res = NULL;
	int nb_file;
	int right_y = 1;
	int state = 0;
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
			// If clicked on QUIT
			if ((isClicked(event.button, quit_pos, 140, 140) & (state == 0))
					|| (isClicked(event.button, quit_btn_pos, 140, 140) & (state == 1 || state == 2)))
			{
				run = 0;
			}
			// If clicked on RETURN
			else if (isClicked(event.button, return_btn_pos, 140,140) & (state == 1|| state == 2))
			{
				state = 0;
				SDL_BlitSurface(gradient, NULL, pScreen, &gradient_pos);
				SDL_BlitSurface(dragdrop, NULL, pScreen, &dragdrop_pos);
				SDL_BlitSurface(group_name, NULL, pScreen, &group_name_pos);
				SDL_BlitSurface(logo, NULL, pScreen, &logo_pos);
				SDL_BlitSurface(quit_btn, NULL, pScreen, &quit_pos);

			}
			// If clicked on DIGITALIZE
			else if (isClicked(event.button, digitalize_btn_pos, 184, 368) & (state == 1))
			{
				res = call_nn(img);
				if(res != NULL)
				{
				SDL_BlitSurface(right_area, NULL, pScreen, &right_area_pos);
				result = TTF_RenderText_Blended_Wrapped(font, res, black,left-margin*2);
				SDL_BlitSurface(result, NULL, pScreen, &result_pos);

				state = 2;
				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &saved_btn_pos);
				SDL_BlitSurface(save_btn,NULL,pScreen, &save_btn_pos);
				}
			}
			// If clicked on SAVE
			else if (isClicked(event.button, saved_btn_pos, 184, 368) & (state == 2))
			{
				char buffer[11];
				nb_file = count_saved_files()-2;
				sprintf(buffer, "%d", nb_file);
				char *c = malloc(sizeof(char) * 16);

				strcpy(c, "res/save");
				strcat(c, buffer);
				strcat(c, ".txt");
				
				save_result(res,c);

				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &saved_btn_pos);

				SDL_BlitSurface(save_at_btn,NULL,pScreen, &save_at_btn_pos);
				state = 1;
			}
			
			break;
		case (SDL_DROPFILE):
		{
			// In case if dropped file
			dropped_filedir = event.drop.file;

			img = img_import(dropped_filedir);

			if (img)
			{
				SDL_BlitSurface(gradient, NULL, pScreen, &gradient_pos);
				SDL_BlitSurface(group_name, NULL, pScreen, &group_name_pos);

				//Resize and Display image to the left
				image = IMG_Load(dropped_filedir);
				float w = image->w;
				float h = image->h;
				display_img_pos.h = right * h / w;
				SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);

				//Blit new menu
				state = 1;
				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &digitalize_btn_pos);
				SDL_BlitSurface(digitalize,NULL,pScreen, &digitalize_pos);
				SDL_BlitSurface(return_btn,NULL,pScreen,&return_btn_pos);
				SDL_BlitSurface(quit_btn,NULL,pScreen,&quit_btn_pos);

				//Refresh the screen
				SDL_UpdateWindowSurface(screen);
			}

			// Free dropped_filedir memory
			// SDL_free(dropped_filedir);
			break;
		}

		case (SDL_MOUSEWHEEL):
		{
			if ((state == 1 || state == 2) & isOver(dragdrop_pos,height,left))
			{
				SDL_BlitSurface(right_area, NULL, pScreen, &right_area_pos);
				if (event.wheel.y > 0) // scroll up
				{
					right_y ++;
					result_pos.y -= 30*right_y;
					SDL_BlitSurface(result, NULL, pScreen, &result_pos);
				}
				else if (event.wheel.y < 0) // scroll down
				{	
					if(result_pos.y- 30*right_y < 0)
						right_y --;
					result_pos.y -= 30*right_y;

					SDL_BlitSurface(result, NULL, pScreen, &result_pos);
				}
				SDL_BlitSurface(digitalize_btn,NULL,pScreen, &saved_btn_pos);
				SDL_BlitSurface(save_btn,NULL,pScreen, &save_btn_pos);
			}
			// else if((state == 1 || state == 2) & isOver(gradient_pos,height,right))
			// {
			// 	SDL_BlitSurface(gradient, NULL, pScreen, &gradient_pos);
			// 	SDL_BlitSurface(group_name, NULL, pScreen, &group_name_pos);
			// 	if (event.wheel.y > 0) // scroll up
			// 	{
			// 		display_img_pos.y -= 30;
			// 		SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);
			// 	}
			// 	else if (event.wheel.y < 0) // scroll down
			// 	{	
			// 		if(display_img_pos.y- 30 < 0)
			// 			display_img_pos.y -= 30;

			// 		SDL_BlitScaled(image, NULL, pScreen, &display_img_pos);
			// 	}
			// 	SDL_BlitSurface(digitalize_btn,NULL,pScreen, &digitalize_btn_pos);
			// 	SDL_BlitSurface(digitalize,NULL,pScreen, &digitalize_pos);
			// 	SDL_BlitSurface(return_btn,NULL,pScreen,&return_btn_pos);
			// 	SDL_BlitSurface(quit_btn,NULL,pScreen,&quit_btn_pos);
			// }
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
	SDL_FreeSurface(digitalize_btn);
	SDL_FreeSurface(image);
	SDL_FreeSurface(right_area);

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}

