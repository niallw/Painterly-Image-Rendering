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
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/";

vector<vector<float>> generate_blank_canvas(){
    vector<vector<float>> diff;

    for (int row = 0; row < height; row++){
        vector<float> temp;
        for (int col = 0; col < width; col++){
            temp.push_back(INT_MAX);
        }
        diff.push_back(temp);
    }

    return diff;
}

vector<float> get_neighbors(vector<vector<float>> diff_map, int row, int col, int grid_size){
    vector<float> neighbors;
    
    for (int i = 0; i < 2*grid_size+1; i++){
        for (int j = 0; j < 2*grid_size+1; j++){
            int neighbor_row = row + i - grid_size/2;
            int neighbor_col = col + j - grid_size/2;

            if (neighbor_row >= 0 && neighbor_row < height && 
                neighbor_col >= 0 && neighbor_col < width){
                    neighbors.push_back(diff_map[neighbor_col][neighbor_row]);
            }
        }
    }

    return neighbors;
}

void paint_layer(Image* canvas, Image* reference_image, int brush_size, bool is_first_layer){
    // vector<Stroke*> strokes;
    int grid_size = GRID_FACTOR * brush_size;
    vector<vector<float>> difference;

    // Build the pointwise difference image
    if (is_first_layer){
        difference = generate_blank_canvas();
    }
    else{
        difference = *canvas - *reference_image;
    }

    // Calculate error in difference map to locate regions we want to paint
    for (int row = 0; row < height; row+=grid_size){
        for (int col = 0; col < width; col+=grid_size){
            vector<float> neighboring_points = get_neighbors(difference, row, col, grid_size);
            float area_error = 0;

            for (int i = 0; i < neighboring_points.size(); i++){
                // cout<< "AAAAAAA: "<<neighboring_points[i]<<endl;
            }

            for (int i = 0; i < neighboring_points.size(); i++) 
                area_error += neighboring_points[i] / pow(grid_size, 2);
            
            if (area_error > THRESHOLD){
                int max_x;
                int max_y;
                int max_i;
                float max = INT_MIN;

                for (int i = 0; i < neighboring_points.size(); i++){
                    if (neighboring_points[i] > max){
                        max = neighboring_points[i];
                        max_x = i / grid_size;
                        max_y = i % grid_size;
                    }
                }

                // Stroke* s = make_stroke();
                // strokes.push_back(s);
            }
        }
    }
}

/**Calculate the 2D Gaussian kernel with the given radius and 
 * standard deviation. Note that the kernel is an approximation,
 * but it's pretty close to the real thing regardless.
 * 
 * radius - radius of the kernel (and brush).
 * sigma - standard deviation of the kernel.
 */
vector<vector<float>> calculate_kernel(int radius, int std_dev){
    float sigma = std_dev;
    int W = 2*radius+1;
    vector<vector<float>> kernel;
    float mean = W/2;
    float sum = 0.0; // For accumulating the kernel values

    // Create the kernel
    for (int x = 0; x < W; ++x) {
        vector<float> temp;
        for (int y = 0; y < W; ++y) {
            // Math to calculate kernel values :)
            temp.push_back(exp(-0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)))
                           / (2 * M_PI * sigma * sigma));

            sum += temp[temp.size() - 1]; // Accumulate the kernel values
        }
        kernel.push_back(temp);
    }

    // Normalize the kernel
    for (int x = 0; x < W; ++x) {
        for (int y = 0; y < W; ++y){
            kernel[x][y] /= sum;
        }
    }

    return kernel;
}

/**Convolve over the image with a Gaussian kernel to apply
 * a Gaussian blur to the image.
 * 
 * input_image - image to blur.
 * radius - radius of the kernel (and brush).
 * sigma - standard deviation of the kernel.
 * TODO: move this to inside the image class
 */
Image* blur_image(Image* input_image, int radius, int std_dev){
    Image* output = new Image(width, height, 255);
    int r = radius;
    vector<vector<float>> kernel = calculate_kernel(r, std_dev);

    // Convolve!!!
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            Color new_val = Color();

            // Weighted sum of neighbors
            for (int i = 0; i < 2*r+1; i++){
                for (int j = 0; j < 2*r+1; j++){
                    int R_DELTA = i-r;
                    int C_DELTA = j-r;
                    float scale = kernel[i][j];
                    Color pixel_color;

                    int new_r = row + R_DELTA;
                    int new_c = col + C_DELTA;

                    // Ignore pixels on the outside FIXME: change this to do mirror?
                    if (new_r >= 0 && new_r < height && new_c >= 0 && new_c < width){
                        pixel_color = input_image->getRGB(new_r, new_c);
                        pixel_color = pixel_color * scale;
                        new_val = new_val + pixel_color;
                    }
                }
            }

            output->addColor(col, row, new_val);
        }
    }

    output->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/1dPeckverison2.ppm");
    return output;
}

Image* paint(Image* original_image, Image* canvas, vector<int> radii){
    sort(radii.begin(), radii.end(), greater<int>()); // Descending order
    Image* output = new Image(width, height, 255);
    bool first_layer = true;

    for (int brush_size : radii){
        Image* reference_image = blur_image(original_image, brush_size, 1);
        paint_layer(canvas, reference_image, brush_size, first_layer);
        if (first_layer) first_layer = false;
    }
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
    input->blur(20);
    // Image* input2 = new Image(path + "man.ppm");
    // height = input2->getHeight();
    // width = input2->getWidth();
    // Image* reference_image = blur_image(input2, 20, 10);
    // cout << height << endl;
    // cout << width << endl;    
    vector<int> brush_radii = get_brushes();

    // Image* t = generate_blank_canvas();
    // t->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/aaa.ppm");

    // Image* canvas = new Image(width, height, 255);
    // canvas = paint(input, canvas, brush_radii);

    // float r = 2;
    // Image blur = Image(width, height, 255);
    // Image blur2 = Image(width, height, 255);
    // float filter_size = (2 * r + 1);
    // for(int w = 0; w < height; w++){
    //     for(int h = 0; h < width; h++){
    //         if(h < r || h > width - r - 1){
    //             blur.setColor(w, h, input->getRGB(w, h));
    //         }
    //         else{
    //             Color c = Color(0.0, 0.0, 0.0);
    //             for(int i = -r; i <=r; i++){
    //                 c = input->getRGB(w, h+i) + c;
    //             }
    //             c = c / filter_size;
    //             blur.setColor(w, h, c);
    //         }
    //     }
    //  }
    
    // for(int w = 0; w < height; w++){
    //     for(int h = 0; h < width; h++){
    //         if(w < r || w > height - r - 1){
    //             blur2.setColor(w, h, blur.getRGB(w,h));
    //         }
    //         else{
    //             Color c = Color(0.0, 0.0, 0.0);
    //             for(int i = -r; i <=r; i++){
    //                 c = blur.getRGB(w+i,h) + c;
    //             }
    //             c = c / filter_size;
    //             blur2.setColor(w, h, c);
    //         }
    //     }
    // }
    // blur2.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/1dPeckverison.ppm");

    // canvas->writeImage(path + "output.ppm");
}