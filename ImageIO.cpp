//
//  ImageIO.cpp
//  Resolution
//
//  Created by Tabitha Peck on 7/23/13.
//  Copyright (c) 2013 Tabitha Peck. All rights reserved.
//  based on code from stack overflow: http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c


#include "ImageIO.h"



ImageIO::ImageIO(int w, int h, int m){
    width = w;
    height = h;
    max = m;
    image = new float**[height];
    for(int i = 0; i < height; i++){
        image[i] = new float*[width];
        for(int j = 0; j < width; j++){
            image[i][j] = new float[3];
            image[i][j][0] = 0;
            image[i][j][1] = 0;
            image[i][j][2] = 0;
        }
    }
    imageDisplayArray = NULL;
}

ImageIO::ImageIO(const char* file_name){
    
    FILE* file;
    char buff[16];
    float r, g, b;
    
    file = fopen(file_name, "r"); // open file for reading
    
    if(!file){
        fprintf(stderr, "Unable to open file %s", file_name);
        exit(1);
    }
    
    fscanf(file, "%s%*[^\n]%*c", magic_number); //read magic number and white space
    
    if(magic_number[0] != 'P' || magic_number[1] != '3'){
        printf("Incorrect file type");
        exit(1);
    }
    
    //check for comments
    fscanf(file, "%s", buff);
    while (strncmp(buff, "#", 1) == 0) {
        fscanf(file, "%s%*[^\n]%*c", buff);
    }
    
    if (fscanf(file, "%d %d %d", &width, &height, &max) != 3) {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", file_name);
        exit(1);
    }
    
    image = new float **[height];
    for(int i = 0; i < height; i++) {
        image[i] = new float *[width];
        for(int j = 0; j<width; j++){
            image[i][j] = new float[3];
            if(fscanf(file, "%f %f %f", &r, &g, &b) != 3){
                fprintf(stderr, "Invalid pixel reading\n");
                exit(1);
            }
            image[i][j][0] = r/255;
            image[i][j][1] = g/255;
            image[i][j][2] = b/255;
        }
    }
    
    fclose(file);
    imageDisplayArray = NULL;
}

ImageIO::~ImageIO(void){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            delete [] image[i][j];
        }
        delete [] image[i];
    }
    delete [] image;
    
    delete [] imageDisplayArray;
}

int ImageIO::getWidth(){
    return width;
}

int ImageIO::getHeight(){
    return height;
}

float*** ImageIO::getImage(){
    return image;
}

float* ImageIO::getImageDisplayArray() {
    if(imageDisplayArray != NULL)
    return imageDisplayArray;
    
    imageDisplayArray = new float[width * height * 3];
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int index = width*height*3 - (width*i + width - j)*3;
            imageDisplayArray[index] = image[i][j][0];
            imageDisplayArray[index + 1] = image[i][j][1];
            imageDisplayArray[index + 2] = image[i][j][2];
        }
    }
    return imageDisplayArray;
}

void ImageIO::writeImage(const char* file_name){
    FILE* file;
    file = fopen(file_name, "w");
    if(!file){
        fprintf(stderr, "Unable to open file %s", file_name);
        exit(1);
    }
    
    fprintf(file, "P3\n");
    fprintf(file, "# CREATOR: Tabitha\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max);
    
    for(int i = 0; i < height; i ++){
        for (int j = 0; j < width; j++){
            fprintf(file, "%d %d %d\n", (int)(image[i][j][0]*255), (int)(image[i][j][1]*255), (int)(image[i][j][2]*255));
        }
    }
    fclose(file);
}



