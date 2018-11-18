
#include "Stroke.hpp"
#include "Image.hpp"

Stroke::Stroke(int x, int y, int radius, Image* reference_image){
    this->control_points.push_back(new Vector(x, y));
    this->radius = radius;
    this->reference_image = reference_image;
}

Stroke::~Stroke(){
    for (Vector* v : control_points)
        delete v;
    control_points.clear();
    delete reference_image;
}