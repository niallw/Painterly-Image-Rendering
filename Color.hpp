//
//  Color.hpp
//  RayTracer
//
//  Created by Peck, Tabitha on 10/15/16.
//  Copyright © 2016 Davidson College. All rights reserved.
//

#ifndef Color_hpp
#define Color_hpp

#include <stdio.h>

class Color{
private:
    float m_color[3];
    
public:
    Color();
    Color(float, float, float);
    ~Color();
    
    Color operator+(const Color& c);
    float operator-(const Color& c);
    Color operator*(const Color& c);
    Color operator/(const Color& c);
    Color operator*(float f);
    Color operator/(float f);
    float get_r();
    float get_g();
    float get_b();
    void clamp();
};
#endif /* Color_hpp */
