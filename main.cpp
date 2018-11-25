#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <climits>
#include <numeric>

#include "Image.hpp"
#include "Stroke.hpp"

using namespace std;

int height, width;
const int GRID_FACTOR = 2;
const int MIN_BRUSH_SIZE = 2;
const int BRUSH_RATIO = 2/1;
const int NUM_BRUSHES = 3;
const float THRESHOLD = 0.2;
const float CURVATURE_FILTER = 1;
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/";
vector<Image*> sobel_filters;
Image* sobel_x, *sobel_y;

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

Stroke* make_stroke(int x, int y, int brush_size, Image* ref_image, Image* canvas){
    Stroke* stroke = new Stroke(x, y, brush_size, ref_image);
    Color stroke_color = stroke->get_color();
    int cur_x = x;
    int cur_y = y;
    float last_Dx = 0;
    float last_Dy = 0;

    for (int i = 0; i <= MAX_STROKE_LENGTH; i++){ //TODO: make i start at 1 and go <= ?
        Color ref_image_color = ref_image->getRGB(cur_x, cur_y);
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
//        float denom = sqrt(d_x * d_x + d_y * d_y); // Just do it once because it's expensive
//        d_x = ((CURVATURE_FILTER * d_x) + ((1-CURVATURE_FILTER * d_x) * last_Dx)) / denom;
//        d_y = ((CURVATURE_FILTER * d_y) + ((1-CURVATURE_FILTER * d_y) * last_Dy)) / denom;
        d_x = CURVATURE_FILTER * (d_x) + (1 - CURVATURE_FILTER) * last_Dx;
        d_y = CURVATURE_FILTER * (d_y) + (1 - CURVATURE_FILTER) * last_Dy;
        d_x = d_x / sqrt(d_x * d_x + d_y * d_y);
        d_y = d_y / sqrt(d_x * d_x + d_y * d_y); //TODO: these are different. cuz dx changes for the y calc
        if (d_x < 0) d_x = 0;
        if (d_y < 0) d_y = 0;
        cur_x = cur_x + brush_size * d_x;
        cur_y = cur_y + brush_size * d_y;
        last_Dx = d_x;
        last_Dy = d_y;

        stroke->add_control_point(cur_x, cur_y);
    }

    return stroke;
}

void paint_layer(Image* canvas, Image* ref_image, int brush_size, bool is_first_layer){
    vector<Stroke*> strokes;
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
                float max = INT_MIN;

                for (int i = 0; i < neighboring_points.size(); i++){
                    if (neighboring_points[i][2] > max){
                        max_row = neighboring_points[i][0];
                        max_col = neighboring_points[i][1];
                        max = neighboring_points[i][2];
                    }
                }

                Stroke* s = make_stroke(max_row, max_col, brush_size, ref_image, canvas);
                strokes.push_back(s);
//                cout<<"Stroke #"<<strokes.size()<<": "<<*s<<endl<<endl;
            }
        }
    }

    cout <<"FREEDOM"<<endl;
}

Image* paint(Image* original_image, vector<int> radii){
    sort(radii.begin(), radii.end(), greater<int>()); // Descending order
    bool first_layer = true;
    Image* canvas = new Image(width, height, 255);

    for (int brush_size : radii){
        Image* ref_image = original_image->blur(brush_size, 1);
        paint_layer(canvas, ref_image, brush_size, first_layer);
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
    Image* input = new Image(path + "man.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << "width: " << width << endl;
    cout << "height: "<< height << endl;
    vector<int> brush_radii = get_brushes();

    sobel_filters = input->sobel();
    sobel_x = sobel_filters[1];
    cout<<sobel_x->getRGB(4,4).get_b()<<endl;
    sobel_y = sobel_filters[2];

    Image* canvas = paint(input, brush_radii);
    // input->blur(2, 3);
}