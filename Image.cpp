//
//  Image.cpp
//  RayTracer
//
//  Created by Peck, Tabitha on 10/15/16.
//  Copyright © 2016 Davidson College. All rights reserved.
//

#include "Image.hpp"

//
//  ImageProcessor.cpp
//  Resolution
//
//  Created by Tabitha Peck on 7/23/13.
//  Copyright (c) 2013 Tabitha Peck. All rights reserved.
//  based on code from stack overflow: http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c


/*
 Create a new blank image
 */
Image::Image(int w, int h, int m){
    m_width = w;
    m_height = h;
    m_max = m;
    m_image = new Color*[m_width];
    for(int i = 0; i < m_width; i++){
        m_image[i] = new Color[m_height];
        for(int j = 0; j < m_height; j++){
            m_image[i][j] = Color();
        }
    }
}

// Image::Image(const char* file_name){
    
//     FILE* file;
//     char buff[16];
//     float r, g, b;
    
//     file = fopen(file_name, "r"); // open file for reading
    
//     if(!file){
//         fprintf(stderr, "Unable to open file %s", file_name);
//         exit(1);
//     }
    
//     fscanf(file, "%s%*[^\n]%*c", m_magic_number); //read magic number and white space
    
//     if(m_magic_number[0] != 'P' || m_magic_number[1] != '3'){
//         printf("Incorrect file type");
//         exit(1);
//     }
    
//     //check for comments
//     fscanf(file, "%s", buff);
//     while (strncmp(buff, "#", 1) == 0) {
//         fscanf(file, "%s%*[^\n]%*c", buff);
//     }
    
//     if (fscanf(file, "%d %d %d", &m_width, &m_height, &m_max) != 3) {
//         fprintf(stderr, "Invalid image size (error loading '%s')\n", file_name);
//         exit(1);
//     }
    
//     image = new float **[height];
//     for(int i = 0; i < height; i++) {
//         image[i] = new float *[width];
//         for(int j = 0; j<width; j++){
//             image[i][j] = new float[3];
//             if(fscanf(file, "%f %f %f", &r, &g, &b) != 3){
//                 fprintf(stderr, "Invalid pixel reading\n");
//                 exit(1);
//             }
//             image[i][j][0] = r/255;
//             image[i][j][1] = g/255;
//             image[i][j][2] = b/255;
//         }
//     }
    
//     fclose(file);
//     imageDisplayArray = NULL;
// }

Image::~Image(void){
    for(int i = 0; i < m_width; i++){
        delete [] m_image[i];
    }
    delete [] m_image;
}

void Image::setColor(int w, int h, Color c){
    m_image[w][h] = c;
}

void Image::addColor(int w, int h, Color c){
    m_image[w][h] = m_image[w][h] + c;
    m_image[w][h].clamp();
}

int Image::getWidth(){
    return m_width;
}

int Image::getHeight(){
    return m_height;
}

Color** Image::getImage(){
    return m_image;
}

void Image::writeImage(const char* file_name){
    FILE* file;
    file = fopen(file_name, "w");
    if(!file){
        fprintf(stderr, "Unable to open file %s", file_name);
        exit(1);
    }
    
    // your code goes here
    fprintf(file, "P3\n");
    fprintf(file, "# CREATOR: Tabitha C. Peck\n");
    
    fprintf(file, "%d %d\n%d\n", m_width, m_height, m_max);
    for(int i = 0; i < m_height; i++){
        for(int j = 0; j < m_width; j++){
            Color c = m_image[j][i]*255;
            fprintf(file, "%d %d %d\n", (int)c.get_r(), (int)c.get_g(), (int)c.get_b() );
        }
    }
    
    fclose(file);
    
}


