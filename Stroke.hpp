// Class for creating brush strokes. 
// Strokes are represented by cubic B-splines, which we define
// with control points.
// Authors: James Plaut and Niall Williams

#ifndef Stroke_hpp
#define Stroke_hpp

#include <vector>
#include "Vector.hpp"
#include "Image.hpp"
#include "Color.hpp"

#define MAX_STROKE_LENGTH 10 // Max number of control points for a stroke
#define MIN_STROKE_LENGTH 4 // Min number of control points for a stroke
#define STROKE_RESOLUTION 100.0 // How smooth the curve will be

class Stroke{
    public:
        Stroke(int, int, int);
        ~Stroke();

        // Easily print a stroke
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
        float calculate_N(float, int, int, vector<float>);
        void draw_stroke(Image*, int);

    private:
        int radius;
        vector<Vector*> control_points;
        vector<Vector*> knots;
        Color color;

        vector<float> make_knot_vector(int, int, int);
        vector<Vector> calc_circ(int, int, int, int);
        void draw_pointillist(Image*);
};

#endif /* Stroke_hpp */