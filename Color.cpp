//
//  Color.cpp
//  RayTracer
//
//  Created by Peck, Tabitha on 10/15/16.
//  Copyright © 2016 Davidson College. All rights reserved.
//

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

Color Color::operator-(const Color& c){
    return Color(m_color[0] - c.m_color[0], m_color[1] - c.m_color[1], m_color[2] - c.m_color[2]);
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
