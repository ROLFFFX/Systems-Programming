// THIS CODE WAS MY OWN WORK , IT WAS WRITTEN WITHOUT CONSULTING ANY
// SOURCES OUTSIDE OF THOSE APPROVED BY THE INSTRUCTOR . Yuxuan Shi, yshi373, 2599711
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include<stdbool.h>
#include<string.h>

#include "pbm.h"

//option menu struct is used to organize getopt() options. option_menu will be used at intermediate stage
//between collecting user input and assigning following operations.
typedef struct {
    char opt[2];
    char i_filename[100];
    char o_filename[100];  
    int max_pixel_value;
    char color[20];
    int thumbnail_scale;
} option_menu ;

PPMImage *copy_to_PPM(char *i_filename, char *o_filename){  //helper function: returns a copy of the input ppm file.
    PPMImage *old_ppm = read_ppmfile(i_filename);
    int w = old_ppm->width;
    int h = old_ppm->height;
    int m = old_ppm->max;

    PPMImage *new_ppm = new_ppmimage(w, h, m);

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            new_ppm->pixmap[0][j][i] = old_ppm->pixmap[0][j][i];
            new_ppm->pixmap[1][j][i] = old_ppm->pixmap[1][j][i];
            new_ppm->pixmap[2][j][i] = old_ppm->pixmap[2][j][i];
        }
    }

    del_ppmimage(old_ppm);
    return new_ppm;
};

void convert_to_PBM(char *i_filename, char*o_filename){     //handles -b
    PPMImage *ppm_img = copy_to_PPM(i_filename, o_filename);  
    int w = ppm_img->width;
    int h = ppm_img->height;
    int m = ppm_img->max;    

    //initialize and populate pbm img.
    PBMImage *pbm_img = new_pbmimage(w, h); 
    for (int i = 0; i < w; i++) 
    {
        for (int j = 0; j < h; j++)
        {
            float sum = ppm_img->pixmap[0][j][i] 
            + ppm_img->pixmap[1][j][i] 
            + ppm_img->pixmap[2][j][i];
            float average = sum / 3;
            if (average >= (float)m / 2) {
                pbm_img->pixmap[j][i] = 0;
            } else {
                pbm_img->pixmap[j][i] = 1;
            }
        }
    }
    write_pbmfile(pbm_img, o_filename);
    del_pbmimage(pbm_img);
    del_ppmimage(ppm_img);
}

void *convert_to_PGM(option_menu options){  //handles -g
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;
    int PPM_max = new_ppm->max;

    PGMImage *new_pgm = new_pgmimage(w, h, options.max_pixel_value);

    for (int j = 0; j < w; j++) 
    {
        for (int i = 0; i < h; i++) 
        {
        float sum = new_ppm->pixmap[0][i][j] + new_ppm->pixmap[1][i][j] + new_ppm->pixmap[2][i][j];
        float average = sum / 3;
        new_pgm->pixmap[i][j] = average / (float)PPM_max * (float)(options.max_pixel_value);
        }
    }

    write_pgmfile(new_pgm, options.o_filename);
    del_pgmimage(new_pgm);
    del_ppmimage(new_ppm);    
}   

void isolate_RGB_channel(option_menu options){  //handles -i
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;
    char* color = options.color;

    if (strcmp(color, "red")==0){   //R G B: to isolate red, go into each group of pixel and set the second and third to 0.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[1][i][j] = 0;
                new_ppm->pixmap[2][i][j] = 0;
            }
        }
    } else if (strcmp(color, "green")==0){  //R G B: to isolate green, set the first and third to 0.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[0][i][j] = 0;
                new_ppm->pixmap[2][i][j] = 0;
            }
            }
    } else if (strcmp(color, "blue")==0){   //R G B: to isolate blue, set the first and second to 0.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[0][i][j] = 0;
                new_ppm->pixmap[1][i][j] = 0;
            }     
        }
    }
    
    write_ppmfile(new_ppm, options.o_filename);
    del_ppmimage(new_ppm);
}

void remove_RGB_channel(option_menu options){   //handles -r
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;
    char* color = options.color;

    if (strcmp(color, "red")==0){   //R G B: to remove red, go into the triplet and remove first.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[0][i][j] = 0;
            }
        }
    } else if (strcmp(color, "green")==0){  //R G B: to remove green, set the second to 0.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[1][i][j] = 0;
            }
            }
    } else if (strcmp(color, "blue")==0){   //R G B: to isolate blue, set the last to 0.
        for (int j = 0; j < w; j++) 
        {
            for (int i = 0; i < h; i++) 
            {
                new_ppm->pixmap[2][i][j] = 0;
            }     
        }
    }
    
    write_ppmfile(new_ppm, options.o_filename);
    del_ppmimage(new_ppm);
}

void sepia_transformation(option_menu options){ //handles -s
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;
    int m = new_ppm->max;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            //record old color composition
            float oldR = new_ppm->pixmap[0][j][i];
            float oldG = new_ppm->pixmap[1][j][i];
            float oldB = new_ppm->pixmap[2][j][i];

            //include validation in new channel initialization stage.
            float newR = 0.393*oldR + 0.769*oldG + 0.189*oldB;
            if (newR > m){newR=m;}
            float newG = 0.349*oldR + 0.686*oldG + 0.168*oldB;
            if (newG > m){newG=m;}
            float newB = 0.272*oldR + 0.534*oldG + 0.131*oldB;
            if (newB > m){newB=m;}

            //assign new color to each pixel.
            new_ppm->pixmap[0][j][i] = newR;
            new_ppm->pixmap[1][j][i] = newG;
            new_ppm->pixmap[2][j][i] = newB;
        }
    }
    write_ppmfile(new_ppm, options.o_filename);
    del_ppmimage(new_ppm);
}

void mirror(option_menu options){   //hanldes -m
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;

    //just copy left half to right half. copy starts at half of width so it handles odd number of columns.
    for (int i = 0; i < h; i++)
    {
        for (int j = w/2; j < w; j++)
        {
           new_ppm->pixmap[0][i][j] = new_ppm->pixmap[0][i][w-j-1];
           new_ppm->pixmap[1][i][j] = new_ppm->pixmap[1][i][w-j-1];
           new_ppm->pixmap[2][i][j] = new_ppm->pixmap[2][i][w-j-1];
        }
    }
    write_ppmfile(new_ppm, options.o_filename);
    del_ppmimage(new_ppm);
}

void thumbnail(option_menu options){    //handles -t
    PPMImage *new_ppm = copy_to_PPM(options.i_filename, options.o_filename);
    int w = new_ppm->width;
    int h = new_ppm->height;
    int m = new_ppm->max;

    //calculate dimension for returning ppm img
    int new_width = w/(options.thumbnail_scale);
    int new_height = h/(options.thumbnail_scale);

    PPMImage *return_ppm = new_ppmimage(new_width, new_height, m);

    // iterate over the return ppm and assign value according to the original ppm img. Te index for original ppm img jump 
    // for the scale of thumbnail scale for each iteration.
    for (int j = 0, w_original = 0; j < new_width; j++, w_original += options.thumbnail_scale)
    {
        for (int i = 0, h_original = 0; i < new_height; i++, h_original += options.thumbnail_scale)
        {
            return_ppm->pixmap[0][i][j] = new_ppm->pixmap[0][h_original][w_original];
            return_ppm->pixmap[1][i][j] = new_ppm->pixmap[1][h_original][w_original];
            return_ppm->pixmap[2][i][j] = new_ppm->pixmap[2][h_original][w_original];
        }
    }
    write_ppmfile(return_ppm, options.o_filename);
    del_ppmimage(return_ppm);
    del_ppmimage(new_ppm);
}

void thumbnail_tile(option_menu options){   //handles -n
    //validate the input scale is within 1 to 8
    if (options.thumbnail_scale < 1 || options.thumbnail_scale > 8){
        fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", options.thumbnail_scale);
        exit(1);
    }

    
    PPMImage *ppm_original = copy_to_PPM(options.i_filename, options.o_filename);
    PPMImage *ppm_returning = new_ppmimage(ppm_original->width, ppm_original->height, ppm_original->max);   //same size as original ppm

    //initialize empty template for ppm thumbnail
    int new_width = (ppm_original->width) / (options.thumbnail_scale);
    int new_height = (ppm_original->height) / (options.thumbnail_scale);
    PPMImage *ppm_thumbnail = new_ppmimage(new_width, new_height, ppm_original->max);
    //populate ppm thumbnail. same as -t.
    for (int j = 0, w_original = 0; j < new_width; j++, w_original += options.thumbnail_scale)
    {
        for (int i = 0, h_original = 0; i < new_height; i++, h_original += options.thumbnail_scale)
        {
            ppm_thumbnail->pixmap[0][i][j] = ppm_original->pixmap[0][h_original][w_original];
            ppm_thumbnail->pixmap[1][i][j] = ppm_original->pixmap[1][h_original][w_original];
            ppm_thumbnail->pixmap[2][i][j] = ppm_original->pixmap[2][h_original][w_original];
        }
    }  
    //populate returning ppm img with thumbnail.
    int thumbnail_width = 0;
    for (int j = 0; j < ppm_returning->width; j++) 
    {
        int thumbnail_height = 0;
        for (int i = 0; i < ppm_returning->height; i++) 
        {
        //keep populating returning ppm from start of thumbnail
        if (thumbnail_width == new_width) { 
            thumbnail_width = 0;
        }
        if (thumbnail_height == new_height) {
            thumbnail_height = 0;
        }
        ppm_returning->pixmap[0][i][j] = ppm_thumbnail->pixmap[0][thumbnail_height][thumbnail_width];
        ppm_returning->pixmap[1][i][j] = ppm_thumbnail->pixmap[1][thumbnail_height][thumbnail_width];
        ppm_returning->pixmap[2][i][j] = ppm_thumbnail->pixmap[2][thumbnail_height][thumbnail_width];
        thumbnail_height++;
        }
        thumbnail_width++;
    }
    write_ppmfile(ppm_returning, options.o_filename);
    del_ppmimage(ppm_returning);
    del_ppmimage(ppm_original);
    del_ppmimage(ppm_thumbnail);
}

int main( int argc, char *argv[] )
{
    int option;
    int transformation_counter = 0;
    bool used_dash_o = false;

    option_menu options;

    while ((option = getopt(argc, argv, ":bg:i:r:smt:n:o:")) != -1){
        switch(option)
        {
        case 'b':   //convert input file to a Portable Bitmap (PBM) file. (DEFAULT)
            transformation_counter++;
            strcpy(options.opt, "b");
            break;

        case 'g':   //convert input file to a Portable Gray Map (PGM) file using the specified max grayscale pixel value[1-65535]
            transformation_counter++;
            strcpy(options.opt, "g");
            options.max_pixel_value = atoi(optarg);
            if(options.max_pixel_value > 65535 || options.max_pixel_value < 1) {    //validate max grayscale pixel value
                fprintf(stderr, "Error: Invalid max grayscale pixel value: %s; must be less than 65,536\n", optarg);
                exit(1);
            }
            break;

        case 'i':   //isolate the specified RGB channel. Valid channels are “red”, “green”, or “blue”
            transformation_counter++;
            strcpy(options.opt, "i");
            strcpy(options.color, optarg);
            //validate the color input is valid.
            if ( !(strcmp(options.color, "red")==0) & !(strcmp(options.color, "blue")==0) & !(strcmp(options.color, "green")==0)){
                fprintf(stderr, "Error: Invalid channel specification: (%s); should be 'red', 'green', or 'blue'\n", options.color);
                exit(1);
            } 
            break;

        case 'r':   //remove the specified RGB channel. Valid channels are “red”, “green”, or “blue”
            transformation_counter++;
            strcpy(options.opt, "r");
            strcpy(options.color, optarg);
            //validate the color input is valid.
            if ( !(strcmp(options.color, "red")==0) & !(strcmp(options.color, "blue")==0) & !(strcmp(options.color, "green")==0)){
                fprintf(stderr, "Error: Invalid channel specification: (%s); should be 'red', 'green', or 'blue'\n", options.color);
                exit(1);
            } 
            break;

        case 's':   //apply a sepia transformation
            transformation_counter++;
            strcpy(options.opt, "s");
            break;

        case 'm':   //vertically mirror the first half of the image to the second half
            transformation_counter++;
            strcpy(options.opt, "m");
            break;

        case 't':   //reduce the input image to a thumbnail based on the given scaling factor [1-8]
            transformation_counter++;
            strcpy(options.opt, "t");
            options.thumbnail_scale = atoi(optarg);
            //validate the input scale is within 1 to 8
            if (options.thumbnail_scale < 1 || options.thumbnail_scale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", options.thumbnail_scale);
                exit(1);
            }
            break;

        case 'n':   //tile thumbnails of the input image based on the given scaling factor [1-8]
            transformation_counter++;
            strcpy(options.opt, "n");
            options.thumbnail_scale = atoi(optarg);
            //validate the input scale is within 1 to 8
            if (options.thumbnail_scale < 1 || options.thumbnail_scale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", options.thumbnail_scale);
                exit(1);
            }
            break;

        case 'o':   //write output image to the specified file. Existent output files will be overwritten
            used_dash_o=true;
            strcpy(options.o_filename, optarg);
            if (strcmp(options.opt, "") == 0) { //if no option specified, -b will be the default.
                strcpy(options.opt, "b");
            }
            break;
        
        case '?':   //malformed command line  
            fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
            exit(1);
            break;
        
        case ':':  // Add this case
            if (optopt == 'o') {  // Check if the option that was missing an argument was -o
                fprintf(stderr, "Missing argument for option -o\n");
                exit(1);
            }
            exit(1);
            break;
        
        default:    
            transformation_counter++;
            strcpy(options.opt, "b");
            break;
        }
    }

    if (transformation_counter > 1){  //multiple transformation request handling
        fprintf(stderr, "Error: Multiple transformations specified\n");
        exit(1);
    }

    if (!used_dash_o){
        fprintf(stderr, "Error: No output file specified\n");
        exit(1);
    }

    //record output filename. Case for 'no -o', '-o but no following file', 'correct -o' are all handled in for loop.
    if (optind < argc) {
        strcpy(options.i_filename, argv[optind]);
    } else {
        fprintf(stderr, "Error: No input file specified\n");
        exit(1);
    }
 
    if (strcmp(options.opt, "g")==0){
        convert_to_PGM(options);
    } else if (strcmp(options.opt, "i")==0){
        isolate_RGB_channel(options);
    } else if (strcmp(options.opt, "r")==0){
        remove_RGB_channel(options);
    } else if (strcmp(options.opt, "s")==0){
        sepia_transformation(options);
    } else if (strcmp(options.opt, "m")==0){
        mirror(options);
    } else if (strcmp(options.opt, "t")==0){
        thumbnail(options);
    } else if (strcmp(options.opt, "n")==0){
        thumbnail_tile(options);
    } else {
        convert_to_PBM(options.i_filename, options.o_filename); //handles default operation: -b
    }

    return 0;
}
