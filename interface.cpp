#include <stdarg.h>
#include <stdio.h>
#include "interface.h"

void pixel(SDL_Surface * screen, int x, int y, unsigned int color_t)
{
	if (x >= 0 && y >= 0 && x < screen->w && y < screen->h)
	{
		int bpp = screen->format->BytesPerPixel;
		Uint8 *p = (Uint8 *) screen->pixels + y * screen->pitch + x * bpp;
		switch (bpp)
		{
		case 1:
			*p = color_t;
			break;
		case 2:
			*(Uint16 *) p = color_t;
			break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (color_t >> 16) & 0xff;
				p[1] = (color_t >> 8) & 0xff;
				p[2] = color_t & 0xff;
			}
			else
			{
				p[0] = color_t & 0xff;
				p[1] = (color_t >> 8) & 0xff;
				p[2] = (color_t >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *) p = color_t;
			break;
		}
	}
}

SDL_Surface *Load_img(const char *filename)
{
	SDL_Surface *temp = NULL;
	temp = IMG_Load(filename);
	if (!temp)
	{
		fprintf(stdout, "error: %s\n", SDL_GetError());
		return NULL;
	}
	else
	{
		fprintf(stdout, "load img success from %s\n", filename);
	}
	temp = SDL_DisplayFormat(temp);
	return temp;
}

SDL_Surface *Cut_surface(SDL_Surface * img,  int depht, int x, int y, int w, int h)
{
	SDL_Surface *temp = SDL_CreateRGBSurfaceFrom(img->pixels,
												 w, h, depht, img->w*4,
img->format->Rmask, img->format->Gmask, img->format->Bmask, img->format->Amask);
	if (!temp)
	{
		fprintf(stdout, "error surface not create %s", SDL_GetError());
	}
	temp = SDL_DisplayFormat(temp);
	return temp;

}

int transparency(SDL_Surface * s, Uint8 r, Uint8 g, Uint8 b)
{
	int ret;
	int flags = SDL_SRCCOLORKEY | SDL_RLEACCEL;
	Uint32 color = SDL_MapRGB(s->format, r, g, b);
	ret = SDL_SetColorKey(s, flags, color);
	if (ret < 0)
	{
		fprintf(stdout, "error transparency no fount\n");
		return -1;
	}
	else
	{
		fprintf(stdout, "transparency apply success\n");
		return 0;
	}
}

void draw_s(SDL_Surface * screen, SDL_Surface * img, I32 x, I32 y)
{
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	SDL_BlitSurface(img, 0, screen, &pos);
}



// COSNTRUCTOR POR DEFECTO DA VALOR A ANCHO Y ALTO 8
void __attribute__ ((constructor)) SETFONTMODE(void)
{
	FONTMODE.ancho = 8;
	FONTMODE.alto = 8;
	return;
}


void caracter(SDL_Surface * screen, int x, int y, const char ascii, unsigned int color)
{
	// Los valores de la estructura FONTMODE definen el tamaÃ±o de las
	// letras.
	int X, Y, W, H;				// variables a usar
	for (Y = 0; Y < FONTMODE.ancho; ++Y)	// for para abrir ALTO
		for (X = 0; X < FONTMODE.alto; ++X)
		{						// for para abrir ANCHO
			W = (8 * X / FONTMODE.ancho);	// estiramiento calcula cuantos
			// pixeles deveran ser rellenados
			// a lo largo dependiendo el
			// tamaño
			H = (8 * Y / FONTMODE.alto);	// estiramiento calcula cuantos
			// pixeles deveran ser rellenados
			// a lo alto dependiendo el
			// tamaño 
			if ((font_data[H + (ascii) * 8] >> W) & 1)
			{
				pixel(screen, X + x, Y + y, color);
			}					// esto no lo entiendo bien
		}
	return;						// retornas el control
}

void fontsize(int w, int h)
{
	FONTMODE.ancho = w;
	FONTMODE.alto = h;
	return;
}

void print(SDL_Surface * screen, int x, int y, char *text, unsigned int color_t)
{
	int i;
	int longitud = strlen(text);	// es necesario conocer la longitud?
	for (i = 0; i < longitud; i++)
	{							// recorreremos la derecha
		caracter(screen, (x + i * (FONTMODE.ancho)), y, (*text++), color_t);
	}
}

void print_f(SDL_Surface * screen, int x, int y, const char *s, unsigned int color_t, ...)
{

	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	print(screen, x, y, buffer, color_t);
}

// ************ class CInterface*******************

CInterface::CInterface()
{
}

CInterface::~CInterface()
{
}

void CInterface::Init(I32 w, I32 h, I32 bpp, const char *mensage)
{
	this->w = w;
	this->h = h;
	this->bpp = bpp;
	this->mensage = mensage;

	// PS2 SUBSYSTEM 
#ifdef PS2

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) < 0)
	{
		fprintf(stdout, "error : %s \n", SDL_GetError());
		atexit(SDL_Quit);
		runing = false;
	}
	fprintf(stdout, "init SDL on PS2 succes\n");


	screen = SDL_SetVideoMode(w, h, bpp, SDL_SWSURFACE);
	if (screen)
	{
		fprintf(stdout, "video mode w %d x h %d  bpp %d\n", w, h, bpp);
		runing = true;
	}

	SDL_ShowCursor(0);

#else /**/
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stdout, "error : %s \n", SDL_GetError());
		atexit(SDL_Quit);
		runing = false;
	}

	screen = SDL_SetVideoMode(w, h, bpp, SDL_HWSURFACE);
	if (screen)
	{
		fprintf(stdout, "video mode w %d x h %d  bpp %d\n", w, h, bpp);
		runing = true;
	}

#endif

	if (runing)
	{
		fprintf(stdout, "%s\n", mensage);
	}
}

void CInterface::fps_sincronizar(I32 fps)
{
	this->fps = fps;
	int desiredDelta = 1000 / fps;
	int starttick = SDL_GetTicks();
	int delta = SDL_GetTicks() - starttick;
	if (delta < desiredDelta)
	{
		SDL_Delay(desiredDelta - delta);
	}
}

void CInterface::fill_rect(I32 x, I32 y, I32 w, I32 h, unsigned int color_t)
{
	I32 i, j;
	for (i = 0; i < w; i++)
	{
		for (j = 0; j < h; j++)
		{
			pixel(screen, x + i, y + j, color_t);
		}
	}

}