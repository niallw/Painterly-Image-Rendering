#ifndef Stroke_hpp
#define Stroke_hpp

#include <vector>
#include "Vector.hpp"
#include "Image.hpp"
#include "Color.hpp"

#define MAX_STROKE_LENGTH 10 //TODO: change number?

class Stroke{
    public:
        Stroke(int, int, int, Image*);
        ~Stroke();

    private:
        int radius;
        vector<Vector*> control_points;
        Color color;
        Image* reference_image;
};

#endif /* Stroke_hpp */