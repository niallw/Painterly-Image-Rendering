#include <iostream>
#include "ImageIO.h"

using namespace std;

ImageIO* img;

int main(){
    img = new ImageIO("test.ppm");
    float*** test = img->getImage();
    cout << test[0][0][2] << endl;

    // std::string filename = "test.ppm";
    // std::ifstream file(filename.c_str());
    // if(file.is_open()) {
    //     std::string line;
    //     for (int i=0;i<4;i++) { std::getline(file,line); }
    //     RGB rgb;
    //     for (int i=0;i<LINES_TO_READ;i++) {
    //         file >> rgb;
    //         std::cout << rgb;
    //     }
    // }
}