#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <climits>
#include <numeric>
#include <random>

#include "Image.hpp"
#include "Stroke.hpp"

using namespace std;

int height, width;
const float GRID_FACTOR = 1.0;
const int MIN_BRUSH_SIZE = 2;
const int BRUSH_RATIO = 2/1;
const int NUM_BRUSHES = 3;
const float THRESHOLD = 50.0;
const float CURVATURE_FILTER = 0.25;
const int SPLINE_DEGREE = 3; // Cubic spline
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/";
auto rng = default_random_engine {};

// TODO: FIXME: B SPLINE WEBSITE. DELETE AFTER
// http://research.engr.utexas.edu/cagd/B-Spline-Interaction/
// http://nurbscalculator.in/

/** Creates a difference map that always returns max int which will guarantee
 *  that we paint strokes. The difference map error (error between the reference image
 *  and our canvas) is always the max value with the difference map created by this
 *  function, which means the error is always above the threshold, so we always paint.
 */
vector<vector<float>> generate_blank_canvas(){
    vector<vector<float>> diff;

    for (int row = 0; row < height; row++){
        vector<float> temp;
        for (int col = 0; col < width; col++){
            temp.push_back(INTMAX_MAX);
        }
        diff.push_back(temp);
    }

    return diff;
}

/** Finds all pixels that are neighbors of a given pixel.
 *  diff_map - The point-wise color difference map. AKA the pixel values that
 *             represent the color difference between the reference image and
 *             our current canvas.
 *  row - y-coordinate of the pixel whose neighbors we want to find.
 *  col - x-coordinate of the pixel whose neighbors we want to find.
 *  grid_size - The size of the "window" we sample the image with. This is based on
 *              the brush size.
 */
vector<vector<float>> get_neighbors(vector<vector<float>> diff_map, int row, int col, int grid_size){
    vector<vector<float>> neighbors;
//    if (col == 340)
//        cout <<"A"<<endl;

    for (int i = -(grid_size/2); i <= grid_size/2; i++){
        for (int j = -(grid_size/2); j <= grid_size/2; j++){
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            if (neighbor_row >= 0 && neighbor_row < height &&
                neighbor_col >= 0 && neighbor_col < width){
                vector<float> temp;
                temp.push_back(neighbor_row);
                temp.push_back(neighbor_col);
                temp.push_back(diff_map[neighbor_row][neighbor_col]);
                neighbors.push_back(temp);
            }
        }
    }

    return neighbors;
}

/** Creates a stroke. This is done by calculating a series of control points
 *  that define a B-spline which will represent our stroke. The control points
 *  are placed perpendicular to the image gradient.
 *  x - x-coordinate of the first control point.
 *  y - y-coordinate of the first control point.
 *  brush_size - The size of the brush we are painting with.
 *  ref_image - The blurred image we are painting.
 *  canvas - The canvas image we are painting onto.
 *  sobel_x - The horizontal gradient of the reference image.
 *  sobel_y - The vertical gradient of the reference image.
 */
Stroke* make_stroke(int y, int x, int brush_size, Image* ref_image,
                    Image* canvas, Image* sobel_x, Image* sobel_y){
    Stroke* stroke = new Stroke(x, y, brush_size);
    Color stroke_color = ref_image->getRGB(y, x);
    stroke->set_color(stroke_color);
    int cur_x = x;
    int cur_y = y;
    float last_Dx = 0;
    float last_Dy = 0;

    for (int i = 1; i <= MAX_STROKE_LENGTH; i++){
        Color ref_image_color = Color(ref_image->getRGB(cur_y, cur_x).get_r(),
                                      ref_image->getRGB(cur_y, cur_x).get_g(),
                                      ref_image->getRGB(cur_y, cur_x).get_b());
        Color canvas_color = canvas->getRGB(cur_y, cur_x);

        if ((i > MIN_STROKE_LENGTH) &&
            (abs(ref_image_color - canvas_color) < abs(ref_image_color - stroke_color))){
            return stroke;
        }

        // Detect vanishing gradient
        float gradient_mag = sqrt(sobel_x->getRGB(cur_y, cur_x).get_r() +
                                  sobel_y->getRGB(cur_y, cur_x).get_r());
        if (gradient_mag == 0){
            return stroke;
        }

        // Get unit vector of gradient
        float theta = atan(sobel_x->getRGB(cur_y, cur_x).get_r() /
                           sobel_y->getRGB(cur_y, cur_x).get_r());
        float g_x = cos(theta);
        float g_y = sin(theta);
        // Compute a normal to the gradient
        float d_x = -g_y;
        float d_y = g_x;

        // Reverse normal if necessary
        if (last_Dx * d_x + last_Dy * d_y < 0){
            d_x *= -1;
            d_y *= -1;
        }

        // Filter the stroke direction
        d_x = CURVATURE_FILTER * (d_x) + (1 - CURVATURE_FILTER) * last_Dx;
        d_y = CURVATURE_FILTER * (d_y) + (1 - CURVATURE_FILTER) * last_Dy;
        float denom = sqrt(d_x * d_x + d_y * d_y); // Just do it once because it's expensive
        d_x = d_x / denom;
        d_y = d_y / denom;
        cur_x = cur_x + brush_size * d_x;
        cur_y = cur_y + brush_size * d_y;

        // Clamp
        if (cur_x < 0) cur_x = 0;
        if (cur_y < 0) cur_y = 0;
        if (cur_x >= width) cur_x = width - 1;
        if (cur_y >= height) cur_y = height - 1;

        last_Dx = d_x;
        last_Dy = d_y;

        stroke->add_control_point(cur_x, cur_y);
    }

    return stroke;
}

/** Paint one layer of the image with the specified brush size. The painting
 *  is based off a Gaussian blurred image with a standard deviation based on
 *  the brush size.
 *  cavas - The image cavas we are painting onto.
 *  ref_image - The blurred image we are recreating with brush strokes.
 *  brush_size - The current brush size we are painting with.
 *  is_first_layer - Flag that is used to know which point-wise pixel color difference
 *                   map to use. On the first layer, the canvas is empty so we need a
 *                   difference map that always forces us to draw strokes.
 */
void paint_layer(Image* canvas, Image* ref_image, int brush_size, bool is_first_layer){
    vector<Stroke*> strokes;
    Image sobel_x = ref_image->sobel_x();
    Image sobel_y = ref_image->sobel_y();
    int grid_size = GRID_FACTOR * brush_size;
    vector<vector<float>> difference;

    // Build the pointwise difference image
    if (is_first_layer){
        difference = generate_blank_canvas();
    }
    else{
        difference = *canvas - *ref_image;
    }

    // Calculate error in difference map to locate regions we want to paint
    for (int row = 0; row < height; row+=grid_size){
        for (int col = 0; col < width; col+=grid_size){
            vector<vector<float>> neighboring_points = get_neighbors(difference, row, col, grid_size);
            float area_error = 0;

            for (int i = 0; i < neighboring_points.size(); i++)
                area_error += neighboring_points[i][2];
            area_error /=  pow(grid_size, 2);

            if (area_error > THRESHOLD){
                int max_row;
                int max_col;
                float max = INTMAX_MIN;

                for (int i = 0; i < neighboring_points.size(); i++){
                    if (neighboring_points[i][2] > max){
                        max_row = neighboring_points[i][0];
                        max_col = neighboring_points[i][1];
                        max = neighboring_points[i][2];
                    }
                }

                Stroke* s = make_stroke(max_row, max_col, brush_size, ref_image,
                                        canvas, &sobel_x, &sobel_y);
                strokes.push_back(s);
            }
        }
    }

    cout<<"num strokes: "<<strokes.size()<<endl;
    shuffle(strokes.begin(), strokes.end(), rng);
    // Draw each stroke
    for (Stroke* s : strokes){
        s->draw_stroke(canvas, SPLINE_DEGREE);
    }

//     Stroke* last = strokes[strokes.size()-1];
//     shuffle(strokes.begin(), strokes.end(), rng);
//     for (auto stroke : strokes){
//         // B SPLINE
//         int degree = T_RESOLUTION - stroke->get_control_points().size() - 1;
//         auto points = stroke->get_control_points();

//         for (float t = 0.0; t < 1.0; t+= 1.0/(T_RESOLUTION)){
//             Vector sum = Vector(0.0, 0.0);

//             for (int i = 0;i < stroke->get_control_points().size(); i++){
//                 Vector next = *(stroke->get_control_points()[i]);
// //                float n = stroke->calculate_N(t, i, degree);
//                 // cout<<"t: "<<t<<" | N: "<<n<<endl;
//                 // cout<<"ctrl pt: ("<<next.get_x()<<", "<<next.get_y()<<")"<<endl;
// //                next = next * n;
// //                sum = sum + next;
//             }

//             // cout<<"("<<(int)sum.get_x()<<", "<<(int)sum.get_y()<<")"<<endl;
//             vector<vector<float>> circle_points = calc_circ((int)sum.get_x(), (int)sum.get_y(), stroke->get_radius());

//             for (auto circ_point : circle_points){
//                 canvas->setColor(circ_point[1], circ_point[0], stroke->get_color());
//             }
//         }

//         // CONTROL POINTS
//         // for (auto pt : points){
//         //     int x = pt->get_x();
//         //     int y = pt->get_y();
//         //     vector<vector<float>> circle_points = calc_circ(y, x, stroke->get_radius());
//         //     for (auto circ_point : circle_points){
//         //         canvas->setColor(circ_point[0], circ_point[1], stroke->get_color());
//         //     }
//         // }
//     }

    for (auto s : strokes){
        delete s;
    }
}

/** Paint the image in a painted style.
 *  original_image - The image we want to paint.
 *  radii - The brush sizes we are painting with.
 */
Image* paint(Image* original_image, vector<int> radii){
    sort(radii.begin(), radii.end(), greater<int>()); // Descending order
    bool first_layer = true;
    Image* canvas = new Image(width, height, 255);

    for (int brush_size : radii){
        Image ref_image = original_image->blur(brush_size, brush_size);
        ref_image.writeImage(path + "ref.ppm"); //TODO: remove this write
        paint_layer(canvas, &ref_image, brush_size, first_layer);
        if (first_layer) first_layer = false;
    }

    return canvas;
}

/** Build the list of brushes based on the painting parameters.
 *  Basically just doubles the size of the previous brush size.
 */
vector<int> get_brushes(){
    vector<int> brushes;

    brushes.push_back(MIN_BRUSH_SIZE);
    for (int i = 0; i < NUM_BRUSHES - 1; i++){
        brushes.insert(brushes.begin(), brushes[0] * BRUSH_RATIO);
    }

    return brushes;
}

void one_spline(){ //TODO: FIXME: delete this
    Image* c = new Image(500, 500, 255);
    height = c->getHeight();
    width = c->getWidth();
    Stroke s = Stroke(100, 100, 5);
    s.set_color(Color(1.0, 0.0, 0.0));
    s.add_control_point(232,71);
    s.add_control_point(148,294);
    s.add_control_point(310,115);
    s.add_control_point(375,280);
//      s.add_control_point(400,400);
    //  s.add_control_point(140,145);
    //  s.add_control_point(256,324);
    //  s.add_control_point(422,134);
    

    // for (auto pt : s.get_control_points()){
    //     int x = pt->get_x();
    //     int y = pt->get_y();
    //     int r = s.get_radius();
    //     cout<<x<<" | "<<y<<endl;
    //     vector<vector<float>> circle_points = calc_circ(y, x, r);
    //     int si = circle_points.size();
    //     for (auto circ_point : circle_points){
    //         c->setColor(circ_point[0], circ_point[1], Color(1.0, 0.0, 0.0));
    //     }
    // }
    s.draw_stroke(c, SPLINE_DEGREE);


//     int num_ctrl_pts = s.get_control_points().size();
//     vector<float> knots = make_knot_vector(NUM_KNOTS, 3, s.get_control_points().size());
//     // int degree = (T_RESOLUTION - 1) - (num_ctrl_pts - 1) - 1;
//     for (float t = 0.0; t < 1.0; t+= 1.0/1000.0){
//         Vector sum = Vector(0.0, 0.0);

//         for (int i = 0;i < num_ctrl_pts; i++){
//             Vector next = *(s.get_control_points()[i]);
//             float n = s.calculate_N(t, i, 3, knots);
// //            cout<<"t: "<<t<<" | N: "<<n<<endl;
// //            cout<<"ctrl pt: ("<<next.get_x()<<", "<<next.get_y()<<")"<<endl;
//             next = next * n;
//             sum = sum + next;
//         }

//         if ((int)sum.get_x() == 324 && (int)sum.get_y() == 205)
//             cout<<"a"<<endl;
//         cout<<"("<<(int)sum.get_x()<<", "<<(int)sum.get_y()<<")"<<endl;
//         vector<vector<float>> circle_points = calc_circ((int)sum.get_y(), (int)sum.get_x(), s.get_radius());

//         for (auto circ_point : circle_points){
//             c->setColor(circ_point[0], circ_point[1], Color(0.0, 1.0, 0.0));
//         }
//     }

    c->writeImage(path + "spline.ppm");
}

int main(){
    Image* input = new Image(path + "cat0.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << "width: " << width << endl;
    cout << "height: "<< height << endl;
    vector<int> brush_radii = get_brushes();

    Image* canvas = paint(input, brush_radii);
    canvas->writeImage(path + "output.ppm");
    cout<<"COMPLETED WRITE"<<endl;

    // one_spline();

    delete input;
    // delete canvas;
    return 0;
}