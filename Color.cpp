//
//  Color.cpp
//  RayTracer
//
//  Created by Peck, Tabitha on 10/15/16.
//  Copyright © 2016 Davidson College. All rights reserved.
//

#include <cmath>
#include "Color.hpp"

Color::Color(){
    m_color[0] = 0.0;
    m_color[1] = 0.0;
    m_color[2] = 0.0;
}

Color::Color(float r, float g, float b){
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
}
Color::~Color(){
    
}

Color Color::operator+(const Color& c){
    return Color(m_color[0] + c.m_color[0], m_color[1] + c.m_color[1], m_color[2] + c.m_color[2]);
}

//TODO: do I actually need to keep this? removed it to be able to do the special color difference from the paper
// Color Color::operator-(const Color& c){
//     return Color(m_color[0] - c.m_color[0], m_color[1] - c.m_color[1], m_color[2] - c.m_color[2]);
// }

float Color::operator-(const Color& c){
    float r_1 = m_color[0];
    float g_1 = m_color[1];
    float b_1 = m_color[2];

    float r_2 = c.m_color[0];
    float g_2 = c.m_color[1];
    float b_2 = c.m_color[2];

    float new_r = pow((r_1 - r_2), 2);
    float new_g = pow((g_1 - g_2), 2);
    float new_b = pow((b_1 - b_2), 2);

    return sqrt(new_r + new_g + new_b);
}

Color Color::operator*(const Color& c){
    return Color(m_color[0] * c.m_color[0], m_color[1] * c.m_color[1], m_color[2] * c.m_color[2]);
}

Color Color::operator/(const Color& c){
    return Color(m_color[0] / c.m_color[0], m_color[1] / c.m_color[1], m_color[2] / c.m_color[2]);
}

Color Color::operator*(float f){
    return Color(m_color[0] * f, m_color[1] * f, m_color[2] * f);
}

Color Color::operator/(float f){
    return Color(m_color[0] / f, m_color[1] / f, m_color[2] / f);
}

float Color::get_r(){
    return m_color[0];
}

float Color::get_g(){
    return m_color[1];
}

float Color::get_b(){
    return m_color[2];
}

void Color::clamp(){
    for(int i = 0; i < 3; i++){
        if(m_color[i] > 1.0){
            m_color[i] = 1.0;
        }
        else{
            if(m_color[i] < 0.0){
                m_color[i] = 0.0;
            }
        }
    }
}
