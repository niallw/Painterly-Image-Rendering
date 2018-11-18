// Vector class
// Authors: Jimmy Plaut and Niall Williams

#ifndef Vector_hpp
#define Vector_hpp

#include <math.h>

class Vector{
private:
    float x, y;

public:
    Vector();
    Vector(float, float);
    ~Vector();

    // Getters
    float get_x() const;
    float get_y() const;

    float get_length();
    Vector normalize();
    Vector get_direction() const;

    // Overload some operators to make life easy
    Vector operator + (const Vector&);
    Vector operator - (const Vector&);
    Vector operator * (const float&);
    Vector operator / (const float&);

    float dot(const Vector&);

};

#endif /* Vector_hpp */
