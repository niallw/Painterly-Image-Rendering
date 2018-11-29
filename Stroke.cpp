
#include "Stroke.hpp"
#include "Image.hpp"

using namespace std;

Stroke::Stroke(int x, int y, int radius){
    this->control_points.push_back(new Vector(x, y));
    this->radius = radius;
}

Stroke::~Stroke(){
    for (Vector* v : control_points)
        delete v;
    control_points.clear();
}

void Stroke::set_color(Color c){
    this->color = Color(c.get_r(), c.get_g(), c.get_b());
}

Color Stroke::get_color(){
    return color;
}

int Stroke::get_radius(){
    return radius;
}

void Stroke::add_control_point(int x, int y){
    control_points.push_back(new Vector(x, y));
}

vector<Vector*> Stroke::get_control_points(){
    return control_points;
}

Vector* Stroke::calculate_spline(float t){
    int degree = calculate_degree(T_RESOLUTION, control_points.size());
    Vector* sum = new Vector(0.0, 0.0);

    for (int i = 0; i < control_points.size(); i++){
        Vector* cur_control_point = control_points[i];
        float cur_N = calculate_N(t, i, degree);

        *sum = *sum + (*cur_control_point * cur_N);
    }
    
    return sum;
}

int Stroke::calculate_degree(int m, int n){
    return m - n - 1;
}

float Stroke::calculate_N(float t, int i, int j){
    float t_1 = (1.0 / T_RESOLUTION) * i;
    float t_2 = (1.0 / T_RESOLUTION) * (i + j);
    float t_3 = (1.0 / T_RESOLUTION) * (i + 1);
    float t_4 = (1.0 / T_RESOLUTION) * (i + j + 1);

    // Base case of basis function
    if (j == 0){
        if (t_1 <= t && t < t_3) return 1;
        else return 0;
    }

    return (((t - t_1) / (t_2 - t_1)) * calculate_N(t, i, j-1)) + 
           (((t_4 - t) / (t_4 - t_3)) * calculate_N(t, i+1, j-1));
}