#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL_image.h>

#include "include/sdl_api.h"
#include "include/colors.h"


void sdl_image(struct netcdf_data *data)
{
	// Initialisation de la SDL
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("", 0, 0, data->X_AXIS, data->Y_AXIS, SDL_WINDOW_HIDDEN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Initialisation de la TTF
	TTF_Init();
	TTF_Font *jetbrains = TTF_OpenFont("nevada_data/fonts/JetBrainsMono-Bold.ttf", 40);
	
	SDL_Surface *surface_text = NULL;
	SDL_Texture *texture_text = NULL;
	char ordinate[15], time[15], scale[15];

	// Initialisation du rendu
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	// Traitement des données
	if (!data->filter) sdl_get_limits(data);

	// Dessin du rendu
	for (int x = 0; x < data->X_AXIS; x ++)
	{
		for (int y = 0; y < data->Y_AXIS; y ++)
		{
			if (data->filter)
			{
				if (data->var[data->Y_AXIS * x + y] < data->minimum)
					data->var[data->Y_AXIS * x + y] = data->minimum;
				if (data->var[data->Y_AXIS * x + y] > data->maximum)
					data->var[data->Y_AXIS * x + y] = data->maximum;
			}
			// Affichage des données
			int color_index = 1019 - floor(1019 * (sdl_invert_sign(data->minimum) + data->var[data->Y_AXIS * x + y]) / (sdl_invert_sign(data->minimum) + data->maximum));
			SDL_SetRenderDrawColor(renderer, COLORS[color_index][0], COLORS[color_index][1], COLORS[color_index][2], 255);
			SDL_RenderDrawPoint(renderer, x, (data->Y_AXIS - y));
		}
	}

	// Échelles et grille
	for (int x = 0; x < data->X_AXIS; x ++)
	{
		for (int y = 0; y < data->Y_AXIS; y ++)
		{
			// Échelle des ordonnées
			if (!(y % 50) && y && y < data->Y_AXIS - 50 && x == data->X_AXIS / 2)
			{
				
				int label = floor(data->y_labels[y]);
				sprintf(ordinate, "%d", label);
				sdl_render_text(renderer, jetbrains, 2, data->Y_AXIS - (y + 52), ordinate, true);
			}

			// Grille
			if (!(y % 50) && y && y < data->Y_AXIS - 50 && !(x % 240) && x)
			{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
				SDL_Rect rect = {x - 5, y - 5, 10, 10};
				SDL_RenderFillRect(renderer, &rect);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
				SDL_RenderDrawRect(renderer, &rect);
			}
		}
		
		// Échelle des temps
		if (!(x % 240) && x)
		{
			sdl_convert_epoch((time_t) data->x_labels[x], "%H:%M", time);
			sdl_render_text(renderer, jetbrains, x - 60, data->Y_AXIS - 55, time, true);
		}
	}

	// Dessin du nuancier de couleur
	for (int x = 0; x < 2040; x ++)
	{
		int color_index = 1019 - floor(x / 2);
		SDL_SetRenderDrawColor(renderer, COLORS[color_index][0], COLORS[color_index][1], COLORS[color_index][2], 255);
		
		for (int y = 10; y < 140; y ++)
			SDL_RenderDrawPoint(renderer, data->X_AXIS - 2050 + x, y);
	}
	for (int color_index = 0; color_index <= 1020; color_index ++)
	{
		if (!(color_index % 255))
		{
			float scale_value = data->minimum + ((color_index * (data->maximum - data->minimum)) / 1020);
			sprintf(scale, "% 1.2e", scale_value);
			sdl_render_text(renderer, jetbrains, data->X_AXIS - 2040 + 1.75 * color_index, 50, scale, false);
		}
	}
	SDL_Rect rect = {data->X_AXIS - 2050, 10, 2040, 130};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &rect);

	// Affichage des métadonnées
	char metadata[50];
	sprintf(metadata, "%s %s", data->varname, data->date);
	SDL_Color color = {0, 0, 0};
	surface_text = TTF_RenderText_Solid(jetbrains, metadata, color);
	texture_text = SDL_CreateTextureFromSurface(renderer, surface_text);
	int text_width = 0, text_height = 0;
	SDL_QueryTexture(texture_text, NULL, NULL, &text_width, &text_height);
	SDL_Rect textrect = {data->X_AXIS - 2060 - text_width, 10, text_width, text_height};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
	SDL_RenderFillRect(renderer, &textrect);
	SDL_RenderCopy(renderer, texture_text, NULL, &textrect);


	// Exportation au format PNG
	char filename[100];
	sprintf(filename, "/home/%s/%s_%s.png", getenv("USER"), data->varname, data->date);
	sdl_save_renderer(filename, renderer, data->X_AXIS, data->Y_AXIS);

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_FreeSurface(surface_text);
	SDL_DestroyTexture(texture_text);

	SDL_Quit();
	TTF_CloseFont(jetbrains);
	TTF_Quit();
}


void sdl_measure(struct netcdf_data *data)
{
	// Initialisation de la SDL
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);

	int x_factor = ceil((float) data->X_AXIS / 1920.), y_factor = ceil((float) data->Y_AXIS / 1080.);
	if (x_factor <= 1)
		x_factor = 1;
	if (y_factor <= 1)
		y_factor = 1;

	window = SDL_CreateWindow("", 0, 0, data->X_AXIS / x_factor, data->Y_AXIS / y_factor, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Initialisation du rendu
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	// Traitement des données
	if (!data->filter) sdl_get_limits(data);
	
	// Dessin du rendu
	for (int x = 0; x < data->X_AXIS / x_factor; x ++)
	{
		for (int y = 0; y < data->Y_AXIS / y_factor; y ++)
		{		
			if (data->filter)
			{
				if (data->var[data->Y_AXIS * (x * x_factor) + (y * y_factor)] < data->minimum)
					data->var[data->Y_AXIS * (x * x_factor) + (y * y_factor)] = data->minimum;
				if (data->var[data->Y_AXIS * (x * x_factor) + (y * y_factor)] > data->maximum)
					data->var[data->Y_AXIS * (x * x_factor) + (y * y_factor)] = data->maximum;
			}

			// Affichage des données
			int color_index = 1019 - floor(1019 * (sdl_invert_sign(data->minimum) + data->var[data->Y_AXIS * (x * x_factor) + (y * y_factor)]) / (sdl_invert_sign(data->minimum) + data->maximum));
			SDL_SetRenderDrawColor(renderer, COLORS[color_index][0], COLORS[color_index][1], COLORS[color_index][2], 255);
			
			SDL_RenderDrawPoint(renderer, x, (data->Y_AXIS / y_factor - y));
		}
	}
	SDL_RenderPresent(renderer);

	char metadata[50];
	sprintf(metadata, "%s %s (mesures)", data->varname, data->date);

	char filename[100];
	sprintf(filename, "/home/%s/%s_%s.csv", getenv("USER"), data->varname, data->date);

	sdl_loop(renderer, filename, data);

	// Initialisation de la TTF
	TTF_Init();
	TTF_Font *jetbrains = TTF_OpenFont("nevada_data/fonts/JetBrainsMono-Bold.ttf", 20);
	sdl_render_text(renderer, jetbrains, 2, 2, metadata, true);

	sprintf(filename, "/home/%s/%s_mesures_%s.png", getenv("USER"), data->varname, data->date);
	sdl_save_renderer(filename, renderer, data->X_AXIS / x_factor, data->Y_AXIS / y_factor);

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	TTF_CloseFont(jetbrains);
	TTF_Quit();
}


void sdl_get_limits(struct netcdf_data *data)
{	
	for (int x = 0; x < data->X_AXIS; x ++)
	{
		for (int y = 0; y < data->Y_AXIS; y ++)
		{
			if (data->var[data->Y_AXIS * x + y] < data->minimum)
				data->minimum = data->var[data->Y_AXIS * x + y];
			if (data->var[data->Y_AXIS * x + y] > data->maximum)
				data->maximum = data->var[data->Y_AXIS * x + y];
		}
	}
}


float sdl_invert_sign(float a)
{
	return -a;
}


void sdl_loop(SDL_Renderer *renderer, const char *filename, struct netcdf_data *data)
{
	uint8_t exit = 0;
	int x, y, index = 1;
	char csv_row[100], coord_index[100], hour[5], min[5];
	int x_factor = ceil((float) data->X_AXIS / 1920.), y_factor = ceil((float) data->Y_AXIS / 1080.);	
	if (x_factor <= 1)
		x_factor = 1;
	if (y_factor <= 1)
		y_factor = 1;

	SDL_Event event;
	TTF_Init();
	TTF_Font *jetbrains = TTF_OpenFont("nevada_data/fonts/JetBrainsMono-Bold.ttf", 20);

	FILE *file = NULL;
	file = fopen(filename, "w");
	sprintf(csv_row, "identifiant; %s; %s; %s\n", data->varname, data->x_name, data->y_name);
	fputs(csv_row, file);

	while (event.type != SDL_QUIT)
	{
		SDL_PollEvent(&event);

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				x = event.button.x;
				y = event.button.y;

				float targeted_data = data->var[data->Y_AXIS * x * x_factor + data->Y_AXIS - y * y_factor];
				int targeted_x = floor(data->x_labels[x * x_factor]);
				int targeted_y = floor(data->y_labels[data->Y_AXIS - y_factor * y]);

				char date[255];
				sdl_convert_epoch((time_t) targeted_x, "%H:%M", date);

				SDL_Rect x_rect = {x - 5, y - 1, 10, 3};
				SDL_Rect y_rect = {x - 1, y - 5, 3, 10};
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderFillRect(renderer, &x_rect);
				SDL_RenderFillRect(renderer, &y_rect);

				sprintf(coord_index, "%d", index);
				sdl_render_text(renderer, jetbrains, x + 2, y + 2, coord_index, true);


				sprintf(csv_row, "%d; %f; %s; %d;\n", index, targeted_data, date, targeted_y);

				SDL_RenderPresent(renderer);				
				fputs(csv_row, file);
				index ++;

				break;
		}
	}
	fclose(file);
	TTF_CloseFont(jetbrains);
	TTF_Quit();
}


void sdl_render_text(SDL_Renderer *renderer, TTF_Font *font, const int x, const int y, const char *text, const bool alpha_bg)
{
	SDL_Color color = {0, 0, 0};
	SDL_Surface *surface_text;
	SDL_Texture *texture_text;

	surface_text = TTF_RenderText_Solid(font, text, color);
	texture_text = SDL_CreateTextureFromSurface(renderer, surface_text);
	
	int text_width = 0, text_height = 0;
	SDL_QueryTexture(texture_text, NULL, NULL, &text_width, &text_height);

	SDL_Rect textrect = {x, y, text_width, text_height};
	if (alpha_bg)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 80);
		SDL_RenderFillRect(renderer, &textrect);
	}

	SDL_RenderCopy(renderer, texture_text, NULL, &textrect);
	SDL_FreeSurface(surface_text);
	SDL_DestroyTexture(texture_text);
}


void sdl_save_renderer(const char *filename, SDL_Renderer *renderer, const int width, const int height)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, filename);
    
    SDL_FreeSurface(surface);
}


void sdl_convert_epoch(const time_t epoch, const char *format, char *date)
{
	struct tm tm;	
	tm = *gmtime(&epoch);
	strftime(date, sizeof(date), format, &tm);
}