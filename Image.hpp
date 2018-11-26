//
//  Image.h
//  ppmReaderWriter
//
//  Created by Peck, Tabitha on 9/18/14.
//  Copyright (c) 2014 Davidson College. All rights reserved.
//

#ifndef Image_hpp
#define Image_hpp

#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include <vector>

#include "Color.hpp"

using namespace std;

class Image{
public:
    Image(int w, int h, int max);
    Image(string file_name);
    ~Image(void);
    void writeImage(string file_name);
    int getWidth();
    int getHeight();
    Color** getImage();
    Color getRGB(int, int);
    void setColor(int, int, Color);
    void addColor(int, int, Color);

    vector<vector<float>> operator-(Image);
    Image blur(int, int);
    void sobel_full();
    Image sobel_x();
    Image sobel_y();
    
private:
    char m_magic_number[2];
    int m_width;
    int m_height;
    int m_max;
    int const sobel_1[3] = {1, 0, -1};
    int const sobel_2[3] = {1, 2, 1};
    Color** m_image;

    Image grayscale();
    vector<vector<float>> calculate_kernel(int, int);
};

#endif /* Image_hpp */
