#include <iostream>
#include <vector>
#include <cmath>
#include "Image.hpp"

using namespace std;

// vector<vector<float> > gaussian_filter {{0.01, 0.08, 0.01}, {0.08, 0.64, 0.08}, {0.01, 0.08, 0.01}};
vector<vector<float> > gaussian_filter {{0.0625, 0.125, 0.0625}, {0.125, .25, 0.125}, {0.0625, 0.125, 0.0625}};
// vector<vector<float> > gaussian_filter {{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}};
int height, width;

vector<vector<float>> calculate_kernel(int radius){
    float sigma = 1;
    int W = radius;
    vector<vector<float>> kernel;
    float mean = W/2;
    float sum = 0.0; // For accumulating the kernel values

    // Create the kernel
    for (int x = 0; x < W; ++x) {
        vector<float> temp;
        for (int y = 0; y < W; ++y) {
            temp.push_back(exp(-0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)))
                            / (2 * M_PI * sigma * sigma));

            // Accumulate the kernel values
            sum += temp[temp.size() - 1];
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

vector<float> calculate_1D_kernel(int radius){
    float sigma = 1;
    int W = 2*radius+1;
    vector<float> kernel;
    float mean = W/2;
    float sum = 0.0; // For accumulating the kernel values

    // Create the kernel
    for (int x = 0; x < W; x++) {
        kernel.push_back(exp( -0.5 * (pow((x-mean)/sigma, 2.0)) )
                        / (2 * M_PI * sigma * sigma));

        // cout << "aaa: "<<kernel.back()<<endl<<endl;
        // Accumulate the kernel values
        sum += kernel.back();
    }

    // Normalize the kernel
    for (int x = 0; x < W; x++) {
        kernel[x] /= sum;
    }

    float tsum=0.;
    for (int i = 0; i < W; i++){
        cout << "inside i: "<<kernel[i]<<endl;
        tsum+=kernel[i];
    }
    cout<<"inside sum: "<<tsum<<endl;

    return kernel;
}


Image* blur_image(Image* input_image, int std_dev, int radius){
    Image* output = new Image(width, height, 255);
    int r = radius;
    vector<int> DELTA;
    for (int i = -r; i <= r; i++){
        DELTA.push_back(i);
        cout <<"delta i: "<<DELTA.back()<<endl;
    }

    vector<float> kernel = calculate_1D_kernel(r);
    float sum = 0.;
    for (int i = 0; i < 2*r+1; i++){
        cout << "i: " << kernel[i] << endl;
        sum += kernel[i];
    }
    cout << "sum: "<<sum<<endl<<endl;

    // Vertical blur
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            Color new_val = Color();

            for (int i = 0; i < 2*r+1; i++){
                int new_r = row+DELTA[i];
                float scale = kernel[i];
                Color pixel_colors;

                if (new_r < 0){
                    pixel_colors = output->getRGB(-1*new_r, col);
                }
                else if (new_r >= height){
                    pixel_colors = output->getRGB((height-1) - (new_r%(height-1)), col);
                }
                else{
                    pixel_colors = output->getRGB(new_r, col);
                }
                pixel_colors = pixel_colors * scale;
                new_val = new_val + pixel_colors;
            }

            output->addColor(col, row, new_val);
        }
    }

    // Horizontal blur
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            Color new_val = Color();

            for (int i = 0; i < 2*r+1; i++){
                int new_c = col+DELTA[i];
                float scale = kernel[i];
                Color pixel_colors;

                if (new_c < 0){
                    pixel_colors = input_image->getRGB(row, -1*new_c);
                }
                else if (new_c >= width){
                    pixel_colors = input_image->getRGB(row, (width-1)-(new_c%(width-1)));
                }
                else{
                    pixel_colors = input_image->getRGB(row, new_c);
                }
                pixel_colors = pixel_colors * scale;
                new_val = new_val + pixel_colors;
            }

            output->addColor(col, row, new_val);
        }
    }

    output->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/gauss1d.ppm");
    return output;
}

bool is_neighbor(int r, int c, int h, int w){
    return (r >= 0 && r < h && c >= 0 && c < w);
}

int main(){
    Image* input = new Image("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/gauss1d.ppm");
    height = input->getHeight();
    width = input->getWidth();
    cout << height << endl;
    cout << width << endl;
    Image* output = new Image(width, height, 255);

    output = blur_image(input, 1, 5);
}

// vector<vector<float>> test = calculate_kernel(3);
// for(int i = 0; i < 3; i++){
//     for (int j = 0; j < 3; j++){
//         cout << test[i][j] << endl;
//     }
// }

// for (int row = STEP_SIZE; row < input->getHeight(); row++){
//     for (int col = STEP_SIZE; col < input->getWidth(); col++){
//         Color new_val = Color();
//         vector<int> R_DELTA {-1, -1, -1, 0, 0, 0, 1, 1, 1};
//         vector<int> C_DELTA {-1, 0, 1, -1, 0, 1, -1, 0, 1};

//         for (int i = 0; i < R_DELTA.size(); i++){
//             int new_r = row+R_DELTA[i];
//             int new_c = col+C_DELTA[i];

//             if(is_neighbor(new_r, new_c, input->getHeight(), input->getWidth())){
//                 float scale = gaussian_filter[1+R_DELTA[i]][1+C_DELTA[i]];
//                 Color pixel_colors = input->getRGB(new_r, new_c);
//                 Color temp_color = Color(pixel_colors.get_r()*scale, pixel_colors.get_g()*scale, pixel_colors.get_b()*scale);
//                 new_val = new_val + temp_color;
//             }
//         }

//         output->addColor(col, row, new_val);
//     }
// }