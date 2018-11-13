#include <iostream>
#include <vector>
#include <cmath>
#include "Image.hpp"

using namespace std;

int height, width;

/**Calculate the 2D Gaussian kernel with the given radius and 
 * standard deviation.
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

    output->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/man_blur_high_stddev.ppm");
    return output;
}

bool is_neighbor(int r, int c, int h, int w){
    return (r >= 0 && r < h && c >= 0 && c < w);
}

int main(){
    Image* input = new Image("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/man.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << height << endl;
    cout << width << endl;
    Image* output;

    output = blur_image(input, 5, 1000);
}