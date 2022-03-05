#include <SDL/SDL.h>
#include "interface.h"

#define w 320
#define h 240
#define bpp 24

int main(int argc, char **argv)
{
	CInterface *interface;
	interface = new CInterface;
	interface->Init(w,h,bpp, "test");
	SDL_Surface *img;
	img = Load_img("spr.bmp");
	//transparency(img, 255, 0, 255);
	int x = 0, y = 0, dx = 1, dy = 1;
	Uint32 color = RGB(255,0,0);
	Uint32 color2 = RGB(255,255,255);
	fontsize(8,8);
    SDL_Surface *test = Cut_surface(img,bpp,0,0, 32, 32);
    transparency(test,255,0,255);
	while (1)
	{

		draw_s(interface->screen, test, x, y);
		interface->fill_rect(x,y,10,10,color);
		print_f(interface->screen,0,0,"MOTOR GRAFICO PS2",color2);
		print_f(interface->screen,230,230,"RETROPANDA",color2);
		interface->render();
		interface->cls();
        interface->fps_sincronizar(120);
		x += dx;
		y += dy;
		if (x > 320)
		{
			dx = -1;
		}
		if (y > 240)
		{
			dy = -1;
		}
		if (x < 1)
		{
			dx = 1;
		}
		if (y < 1)
		{
			dy = 1;
		}
	}
	return 0;
}