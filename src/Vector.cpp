#include "Vector.hpp"
#include <cmath>

vector copy(vector v) { return {.x = v.x, .y = v.y}; }

vector &operator+=(vector &a, vector b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}
vector operator+(vector a, vector b) {
    vector result = copy(a);
    return result += b;
}

vector operator-(vector a) { return {.x = -a.x, .y = -a.y}; }

vector &operator-=(vector &a, vector b) { return a += -b; }

vector operator-(vector a, vector b) {
    vector result = copy(a);
    return result -= b;
}

vector &operator*=(vector &a, double factor) {
    a.x *= factor;
    a.y *= factor;
    return a;
}

vector operator*(vector a, double factor) {
    vector result = copy(a);
    return result *= factor;
}
vector operator*(double factor, vector a) { return a * factor; }

vector &operator/=(vector &a, double factor) {
    a.x /= factor;
    a.y /= factor;
    return a;
}

vector operator/(vector a, double factor) {
    vector result = copy(a);
    return result /= factor;
}
double dotProduct(vector a, vector b) { return a.x * b.x + a.y * b.y; }

double normeCarre(vector a) { return pow(a.x, 2) + pow(a.y, 2); }