//  Image.cpp
//
//  Created by Niall and Jimmy on November 11, 2018.
//  based on code from stack overflow: http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c

#include "Image.hpp"

/*
 Create a new blank image
 */
Image::Image(int w, int h, int m){
    m_width = w;
    m_height = h;
    m_max = m;
    m_image = new Color*[m_height];
    for(int i = 0; i < m_height; i++){
        m_image[i] = new Color[m_width];
        for(int j = 0; j < m_width; j++){
            m_image[i][j] = Color();
        }
    }
}

Image::Image(string file_name){
    FILE* file;
    char buff[16];
    float r, g, b;
    
    file = fopen(file_name.c_str(), "r"); // open file for reading
    
    if(!file){
        fprintf(stderr, "Unable to open file %s", file_name.c_str());
        exit(1);
    }
    
    fscanf(file, "%s%*[^\n]%*c", m_magic_number); //read magic number and white space
    
    if(m_magic_number[0] != 'P' || m_magic_number[1] != '3'){
        printf("Incorrect file type");
        exit(1);
    }
    
    //check for comments
    fscanf(file, "%s", buff);
    while (strncmp(buff, "#", 1) == 0) {
        fscanf(file, "%s%*[^\n]%*c", buff);
    }
    
    if (fscanf(file, "%d %d %d", &m_width, &m_height, &m_max) != 3) {
        fprintf(stderr, "Invalid image size (error loading '%s')\n", file_name.c_str());
        exit(1);
    }
    
    m_image = new Color* [m_height];
    for(int i = 0; i < m_height; i++) {
        m_image[i] = new Color[m_width];
        for(int j = 0; j< m_width; j++){
            if(fscanf(file, "%f %f %f", &r, &g, &b) != 3){
                fprintf(stderr, "Invalid pixel reading\n");
                exit(1);
            }
            m_image[i][j] = Color(r/255, g/255, b/255);
        }
    }
    
    fclose(file);
}

Image::~Image(void){
    for(int i = 0; i < m_height; i++){
        delete [] m_image[i];
    }
    delete [] m_image;
}

void Image::setColor(int h, int w, Color c){
    m_image[h][w] = c;
}

void Image::addColor(int h, int w, Color c){
    m_image[h][w] = m_image[h][w] + c;
    m_image[h][w].clamp();
}

int Image::getWidth(){
    return m_width;
}

int Image::getHeight(){
    return m_height;
}

Color** Image::getImage(){
    return m_image;
}

Color Image::getRGB(int r, int c){
    return m_image[r][c];
}

vector<vector<float>> Image::operator-(Image i){
    vector<vector<float>> difference;

    for (int row = 0; row < m_width; row++){
        vector<float> temp;
        for (int col = 0; col < m_height; col++){
            float r_1 = m_image[row][col].get_r();
            float g_1 = m_image[row][col].get_g();
            float b_1 = m_image[row][col].get_b();

            float r_2 = i.getRGB(row, col).get_r();
            float g_2 = i.getRGB(row, col).get_g();
            float b_2 = i.getRGB(row, col).get_b();

            float new_r = pow((r_1 - r_2), 2);
            float new_g = pow((g_1 - g_2), 2);
            float new_b = pow((b_1 - b_2), 2);
            temp.push_back(sqrt(new_r + new_g + new_b));
        }
        difference.push_back(temp);
    }

    return difference;
}

void Image::writeImage(string file_name){
    FILE* file;
    file = fopen(file_name.c_str(), "w");
    if(!file){
        fprintf(stderr, "Unable to open file %s", file_name.c_str());
        exit(1);
    }
    
    // your code goes here
    fprintf(file, "P3\n");
    fprintf(file, "# CREATORS: Niall Williams and Jimmy Plaut\n");
    
    fprintf(file, "%d %d\n%d\n", m_width, m_height, m_max);
    for(int i = 0; i < m_height; i++){
        for(int j = 0; j < m_width; j++){
            Color c = m_image[i][j]*255;
            fprintf(file, "%d %d %d\n", (int)c.get_r(), (int)c.get_g(), (int)c.get_b() );
        }
    }
    
    fclose(file);
    
}

/**Calculate the 2D Gaussian kernel with the given radius and 
 * standard deviation. Note that the kernel is an approximation,
 * but it's pretty close to the real thing regardless.
 * 
 * radius - radius of the kernel (and brush).
 * sigma - standard deviation of the kernel.
 */
vector<vector<float>> Image::calculate_kernel(int radius, int std_dev){
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
 * TODO: make this work with 2 1D kernels
 */
Image* Image::blur(int radius, int std_dev){
    Image* output = new Image(m_width, m_height, 255);
    int r = radius;
    vector<vector<float>> kernel = calculate_kernel(r, std_dev);

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
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
                    if (new_r >= 0 && new_r < m_height && new_c >= 0 && new_c < m_width){
                        pixel_color = m_image[new_r][new_c];
                        pixel_color = pixel_color * scale;
                        new_val = new_val + pixel_color;
                    }
                }
            }

            output->addColor(row, col, new_val);
        }
    }

    // TODO: remove this writeImage call
    output->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/test22.ppm");
    return output;
}

Image* Image::grayscale(){
    Image* gray = new Image(m_width, m_height, 255);

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            float luminance = m_image[row][col].get_r() * 0.3  + 
                              m_image[row][col].get_g() * 0.59 + 
                              m_image[row][col].get_b() * 0.11;
            Color temp = Color(luminance, luminance, luminance);
            gray->setColor(row, col, temp);
        }
    }

    // TODO: remove this writeImage call
    gray->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/gray.ppm");
    return gray;
}

//FIXME: current bug: the output image is quite noisy. Much more so than the GIMP output.
vector<Image*> Image::sobel(){
    vector<Image*> filtered_images;
    Image* grayscale = this->grayscale();
    Image* combined = new Image(m_width, m_height, 255);
    Image* x = new Image(m_width, m_height, 255);
    Image* y = new Image(m_width, m_height, 255);

    int kernel_1[8] = {1, 0, -1, 2, -2, 1, 0, -1};
    int kernel_2[8] = {1, 2, 1, 0, 0, -1, -2, -1};
    int r_delta[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int c_delta[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color x_c = Color();
            Color y_c = Color();

            for (int i = 0; i < 8; i++){
                int new_r = row + r_delta[i];    
                int new_c = col + c_delta[i];    

                if (new_r >= 0 && new_r < m_height && new_c >= 0 && new_c < m_width){
                    x_c = x_c + (grayscale->getRGB(new_r, new_c) * kernel_1[i]);
                    y_c = y_c + (grayscale->getRGB(new_r, new_c) * kernel_2[i]);
                }
            }
            
            x_c = Color(abs(x_c.get_r()), abs(x_c.get_g()), abs(x_c.get_b()));
            y_c = Color(abs(y_c.get_r()), abs(y_c.get_g()), abs(y_c.get_b()));
            x_c.clamp();
            y_c.clamp();
            x_c = x_c / 8;
            y_c = y_c / 8;
            x->setColor(row, col, x_c);
            y->setColor(row, col, y_c);
            combined->setColor(row, col, (x_c) + (y_c));
        }
    }

    // TODO: remove this writeImage call
    combined->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_final.ppm");
    x->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_xsobel.ppm");
    y->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_ysobel.ppm");

    filtered_images.push_back(combined);
    filtered_images.push_back(x);
    filtered_images.push_back(y);
    return filtered_images;
}

    //TODO: FIXME: remove these comments. I kept them in case we revisit to debug the 1d kernel version.
    // ================================================================================
    // THis is the 2D kernel version. it's slower, but it actually works.

    // Image* out = new Image(m_width, m_height, 255);
    // Image* x = new Image(m_width, m_height, 255);
    // Image* y = new Image(m_width, m_height, 255);

    // int kernel1[8] = {1, 0, -1, 2, -2, 1, 0, -1};
    // int kernel2[8] = {1, 2, 1, 0, 0, -1, -2, -1};

    // for (int row = 0; row < m_height; row++){
    //     for (int col = 0; col < m_width; col++){
    //         int r_delta[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    //         int c_delta[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    //         Color x_c = Color();
    //         Color y_c = Color();

    //         for (int i = 0; i < 8; i++){
    //             int new_r = row + r_delta[i];    
    //             int new_c = col + c_delta[i];    

    //             if (new_r >= 0 && new_r < m_height && new_c >= 0 && new_c < m_width){
    //                 x_c = x_c + (m_image[new_r][new_c] * kernel1[i]);
    //                 y_c = y_c + (m_image[new_r][new_c] * kernel2[i]);
    //             }
    //         }
    //         x_c = Color(abs(x_c.get_r()), abs(x_c.get_g()), abs(x_c.get_b()));
    //         y_c = Color(abs(y_c.get_r()), abs(y_c.get_g()), abs(y_c.get_b()));
    //         x_c.clamp();
    //         y_c.clamp();
    //         x_c = x_c / 8;
    //         y_c = y_c / 8;
    //         x->setColor(row, col, x_c);
    //         y->setColor(row, col, y_c);
    //         out->setColor(row, col, (x_c) + (y_c));
    //     }
    // }

    // out->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_final.ppm");
    // x->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_xsobel.ppm");
    // y->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_ysobel.ppm");


    // ================================================================================

    // this is the 2 1D kernel version. it's faster, but it doesn't work properly lol

    // Image* x_sobel = new Image(m_width, m_height, 255);
    // Image* y_sobel_temp = new Image(m_width, m_height, 255);
    // Image* y_sobel = new Image(m_width, m_height, 255);

    // // Horizontal filter
    // for (int row = 0; row < m_height; row++){
    //     for (int col = 0; col < m_width; col++){
    //         Color new_x_color = Color();
    //         Color new_y_color = Color();

    //         if (col - 1 >= 0){
    //             new_x_color = new_x_color + (this->m_image[row][col-1] * sobel_1[0]);
    //             new_y_color = new_y_color + (this->m_image[row][col-1] * sobel_2[0]);
    //         }
    //         new_y_color = new_y_color + (this->m_image[row][col] * sobel_2[1]);
    //         if (col + 1 < m_width){
    //             new_x_color = new_x_color + (this->m_image[row][col+1] * sobel_1[2]);
    //             new_y_color = new_y_color + (this->m_image[row][col+1] * sobel_2[2]);
    //         }

    //         new_x_color = Color(abs(new_x_color.get_r()), abs(new_x_color.get_b()), abs(new_x_color.get_g()));
    //         new_y_color = Color(abs(new_y_color.get_r()), abs(new_y_color.get_b()), abs(new_y_color.get_g()));
    //         new_x_color.clamp();
    //         x_sobel->setColor(row, col, new_x_color);
    //         new_y_color.clamp();
    //         y_sobel_temp->setColor(row, col, new_y_color);
    //     }
    // }

    // // Vertical filter
    // for (int row = 0; row < m_height; row++){
    //     for (int col = 0; col < m_width; col++){
    //         Color* new_x_color = new Color();
    //         Color* new_y_color = new Color(0.0, 0.0, 0.0);

    //         if (row - 1 >= 0){
    //             *new_x_color = *new_x_color + (x_sobel->getRGB(row-1, col) * sobel_2[0]);
    //             *new_y_color = *new_y_color + (y_sobel_temp->getRGB(row-1, col) * sobel_1[0]);
                
    //         }
    //         *new_x_color = *new_x_color + (x_sobel->getRGB(row, col) * sobel_2[1]);
    //         if (row + 1 < m_height){
    //             *new_x_color = *new_x_color + (x_sobel->getRGB(row+1, col) * sobel_2[2]);
    //             *new_y_color = *new_y_color + (y_sobel_temp->getRGB(row+1, col) * sobel_1[2]);
    //         }
    //         *new_x_color = Color(abs(new_x_color->get_r()), abs(new_x_color->get_b()), abs(new_x_color->get_g()));
    //         *new_y_color = Color(abs(new_y_color->get_r()), abs(new_y_color->get_b()), abs(new_y_color->get_g()));
    //         new_x_color->clamp();
    //         *new_x_color = *new_x_color / 6;
    //         x_sobel->setColor(row, col, *new_x_color);
    //         new_y_color->clamp();
    //         *new_y_color = *new_y_color / 6;
    //         y_sobel->setColor(row, col, *new_y_color);
    //     }
    // }

    // Image* final_img = new Image(m_width, m_height, 255);
    // for (int row = 0; row < m_height; row++){
    //     for (int col = 0; col < m_width; col++){
    //         Color temp = (x_sobel->getRGB(row, col) * 0.5) + (y_sobel->getRGB(row, col) * 0.5);
    //         final_img->setColor(row, col, temp);
    //     }
    // }

    // final_img->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/sobel cat.ppm");
    // x_sobel->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/xsobel.ppm");
    // y_sobel->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/ysobel.ppm");
