//
//  ImageIO.h
//  Resolution
//
//  Created by Tabitha Peck on 7/23/13.
//  Copyright (c) 2013 Tabitha Peck. All rights reserved.
//

#ifndef __Resolution__ImageIO__
#define __Resolution__ImageIO__

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// #ifdef __APPLE__
// #include <GLUT/glut.h>
// #else
// #include <glut.h>
// #endif
using namespace std;


class ImageIO{
public:
    ImageIO(int w, int h, int max);
    ImageIO(const char* file_name);
    ~ImageIO(void);
    void writeImage(const char* file_name);
    int getWidth();
    int getHeight();
    float*** getImage();
    float* getRGB(int, int);
    float* getImageDisplayArray();

private:
    char magic_number[2];
    int width;
    int height;
    int max;
    float*** image;
    float* imageDisplayArray;

};

#endif /* defined(__Resolution__ImageIO__) */
