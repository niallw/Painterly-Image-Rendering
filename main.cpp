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
const float GRID_FACTOR = 0.5;
const int MIN_BRUSH_SIZE = 2;
const int BRUSH_RATIO = 2/1;
const int NUM_BRUSHES = 1;
const float THRESHOLD = 100.0;
const float CURVATURE_FILTER = 1.0;
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/";
auto rng = default_random_engine {};

vector<vector<float>> generate_blank_canvas(){
    vector<vector<float>> diff;

    //TODO: I swapped width and height in the for loops cuz seg fault. i'm not sure if it was correct before or after swapping.
    for (int row = 0; row < width; row++){
        vector<float> temp;
        for (int col = 0; col < height; col++){
            temp.push_back(INTMAX_MAX);
        }
        diff.push_back(temp);
    }

    return diff;
}

vector<vector<float>> get_neighbors(vector<vector<float>> diff_map, int row, int col, int grid_size){
    vector<vector<float>> neighbors;

    for (int i = -(grid_size/2); i <= grid_size/2; i++){
        for (int j = -(grid_size/2); j <= grid_size/2; j++){
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            if (neighbor_row >= 0 && neighbor_row < diff_map.size() &&
                neighbor_col >= 0 && neighbor_col < diff_map[0].size()){
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

Stroke* make_stroke(int x, int y, int brush_size, Image* ref_image,
                    Image* canvas, Image* sobel_x, Image* sobel_y){
    Stroke* stroke = new Stroke(x, y, brush_size);
    Color stroke_color = ref_image->getRGB(x, y);
    stroke->set_color(stroke_color);
    int cur_x = x;
    int cur_y = y;
    float last_Dx = 0;
    float last_Dy = 0;

    for (int i = 1; i <= MAX_STROKE_LENGTH; i++){ //TODO: make i start at 1 and go <= ? thats what the paper has
        Color ref_image_color = Color(ref_image->getRGB(cur_x, cur_y).get_r(),
                                      ref_image->getRGB(cur_x, cur_y).get_g(),
                                      ref_image->getRGB(cur_x, cur_y).get_b());
        Color canvas_color = canvas->getRGB(cur_x, cur_y);

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

vector<vector<float>> calc_circ(int c_x, int c_y, int r){
    vector<vector<float>> points;
//    if (c_x > 200)
//        cout << c_x << endl;

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
//                cout << x << endl;

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
                if (max_row == 116 && max_col == 235)
                    cout<<"E"<<endl;

                Stroke* s = make_stroke(max_row, max_col, brush_size, ref_image,
                                        canvas, &sobel_x, &sobel_y);
                strokes.push_back(s);
                // cout<<"Stroke #"<<strokes.size()<<": "<<*s<<endl<<endl;
            }
        }
    }

    shuffle(strokes.begin(), strokes.end(), rng);
    for (auto stroke : strokes){
        auto points = stroke->get_control_points();
//        cout<<points.size()<<endl;
        for (auto pt : points){
            int x = pt->get_x();
            int y = pt->get_y();
            vector<vector<float>> circle_points = calc_circ(x, y, stroke->get_radius());
            for (auto circ_point : circle_points){
                canvas->setColor(circ_point[0], circ_point[1], stroke->get_color());
            }
        }
    }

    for (auto s : strokes){
        delete s;
    }
}

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

vector<int> get_brushes(){
    vector<int> brushes;

    brushes.push_back(MIN_BRUSH_SIZE);
    for (int i = 0; i < NUM_BRUSHES - 1; i++){
        brushes.insert(brushes.begin(), brushes[0] * BRUSH_RATIO);
    }

    return brushes;
}

int main(){
    Image* input = new Image(path + "filed cat.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << "width: " << width << endl;
    cout << "height: "<< height << endl;
    vector<int> brush_radii = get_brushes();

    Image* canvas = paint(input, brush_radii);
    canvas->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/output.ppm");
    cout<<"COMPLETED WRITE"<<endl;

    delete input;
    // delete canvas;
    return 0;
}