// THIS CODE WAS MY OWN WORK , IT WAS WRITTEN WITHOUT CONSULTING ANY
// SOURCES OUTSIDE OF THOSE APPROVED BY THE INSTRUCTOR . Yuxuan Shi, yshi373, 2599711

#include "pbm.h"
#include<stdio.h>
#include<stdlib.h>

PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m )  
{
    PPMImage *new_ppm_img = malloc(sizeof(PPMImage));
    new_ppm_img->width=w;
    new_ppm_img->height=h;
    new_ppm_img->max=m;

    //initialize space for r,g,b channel
    for (int i = 0; i < 3; i++)
    {
      new_ppm_img->pixmap[i] = malloc(h*sizeof(unsigned int *));
    }

    for (int i = 0; i < h; i++) 
    {
      new_ppm_img->pixmap[0][i] = malloc(w*sizeof(unsigned int));
      new_ppm_img->pixmap[1][i] = malloc(w*sizeof(unsigned int));
      new_ppm_img->pixmap[2][i] = malloc(w*sizeof(unsigned int));
    }
  return new_ppm_img;
}

void del_ppmimage( PPMImage * p )  
{
  for (int i = 0; i < p->height; i++) //free each row of arrays
  {
    free(p->pixmap[0][i]);
    free(p->pixmap[1][i]);
    free(p->pixmap[2][i]);
  }

  for (int i = 0; i < 3; i++) //free arrays
  {
    free(p->pixmap[i]);
  }

  free(p);  //free img itself
}


PBMImage * new_pbmimage( unsigned int w, unsigned int h )
{
  PBMImage *new_pbm_img = malloc(sizeof(PBMImage));
  new_pbm_img->width=w;
  new_pbm_img->height=h;
  new_pbm_img->pixmap = malloc(h*sizeof(unsigned int *));

  for (int i = 0; i < h; i++)
  {
    new_pbm_img->pixmap[i] = malloc(w*sizeof(unsigned int));
  }
  return new_pbm_img;
}

void del_pbmimage( PBMImage * p )
{
  for (int i = 0; i < p->height; i++) //free each row of pixmap arr
  {
    free(p->pixmap[i]);
  }

  free(p->pixmap);  //free arr

  free(p);  //free img itself
}


PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m )
{
  PGMImage *new_pgm_img = malloc(sizeof(PGMImage));
  new_pgm_img->width = w;
  new_pgm_img->height = h;
  new_pgm_img->max = m;
  new_pgm_img->pixmap = malloc(h * sizeof(unsigned int *));

  for (int i = 0; i < h; i++)
  {
    new_pgm_img->pixmap[i] = malloc(w * sizeof(unsigned int));
  }
  return new_pgm_img;
}

void del_pgmimage( PGMImage * p )
{
  for (int i = 0; i < p->height; i++)
  {
    free(p->pixmap[i]);
  }
  free(p->pixmap);
  free(p);
}


