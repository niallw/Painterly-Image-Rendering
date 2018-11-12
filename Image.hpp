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
#include "Color.hpp"

using namespace std;

class Image{
public:
    Image(int w, int h, int max);
    Image(const char* file_name);
    ~Image(void);
    void writeImage(const char* file_name);
    int getWidth();
    int getHeight();
    Color** getImage();
    Color getRGB(int, int);
    void setColor(int, int, Color);
    void addColor(int, int, Color);
    
private:
    char m_magic_number[2];
    int m_width;
    int m_height;
    int m_max;
    Color** m_image;
};

#endif /* Image_hpp */
