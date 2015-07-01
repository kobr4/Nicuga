#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Box.H>
#include <stdio.h>
#include "TextureGenerator.h"

Fl_Window * g_window = NULL;
Fl_RGB_Image * g_image = NULL;
Fl_Box *g_box = NULL; 
Fl_Button * g_generateButton = NULL;
unsigned char * g_buffer_image = NULL;
unsigned int width = 600;
unsigned int height = 400;

unsigned int * g_buffer_work;


void put_sprite(unsigned int * src,int w,int h, unsigned char * dest, int dw,int dh,int x = 0,int y = 0) {
	for (int i = 0;i < w;i++) 
		for(int j = 0;j < h;j++) {
			g_buffer_image[(x + i + (y+j)*dw)*3] = (unsigned char)(g_buffer_work[i+j*w] & 0xff);
			g_buffer_image[(x + i + (y+j)*dw)*3 + 1] = (unsigned char)( (g_buffer_work[i+j*w] & 0xff00) >> 8);
			g_buffer_image[(x + i + (y+j)*dw)*3 + 2] = (unsigned char)( (g_buffer_work[i+j*w] & 0xff0000) >> 16);
		}
}


void start_generate(Fl_Widget*, void*) {
	g_buffer_image = (unsigned char*)calloc(width*height*3, sizeof(unsigned char));


	g_buffer_work = (unsigned int *)calloc(width * height,sizeof(unsigned int));

	TextureGenerator::generateShape(g_buffer_work,width,height,0xffffffff,40);
	put_sprite(g_buffer_work,width,height,g_buffer_image,width,height,0,0);


	g_box = new Fl_Box(0,40,width,height,"");
	g_image = new Fl_RGB_Image(g_buffer_image, width, height);
	g_box->image(g_image);
	g_window->add(g_box);
	g_window->redraw();
}

int main(int argc,char **argv) {
	Fl::lock();
	g_window = new Fl_Window(width,height+40,"Texture Generator 1.0");
	//init_main_panel();

	g_generateButton = new Fl_Button(10,5,100,25,"Generate !");
	g_generateButton->callback((Fl_Callback_p)&start_generate);
	g_window->add(g_generateButton);

	g_window->end();
	g_window->show(argc, argv);

	return Fl::run();
}