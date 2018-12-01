// Class for creating brush strokes. 
// Strokes are represented by cubic B-splines, which we define
// with control points.
// Authors: James Plaut and Niall Williams

#include "Stroke.hpp"
#include "Image.hpp"

using namespace std;

/** Constructor
 *  x - x-coordinate of the first control point
 *  y - y-coordinate of the first control point
 *  radius - Radius of the brush this stroke will be painted with.
 */
Stroke::Stroke(int x, int y, int radius){
    this->control_points.push_back(new Vector(x, y));
    this->radius = radius;
}

/** Deconstructor
 */
Stroke::~Stroke(){
    for (Vector* v : control_points)
        delete v;
    control_points.clear();
}

/** Set the color of the stroke.
 *  c - Color of the stroke.
 */
void Stroke::set_color(Color c){
    this->color = Color(c.get_r(), c.get_g(), c.get_b());
}

/** Return the color of the stroke.
 */
Color Stroke::get_color(){
    return color;
}

/** Return the radius of the stroke.
 */
int Stroke::get_radius(){
    return radius;
}

/** Add another control point to the spline.
 *  x - x-coordinate of the control point being added.
 *  y - y-coordinate of the control point being added.
 */
void Stroke::add_control_point(int x, int y){
    control_points.push_back(new Vector(x, y));
}

/** Return a list of the spline's control points.
 */
vector<Vector*> Stroke::get_control_points(){
    return control_points;
}

/**Calculate the curve defined by the stroke's control points,
 * and draw the curve onto the canvas. This is where the actual
 * painting happens.
 * canvas - The image object we are painting on.
 * spline_degree - Degree of the spline to draw. In this case, the
 *                  degree is 3 because we are drawing cubic B-splines.
 */
void Stroke::draw_stroke(Image* canvas, int spline_degree){
    int num_ctrl_points = control_points.size();
    int num_knots = control_points.size() + spline_degree + 1;
    vector<float> knots = make_knot_vector(num_knots, spline_degree, num_ctrl_points);

    // Calculate the position of the point along the curve at time t
    for (float t = 0.0; t <= 1.0; t += 1.0/STROKE_RESOLUTION){
        Vector curve_point = Vector(0.0, 0.0);

        // Weight each control point based on t and sum them.
        for (int i = 0; i < num_ctrl_points; i++){
            Vector* cur_point = control_points[i];
            float N = calculate_N(t, i, spline_degree, knots);
            curve_point = curve_point + (*cur_point * N);
        }

        vector<Vector> circle_points = calc_circ((int)curve_point.get_y(),
                                                (int)curve_point.get_x(),
                                                radius, canvas->getHeight(),
                                                canvas->getWidth());

        // Paint!
        for (Vector point : circle_points){
            canvas->setColor(point.get_y(), point.get_x(), color);
        }
    }
}

/**Build the knot vector which is needed to draw the spline curve.
 * m - The number of knots to calculate.
 * p - The degree of the spline.
 * n - The number of control points for the spline.
 */
vector<float> Stroke::make_knot_vector(int m, int p, int n){
    vector<float> knots;

    // Forces the curve to start at the first control point.
    for (int i = 0; i <= p; i++){
        knots.push_back(0.0);
    }
    for (int i = 1; i < n - p; i++){
        knots.push_back((float)i / (float)(n-p+1));
    }
    // Forces the curve to end at the last control point.
    for (int i = 0; i <= p; i++){
        knots.push_back(1.0);
    }

    return knots;
}

/**Calculate the position of the point on the curve at time t along the curve.
 * A B-spline is defined recursively. A point along the curve at time t is 
 * calculated by weighting each of the control points and summing them.
 * t - Time variable to denote step-size along the curve. Goes from 0 to 1.
 * i - Index of the control point whose weight is being calculated.
 * j - Index of the knot.
 * knots - List of knots
 */
float Stroke::calculate_N(float t, int i, int j, vector<float> knots){
    float t_1 = knots[i];
    float t_2 = knots[(i + j)];
    float t_3 = knots[(i + 1)];
    float t_4 = knots[(i + j + 1)];

    // Base case of basis function
    if (j == 0){
        if (t_1 <= t && t < t_3) return 1;
        else return 0;
    }

    // Check for divide by zero
    float temp1 = (t_2 - t_1 == 0) ? 0 : ((t - t_1) / (t_2 - t_1)) * calculate_N(t, i, j-1, knots);
    float temp2 = (t_4 - t_3 == 0) ? 0 : ((t_4 - t) / (t_4 - t_3)) * calculate_N(t, i+1, j-1, knots);

    return temp1 + temp2;
}

/**Calculate the pixels around a center pixel that fall into a circle that is drawn
 * at that pixel center. This is used so we know which pixels to fill in
 * when rendering the strokes.
 * c_y - y-coordinate of the center of the circle.
 * c_x - x-coordinate of the center of the circle.
 * r - Radius of the circle.
 * height - Height of the image.
 * width - Width of the image.
 */
vector<Vector> Stroke::calc_circ(int c_y, int c_x, int r, int height, int width){
    vector<Vector> points;

    for (int y = c_y - r; y <= c_y; y++){
        for (int x = c_x - r; x <= c_x; x++){
            int distance = (y - c_y)*(y - c_y) + (x - c_x)*(x - c_x);

            if (distance <= r*r){
                // Use symmetry to quickly calculate the points in the other 3 
                // quandrants of the circle to be drawn.
                int y_sym = c_y - (y - c_y);
                int x_sym = c_x - (x - c_x);

                if (y < 0 || y >= height || x < 0 || x >= width ||
                    y_sym < 0 || y_sym >= height || x_sym < 0 || x_sym >= width){
                    break;
                }

                Vector p1 = Vector(x, y);
                Vector p2 = Vector(x_sym, y);
                Vector p3 = Vector(x,y_sym);
                Vector p4 = Vector(x_sym, y_sym);

                points.push_back(p1);
                points.push_back(p2);
                points.push_back(p3);
                points.push_back(p4);
            }
        }
    }

    return points;
}