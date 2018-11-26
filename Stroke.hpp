#ifndef Stroke_hpp
#define Stroke_hpp

#include <vector>
#include "Vector.hpp"
#include "Image.hpp"
#include "Color.hpp"

#define MAX_STROKE_LENGTH 2 //TODO: change number?
#define MIN_STROKE_LENGTH 1 //TODO: change number?

class Stroke{
    public:
        Stroke(int, int, int, Color);
        ~Stroke();

        friend ostream& operator<<(ostream& stream, const Stroke& stroke){
            for (int i = 0; i < stroke.control_points.size(); i++){
                stream<<"("<<stroke.control_points[i]->get_x()<<", "<<stroke.control_points[i]->get_y()<<") ";
            }
            return stream;
        }

        Color get_color();
        vector<Vector*> get_control_points();
        void add_control_point(int, int);

    private:
        int radius;
        vector<Vector*> control_points;
        Color color;
};

#endif /* Stroke_hpp */