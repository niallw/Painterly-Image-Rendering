#include <iostream>
#include <vector>
#include "ImageIO.h"
#include "Image.hpp"

using namespace std;

ImageIO* img;
vector<vector<float> > gaussian_filter {{0.01, 0.08, 0.01}, {0.08, 0.64, 0.08}, {0.01, 0.08, 0.01}};

bool isNeighbor(int r, int c, int h, int w){
    return (r >= 0 && r < h && c >= 0 && c < w);
}

int main(){
    img = new ImageIO("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly/images/test.ppm");
    Image output = Image(img->getWidth(), img->getHeight(), 255);

    cout << img->getHeight() << endl;
    cout << img->getWidth() << endl;

    int STEP_SIZE = 2;

    for (int row = STEP_SIZE; row < img->getHeight(); row++){
        for (int col = STEP_SIZE; col < img->getWidth(); col++){
            // Color new_val = Color(0.0, 0.0, 0.0);
            float* orig_colors = img->getRGB(row, col);
            Color new_val = Color(orig_colors[0], orig_colors[1], orig_colors[2]);

            vector<int> R_DELTA {-1, -1, -1, 0, 0, 1, 1, 1};
            vector<int> C_DELTA {-1, 0, 1, -1, 1, -1, 0, 1};

            for (int i = 0; i < R_DELTA.size(); i++){
                int new_r = row+R_DELTA[i];
                int new_c = col+C_DELTA[i];
                if(isNeighbor(new_r, new_c, img->getHeight(), img->getWidth())){
                    float scale = gaussian_filter[1+R_DELTA[i]][1+C_DELTA[i]];
                    float* pixel_colors = img->getRGB(new_r, new_c);
                    Color temp_color = Color(pixel_colors[0]*scale, pixel_colors[1]*scale, pixel_colors[2]*scale);
                    new_val = new_val + temp_color;
                }
            }

            output.addColor(col, row, new_val);
        }
    }
    output.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly/images/gauss.ppm");
}