#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

#include "Image.hpp"
#include "Stroke.hpp"

using namespace std;

int height, width;
string path = "/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/";

int main(){
    Image* input = new Image(path + "man.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << height << endl;
    cout << width << endl;
    vector<int> brush_radii {2, 3};

    Image* canvas = paint(input, canvas, brush_radii);

    canvas->writeImage(path + "output.ppm");
}

Image* paint(Image* original_image, Image* canvas, vector<int> radii){
    sort(radii.begin(), radii.end(), greater<int>()); // Descending order
    Image* output;

    for (int brush_size : radii){
        Image* reference_image = blur_image(original_image, brush_size, 1);
        paint_layer(canvas, reference_image, brush_size);
    }
}

void paint_layer(Image* canvas, Image* reference_image, int brush_size){
    // vector<Stroke*> strokes;
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

    // output->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/man_blur_high_stddev.ppm");
    return output;
}