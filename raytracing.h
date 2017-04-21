#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>


/*
//original function
void raytracing(uint8_t *pixels, color background_color,
                rectangular_node rectangulars, sphere_node spheres,
                light_node lights, const viewpoint *view,
                int width, int height);
*/

//start_routine for pthread_create
void *raytracing( void* arg);


//
typedef struct __PTHREAD_ARGUMENT_STRUCTURE{
	uint8_t *pixels;
	double *background_color;
	rectangular_node rectangulars;
	sphere_node spheres;
	light_node lights;
	const viewpoint *view;
	int width;
	int height;
	int pthNum;
}PTH_ARGS;

typedef struct PTHREAD_NODE{
	PTH_ARGS* argPtr;
	int init_height;
}PTH_NODE;




#endif
