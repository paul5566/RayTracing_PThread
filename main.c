#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

//multi-thread sol
#include <pthread.h>

#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512

#define NUM_PTHREAD 8

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

	pthread_t pth_pool[ NUM_PTHREAD ];
	PTH_ARGS pth_args;
	PTH_NODE *pth_node[ NUM_PTHREAD ];
	int i;

	pth_args.pixels = pixels;
	pth_args.background_color = background;
	pth_args.rectangulars = rectangulars;
	pth_args.spheres = spheres;
	pth_args.lights = lights;
	pth_args.view = &view;
	pth_args.width = COLS;
	pth_args.height = ROWS;
	pth_args.pthNum = NUM_PTHREAD;

	for(i=0; i<NUM_PTHREAD; i++){
		pth_node[ i ] = ( PTH_NODE* )malloc( sizeof(PTH_NODE) );
		pth_node[ i ]->argPtr = &pth_args;
		pth_node[ i ]->init_height=i;
	}
	
    
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    /*raytracing(pixels, background,
               rectangulars, spheres, lights, &view, ROWS, COLS);*/
	for(i=0; i<NUM_PTHREAD; i++){
		if( (pthread_create( &pth_pool[ i ], NULL, raytracing, pth_node[ i ])) ){
			exit(1);
		}
	}
	
	void *ret;
	for(i=0; i<NUM_PTHREAD; i++){
		pthread_join( pth_pool[ i ], &ret);
	}
	
	printf("# Rendering scene\n");
    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
    free(pixels);
	
	for(i=0; i<NUM_PTHREAD; i++){
		free( pth_node[ i ] );
	}

    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    return 0;
}
