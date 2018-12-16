// Program to render images in the style of a painted image
// Authors: James Plaut and Niall Williams

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
#include "Vector.hpp"

using namespace std;

int layer_count = 1;
int height, width;
const float GRID_FACTOR = 1.0;       // Scale factor for the error area grid size.
const float GAUSSIAN_FACTOR = 1;     // Scale factor for the error area grid size.
const int MIN_BRUSH_SIZE = 2;        // Radius of the smallest brush.
const int BRUSH_RATIO = 2/1;         // Scale factor for calculating next brush size.
const int NUM_BRUSHES = 3;           // Number of brushes we are going to paint with.
const float THRESHOLD = 0.35;        // Error threshold when determining whether to paint or not.
const float CURVATURE_FILTER = 10;  // Determines if we exagerrate or reduce the stroke curvature.
const int SPLINE_DEGREE = 3;         // Cubic spline
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/"; // Path to directory of image.
auto rng = default_random_engine {};

vector<int> get_brushes();
Image* paint(Image*, vector<int>);
void paint_layer(Image*, Image*, int, bool);
vector<vector<float>> generate_blank_canvas();
vector<Vector> get_neighbors(int, int, int);
Stroke* make_stroke(int, int, int, Image*, Image*, Image*, Image*);

int main(){
    Image* input = new Image(path + "family4.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << "Width: " << width << endl;
    cout << "Height: "<< height << endl;
    vector<int> brush_radii = get_brushes();
    
    Image* canvas = paint(input, brush_radii);
    canvas->writeImage(path + "output.ppm");
    cout<<"COMPLETED WRITE"<<endl;

    delete input;
    delete canvas;
    return 0;
}

/**Build the list of brushes based on the painting parameters.
 * Basically just doubles the size of the previous brush.
 */
vector<int> get_brushes(){
    vector<int> brushes;

    brushes.push_back(MIN_BRUSH_SIZE);
    for (int i = 0; i < NUM_BRUSHES - 1; i++){
        brushes.insert(brushes.begin(), brushes[0] * BRUSH_RATIO);
    }

    return brushes;
}

/**Paint the image in a painted style.
 * original_image - The image we want to paint.
 * radii - The brush sizes we are painting with.
 */
Image* paint(Image* original_image, vector<int> radii){
    sort(radii.begin(), radii.end(), greater<int>()); // Descending order
    bool first_layer = true;
    Image* canvas = new Image(width, height, 255);

    for (int brush_size : radii){
        cout<<"Painting with brush size "<<brush_size<<endl;
        Image ref_image = original_image->blur(brush_size, GAUSSIAN_FACTOR * brush_size);
        paint_layer(canvas, &ref_image, brush_size, first_layer);
        if (first_layer) first_layer = false;
    }

    return canvas;
}

/**Paint one layer of the image with the specified brush size. The painting
 * is based off a Gaussian blurred image with a standard deviation based on
 * the brush size.
 * cavas - The image cavas we are painting onto.
 * ref_image - The blurred image we are recreating with brush strokes.
 * brush_size - The current brush size we are painting with.
 * is_first_layer - Flag that is used to know which point-wise pixel color difference
 *                  map to use. On the first layer, the canvas is empty so we need a
 *                  difference map that always forces us to draw strokes.
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

    // Find the areas of the difference map that have the largest error
    // to locate regions we want to paint
    for (int row = 0; row < height; row+=grid_size){
        for (int col = 0; col < width; col+=grid_size){
            vector<Vector> neighboring_points = get_neighbors(row, col, grid_size);
            float area_error = 0;

            for (Vector neighbor : neighboring_points)
                area_error += difference[neighbor.get_y()][neighbor.get_x()];
            area_error /=  pow(grid_size, 2);

            // We want to paint on this area
            if (area_error > THRESHOLD){
                int max_row;
                int max_col;
                float max = INTMAX_MIN;

                // Find the largest error point in the neighborhood
                for (Vector neighbor : neighboring_points){
                    if (difference[neighbor.get_y()][neighbor.get_x()] > max){
                        max_row = neighbor.get_y();
                        max_col = neighbor.get_x();
                        max = difference[max_row][max_col];
                    }
                }

                Stroke* s = make_stroke(max_row, max_col, brush_size, ref_image,
                                        canvas, &sobel_x, &sobel_y);
                strokes.push_back(s);
            }
        }
    }

    // Draw each stroke. Shuffle the stroke order so that we don't
    // get a "cascade" of strokes from the bottom right corner up to
    // the top left corner.
    cout<<"   Number of strokes: "<<strokes.size()<<endl;
    shuffle(strokes.begin(), strokes.end(), rng);
    for (Stroke* s : strokes){
        s->draw_stroke(canvas, SPLINE_DEGREE);
    }
    canvas->writeImage(path + "layer" + to_string(layer_count) + ".ppm");
    layer_count++;

    for (Stroke* s : strokes){
        delete s;
    }
}

/**Creates a difference map that always returns max int which will guarantee
 * that we paint strokes. The difference map error (error between the reference image
 * and our canvas) is always the max value with the difference map created by this
 * function, which means the error is always above the threshold, so we always paint.
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

/**Finds all pixels that are neighbors of a given pixel.
 * diff_map - The point-wise color difference map. AKA the pixel values that
 *            represent the color difference between the reference image and
 *            our current canvas.
 * row - y-coordinate of the pixel whose neighbors we want to find.
 * col - x-coordinate of the pixel whose neighbors we want to find.
 * grid_size - The size of the "window" we sample the image with. This is based on
 *             the brush size.
 */
vector<Vector> get_neighbors(int row, int col, int grid_size){
    vector<Vector> neighbors;

    for (int i = -(grid_size/2); i <= grid_size/2; i++){
        for (int j = -(grid_size/2); j <= grid_size/2; j++){
            int neighbor_row = row + i;
            int neighbor_col = col + j;

            if (neighbor_row >= 0 && neighbor_row < height &&
                neighbor_col >= 0 && neighbor_col < width){
                Vector temp = Vector(neighbor_col, neighbor_row);
                neighbors.push_back(temp);
            }
        }
    }

    return neighbors;
}

/**Creates a stroke. This is done by calculating a series of control points
 * that define a B-spline which will represent our stroke. The control points
 * are placed perpendicular to the image gradient.
 * x - x-coordinate of the first control point.
 * y - y-coordinate of the first control point.
 * brush_size - The size of the brush we are painting with.
 * ref_image - The blurred image we are painting.
 * canvas - The canvas image we are painting onto.
 * sobel_x - The horizontal gradient of the reference image.
 * sobel_y - The vertical gradient of the reference image.
 */
Stroke* make_stroke(int y, int x, int brush_size, Image* ref_image,
                    Image* canvas, Image* sobel_x, Image* sobel_y){
    Stroke* stroke = new Stroke(x, y, brush_size);
    Color stroke_color = ref_image->getRGB(y, x);
    stroke->set_color(stroke_color);
    Vector cur_point = Vector(x, y);
    Vector last_direction = Vector(0.0, 0.0);

    for (int i = 1; i <= MAX_STROKE_LENGTH; i++){
        Color ref_image_color = Color(ref_image->getRGB(cur_point.get_y(), cur_point.get_x()).get_r(),
                                      ref_image->getRGB(cur_point.get_y(), cur_point.get_x()).get_g(),
                                      ref_image->getRGB(cur_point.get_y(), cur_point.get_x()).get_b());
        Color canvas_color = canvas->getRGB(cur_point.get_y(), cur_point.get_x());

        if ((i > MIN_STROKE_LENGTH) &&
            (abs(ref_image_color - canvas_color) < abs(ref_image_color - stroke_color))){
            return stroke;
        }

        // Detect vanishing gradient
        float gradient_mag = sqrt(pow(sobel_x->getRGB(cur_point.get_y(), cur_point.get_x()).get_r(), 2) +
                                  pow(sobel_y->getRGB(cur_point.get_y(), cur_point.get_x()).get_r(), 2));
        if (gradient_mag == 0){
            return stroke;
        }

        // Get unit vector of gradient
        float theta = atan(sobel_y->getRGB(cur_point.get_y(), cur_point.get_x()).get_r() /
                           sobel_x->getRGB(cur_point.get_y(), cur_point.get_x()).get_r());
        float g_x = cos(theta);
        float g_y = sin(theta);
        Vector temp = Vector(g_x, g_y);
        temp = temp.normalize();
        // Compute a normal to the gradient
        Vector direction = Vector(-(temp.get_y()), temp.get_x());

        // Reverse normal if necessary
        if (last_direction.get_x() * direction.get_x() + 
            last_direction.get_y() * direction.get_y() < 0){
            direction = direction * -1;
        }

        // Filter the stroke direction
        direction = (direction * CURVATURE_FILTER) + (last_direction * (1 - CURVATURE_FILTER));
        float denom = sqrt(direction.get_x() * direction.get_x() + // Just do it once because it's expensive
                           direction.get_y() * direction.get_y()); 
        direction = direction / denom;
        cur_point = cur_point + (direction * brush_size);

        // Clamp so that we don't paint outside the canvas. That would make a mess :)
        cur_point.clamp(width, height);

        last_direction.set_x(direction.get_x());
        last_direction.set_y(direction.get_y());

        stroke->add_control_point(cur_point.get_x(), cur_point.get_y());
    }

    return stroke;
}
