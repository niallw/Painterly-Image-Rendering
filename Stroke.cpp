
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

// Vector* Stroke::calculate_spline(float t){
//     int degree = calculate_degree(T_RESOLUTION, control_points.size());
//     Vector* sum = new Vector(0.0, 0.0);

//     for (int i = 0; i < control_points.size(); i++){
//         Vector* cur_control_point = control_points[i];
//         // float cur_N = calculate_N(t, i, degree, 1.0);

//         // *sum = *sum + (*cur_control_point * cur_N);
//     }
    
//     return sum;
// }

int Stroke::calculate_degree(int m, int n){
    return m - n - 1;   
}

void Stroke::draw_stroke(Image* canvas, int spline_degree){
    int num_ctrl_points = control_points.size();
    int num_knots = control_points.size() + spline_degree + 1;
    vector<float> knots = make_knot_vector(num_knots, spline_degree, num_ctrl_points);

    for (float t = 0.0; t <= 1.0; t += 1.0/STROKE_RESOLUTION){
        Vector curve_point = Vector(0.0, 0.0);

        for (int i = 0; i < num_ctrl_points; i++){
            Vector* cur_point = control_points[i];
            float N = calculate_N(t, i, spline_degree, knots);
            curve_point = curve_point + (*cur_point * N);
        }

        vector<vector<float>> circle_points = calc_circ((int)curve_point.get_y(),
                                                        (int)curve_point.get_x(),
                                                        radius, canvas->getHeight(),
                                                        canvas->getWidth());
        for (auto point : circle_points){
            canvas->setColor(point[0], point[1], color);
        }
    }
}

vector<float> Stroke::make_knot_vector(int m, int p, int n){
    vector<float> knots;
    for (int i = 0; i <= p; i++){
        knots.push_back(0.0);
    }
    for (int i = 1; i < n - p; i++){
        knots.push_back((float)i/(float)(n-p+1));
    }
    for (int i = 0; i <= p; i++){
        knots.push_back(1.0);
    }
    return knots;
}

float Stroke::calculate_N(float t, int i, int j, vector<float> knots){
//    https://stackoverflow.com/questions/53564615/how-do-i-get-the-b-spline-curve-to-connect-to-the-final-control-point
//    jim bob
//    o831790@nwytg.net
//    painting96!
    float t_1 = knots[i];
    float t_2 = knots[(i + j)];
    float t_3 = knots[(i + 1)];
    float t_4 = knots[(i + j + 1)];
    if (t_1 == 1.0){
        cout<<"E";
    }

    // Base case of basis function
    if (j == 0){
        if (t_1 <= t && t < t_3) return 1;
        else return 0;
    }

    float temp1 = (t_2 - t_1 == 0) ? 0 : ((t - t_1) / (t_2 - t_1)) * calculate_N(t, i, j-1, knots);
    float temp2 = (t_4 - t_3 == 0) ? 0 : ((t_4 - t) / (t_4 - t_3)) * calculate_N(t, i+1, j-1, knots);
//    if (t_2 - t_1 == 0) temp1 = 0;
//
//    if (t_4 - t_3 == 0) temp2 = 0;
//    else{
//        ;
//        temp2 = ((t_4 - t) / (t_4 - t_3)) * calculate_N(t, i+1, j-1, knots);
//    }

//    float temp3 = ;


    return temp1 + temp2;
}

/** Calculate the pixels about a pixel that fall into a circle that is drawn
 *  at that pixel center. This is used so we know which pixels to fill in
 *  when rendering the strokes.
 *  c_x - x-coordinate of the center of the circle.
 *  c_y - y-coordinate of the center of the circle.
 *  r - Radius of the circle.
 */
// TODO: CHANGE THE NAMES OF C_X AND C_Y. C_X IS CURRENTLY THE ROW, AND C_Y IS THE COL
vector<vector<float>> Stroke::calc_circ(int c_x, int c_y, int r, int height, int width){
    vector<vector<float>> points;

    for (int x = c_x - r; x <= c_x; x++){
        for (int y = c_y - r; y <= c_y; y++){
            int test = (x - c_x)*(x - c_x) + (y - c_y)*(y - c_y);
            if (test <= r*r){
                int x_sym = c_x - (x - c_x);
                int y_sym = c_y - (y - c_y);
                if (x < 0 || x >= height || y < 0 || y >= width ||
                    x_sym < 0 || x_sym >= height || y_sym < 0 || y_sym >= width){
                    break;
                }

                vector<float> p1, p2, p3, p4;
                p1.push_back(x);
                p1.push_back(y);

                p2.push_back(x);
                p2.push_back(y_sym);

                p3.push_back(x_sym);
                p3.push_back(y);

                p4.push_back(x_sym);
                p4.push_back(y_sym);

                points.push_back(p1);
                points.push_back(p2);
                points.push_back(p3);
                points.push_back(p4);
            }
        }
    }

    return points;
}