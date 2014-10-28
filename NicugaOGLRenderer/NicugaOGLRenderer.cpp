#include "Renderer.h"

int main(int argc, char *argv[])
{
	Renderer renderer = Renderer();
	renderer.init();
	renderer.loop();
    return 0;
}