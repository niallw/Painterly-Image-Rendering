//  Image.cpp
//
//  Edited by Niall and Jimmy on November 11, 2018.
//  Created by Peck, Tabitha on 9/18/14.
//  based on code from stack overflow: http://stackoverflow.com/questions/2693631/read-ppm-file-and-store-it-in-an-array-coded-with-c

#include "Image.hpp"

/**Construct a new, empty image object.
 * 
 * w - Width of the image.
 * h - Height of the image.
 * m - Max color channel value of the image (255 for normal RGB).
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

/**Construct an image object from a .ppm file.
 * 
 * file_name - Name of the .ppm file.
 */
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

/**Destructor. This gives a seg fault for some reason.
 */
Image::~Image(void){
    //TODO: fix this. destructor is giving seg fault
//    for(int i = 0; i < m_height; i++){
//        delete [] m_image[i];
//    }
//    delete [] m_image;
}

/**Sets the color of the pixel at position (w, h).
 * 
 * h - Row of pixel.
 * w - Column of pixel.
 * c - Color to set the pixel (w, h) to.
 */
void Image::setColor(int h, int w, Color c){
    m_image[h][w] = c;
}

/**Add a color to the color currently at pixel (w, h).
 * 
 * h - Row of pixel.
 * w - Column of pixel.
 * c - Color to add to pixel (w, h).
 */
void Image::addColor(int h, int w, Color c){
    m_image[h][w] = m_image[h][w] + c;
    m_image[h][w].clamp();
}

/**Return the image's width.
 */
int Image::getWidth(){
    return m_width;
}

/**Return the image's height.
 */
int Image::getHeight(){
    return m_height;
}

/**Return the image's color array.
 */
Color** Image::getImage(){
    return m_image;
}

/**Return the color at pixel (c, r).
 * 
 * r - Row of pixel.
 * c - Column of pixel.
 */
Color Image::getRGB(int r, int c){
//    if (r == 124)
//        this->writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER "
//                         "GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/bug.ppm");
    return m_image[r][c];
}

/**Subtract two images. This gives us the Euclidian distance between
 * two pixels.
 * 
 * i - Other image we are subtracting from this image.
 */
vector<vector<float>> Image::operator-(Image i){
    vector<vector<float>> difference;

    for (int row = 0; row < m_height; row++){
        vector<float> temp;
        for (int col = 0; col < m_width; col++){
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

/**Write the image object to a .ppm file.
 * 
 * file_name - Name of the output file.
 */
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
vector<float> Image::calculate_kernel(int radius, int std_dev){
    float sigma = std_dev;
    int W = 2*radius+1;
    vector<float> kernel;
    float mean = W/2;
    float sum = 0.0; // For accumulating the kernel values

    // // Create the kernel
    // for (int x = 0; x < W; ++x) {
    //     vector<float> temp;
    //     for (int y = 0; y < W; ++y) {
    //         // Math to calculate kernel values :)
    //         temp.push_back(exp(-0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)))
    //                        / (2 * M_PI * sigma * sigma));

    //         sum += temp[temp.size() - 1]; // Accumulate the kernel values
    //     }
    //     kernel.push_back(temp);
    // }

    // Create the kernel
    for (int x = 0; x < W; ++x) {
        // for (int y = 0; y < W; ++y) {
            // Math to calculate kernel values :)
            kernel.push_back(exp(-0.5 * (pow((x-mean)/sigma, 2.0)))
                           / sqrt(2 * M_PI * sigma * sigma));

            sum += kernel[kernel.size() - 1]; // Accumulate the kernel values
        // }
    }

    // Normalize the kernel
    for (int x = 0; x < W; ++x) {
        // for (int y = 0; y < W; ++y){
            kernel[x] /= sum;
        // }
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
Image Image::blur(int radius, int std_dev){
    Image output = Image(m_width, m_height, 255);
    int r = radius;
    vector<float> kernel = calculate_kernel(r, std_dev);

    // Horizontal
    for (int row = 0; row < m_height; row++){
        for (int col = 0; col <m_width; col++){
            Color new_val = Color();

            for (int i = 0; i < kernel.size(); i++){
                int c_delta = i-r;
                float scale = kernel[i];
                Color pixel_color;

                int new_c = col + c_delta;

                if (new_c >= 0 && new_c < m_width){
                    pixel_color = m_image[row][new_c];
                    pixel_color = pixel_color * scale;
                    new_val = new_val + pixel_color;
                }
            }

            new_val.clamp();
            output.setColor(row, col, new_val);
        }
    }

    // Vertical
    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color new_val = Color();

            for (int i = 0; i < kernel.size(); i++){
                int r_delta = i-r;
                float scale = kernel[i];
                Color pixel_color;

                int new_r = row + r_delta;

                if (new_r >= 0 && new_r < m_height){
                    pixel_color = output.getRGB(new_r, col);
                    pixel_color = pixel_color * scale;
                    new_val = new_val + pixel_color;
                }
            }

            new_val.clamp();
            output.setColor(row, col, new_val);
        }
    }

    output.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/Previous Classes/2018 Fall/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/blur.ppm");
    return output;
}

/**Converts an image to just its luminance channels (grayscale).
 */
Image Image::grayscale(){
    Image gray = Image(m_width, m_height, 255);

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            float luminance = m_image[row][col].get_r() * 0.3  + 
                              m_image[row][col].get_g() * 0.59 + 
                              m_image[row][col].get_b() * 0.11;
            Color temp = Color(luminance, luminance, luminance);
            gray.setColor(row, col, temp);
        }
    }

    gray.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/Previous Classes/2018 Fall/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/gray.ppm");
    return gray;
}

/**Combines the x and y sobel images to create a full sobel-filtered image.
 * This is not used in the painting algorithm, but I wrote it to see what 
 * the output would be. Output is pretty noisy.
 */
void Image::sobel_full(){
    Image sobel_x = this->sobel_x();
    Image sobel_y = this->sobel_y();
    Image sobel_image = Image(m_width, m_height, m_max);

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color x_c = sobel_x.getRGB(row, col);
            Color y_c = sobel_x.getRGB(row, col);

            sobel_image.setColor(row, col, (x_c * 0.5) + (y_c * 0.5));
        }
    }

    // sobel_image.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/_CURRENT CLASSES/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/2d_final.ppm");
}

/**Applies a sobel filter on the image in the x direction.
 * Highlights horizontal edges.
 */
Image Image::sobel_x(){
    Image grayscale = this->grayscale();
    Image sobel_x = Image(m_width, m_height, m_max);
    int delta[3] = {-1, 0, 1};

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color new_color = Color();

            for (int i = 0; i < 3; i++){   
                int new_c = col + delta[i]; 

                if (new_c >= 0 && new_c < m_width){
                    new_color = new_color + (grayscale.getRGB(row, new_c) * sobel_1[i]);
                }
            }

            new_color = Color(abs(new_color.get_r()), abs(new_color.get_g()), abs(new_color.get_b()));
            new_color.clamp();

            sobel_x.setColor(row, col, new_color);
        }
    }

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color new_color = Color();

            for (int i = 0; i < 3; i++){
                int new_r = row + delta[i]; 

                if (new_r >= 0 && new_r < m_height){
                    new_color = new_color + (sobel_x.getRGB(new_r, col) * sobel_2[i]);
                }
            }

            new_color = Color(abs(new_color.get_r()), abs(new_color.get_g()), abs(new_color.get_b()));
            new_color.clamp();
            new_color = new_color / 8;

            sobel_x.setColor(row, col, new_color);
        }
    }

    // sobel_x.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/Previous Classes/2018 Fall/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/sobel_x.ppm");
    return sobel_x;
}

/**Applies a sobel filter on the image in the y direction.
 * Highlights vertical edges.
 */
Image Image::sobel_y(){
    Image grayscale = this->grayscale();
    Image sobel_y = Image(m_width, m_height, m_max);
    int delta[3] = {-1, 0, 1};

    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color new_color = Color();

            for (int i = 0; i < 3; i++){
                int new_r = row + delta[i]; 

                if (new_r >= 0 && new_r < m_height){
                    new_color = new_color + (grayscale.getRGB(new_r, col) * sobel_1[i]);
                }
            }

            new_color = Color(abs(new_color.get_r()), abs(new_color.get_g()), abs(new_color.get_b()));
            new_color.clamp();

            sobel_y.setColor(row, col, new_color);
        }
    }
    
    for (int row = 0; row < m_height; row++){
        for (int col = 0; col < m_width; col++){
            Color new_color = Color();

            for (int i = 0; i < 3; i++){   
                int new_c = col + delta[i]; 

                if (new_c >= 0 && new_c < m_width){
                    new_color = new_color + (sobel_y.getRGB(row, new_c) * sobel_2[i]);
                }
            }

            new_color = Color(abs(new_color.get_r()), abs(new_color.get_g()), abs(new_color.get_b()));
            new_color.clamp();
            // new_color = new_color / 4;
            new_color = new_color / 8;

            sobel_y.setColor(row, col, new_color);
        }
    }

    // sobel_y.writeImage("/home/niwilliams/Dropbox (Davidson College)/Davidson/Previous Classes/2018 Fall/CSC 361 - COMPUTER GRAPHICS/Homework and exercises/Painterly-Image-Rendering/images/sobel_y.ppm");
    return sobel_y;
}