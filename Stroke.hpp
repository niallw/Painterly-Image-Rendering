#ifndef Stroke_hpp
#define Stroke_hpp

#include <vector>
#include "Vector.hpp"
#include "Image.hpp"
#include "Color.hpp"

#define MAX_STROKE_LENGTH 10
#define MIN_STROKE_LENGTH 4
#define STROKE_RESOLUTION 100.0

class Stroke{
    public:
        Stroke(int, int, int);
        ~Stroke();

        friend ostream& operator<<(ostream& stream, const Stroke& stroke){
            for (int i = 0; i < stroke.control_points.size(); i++){
                stream<<"("<<stroke.control_points[i]->get_x()<<", "<<stroke.control_points[i]->get_y()<<") ";
            }
            return stream;
        }

        void set_color(Color);
        int get_radius();
        Color get_color();
        vector<Vector*> get_control_points();
        void add_control_point(int, int);

        // B-spline methods
        // Vector* calculate_spline(float);
        float calculate_N(float, int, int, vector<float>);
        void draw_stroke(Image*, int);

    private:
        int radius;
        vector<Vector*> control_points;
        vector<Vector*> knots;
        Color color;

        int calculate_degree(int, int);
        vector<float> make_knot_vector(int, int, int);
        vector<vector<float>> calc_circ(int, int, int, int, int);
};

#endif /* Stroke_hpp */