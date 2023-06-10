#pragma once
struct vector {
    double x, y;
};

vector copy(vector);

vector &operator+=(vector &, vector);
vector operator+(vector, vector);

vector &operator-=(vector &, vector);
vector operator-(vector);
vector operator-(vector, vector);

vector &operator*=(vector &, double factor);

vector operator*(vector, double factor);
vector operator*(double factor, vector);

vector &operator/=(vector &, double factor);

vector operator/(vector, double factor);

double dotProduct(vector, vector);
double normeCarre(vector);