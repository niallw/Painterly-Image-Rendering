// Vector class
// Authors: Jimmy Plaut and Niall Williams

#include "Vector.hpp"

/** Default constructor. Make a vector at the origin.
 */
Vector::Vector(){
    x = 0.0;
    y = 0.0;
}

/** Default constructor. Make a vector with origin at point (x, y, z).
 *  x - x coordinate of the origin of the vector.
 *  y - y coordinate of the origin of the vector.
 *  z - z coordinate of the origin of the vector.
 */
Vector::Vector(float x, float y){
    this->x = x;
    this->y = y;
}

/** Deconstructor
 */
Vector::~Vector(){
    // Nothing to delete
}

/** Get the x coordinate of the vector.
 */
float Vector::get_x() const{
    return x;
}

/** Get the y coordinate of the vector.
 */
float Vector::get_y() const{
    return y;
}

/** Set the x coordinate of the vector.
 */
void Vector::set_x(float new_x){
    this->x = new_x;
}

/** Set the y coordinate of the vector.
 */
void Vector::set_y(float new_y){
    this->y = new_y;
}

/** Calculate the norm of the vector.
 */
float Vector::get_length(){
    return sqrt(x*x + y*y);
}

/** Get the direction of the vector. In orthographic projection this
 *  is always the -z direction
 */
Vector Vector::get_direction() const{
    return Vector(0.0, 0.0);
}

/** Normalize the vector.
 */
Vector Vector::normalize(){
    float len = this->get_length();
    if (len == 0) return *this;
    return *this/len;
}

/** Vector addition.
 */
Vector Vector::operator+(const Vector& v){
    return Vector(this->x + v.get_x(), this->y + v.get_y());
}

/** Vector subtraction.
 */
Vector Vector::operator-(const Vector& v){
    return Vector(this->x - v.get_x(), this->y - v.get_y());
}

/** Vector by scalar multiplication.
 */
Vector Vector::operator*(const float& scalar){
    return Vector(this->x * scalar, this->y * scalar);
}

/** Vector by scalar division.
 */
Vector Vector::operator/(const float& scalar){
    return Vector(this->x / scalar, this->y / scalar);
}

/** Calculate the dot product of the vector and another vector.
 *  v - The other vector to use in the dot product
 */
float Vector::dot(const Vector& v){
    return (this->x * v.get_x()) + (this->y * v.get_y());
}

void Vector::clamp(int width, int height){
    if (this->x < 0) this->x = 0;
    if (this->x >= width) this->x = width - 1;
    if (this->y < 0) this->y = 0;
    if (this->y >= height) this->y = height - 1;
}