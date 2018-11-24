
#include "Stroke.hpp"
#include "Image.hpp"

using namespace std;

Stroke::Stroke(int x, int y, int radius, Image* reference_image){
    this->control_points.push_back(new Vector(x, y));
    this->radius = radius;
    this->reference_image = reference_image;
    this->color = reference_image->getRGB(x, y);
}

Stroke::~Stroke(){
    for (Vector* v : control_points)
        delete v;
    control_points.clear();
    delete reference_image;
}

Color Stroke::get_color(){
    return color;
}

void Stroke::add_control_point(int x, int y){
    control_points.push_back(new Vector(x, y));
}