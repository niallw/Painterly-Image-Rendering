
#include "Stroke.hpp"
#include "Image.hpp"

using namespace std;

Stroke::Stroke(int x, int y, int radius, Color c){
    this->control_points.push_back(new Vector(x, y));
    this->radius = radius;
    this->color = Color(c.get_r(), 
                        c.get_g(), 
                        c.get_b());
}

Stroke::~Stroke(){
    for (Vector* v : control_points)
        delete v;
    control_points.clear();
}

Color Stroke::get_color(){
    return color;
}

void Stroke::add_control_point(int x, int y){
    control_points.push_back(new Vector(x, y));
}

vector<Vector*> Stroke::get_control_points(){
    return control_points;
}