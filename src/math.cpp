#include "types.cpp"
#include <cmath>

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))


const f64 coefficients[][4] = {
    {-0.162257748958, -0.0016459527368, 1.00016069667, 0.0},
    {-0.150965171504, -0.0134715121937, 1.00428859564, -0.000480302854574},
    {-0.136744076525, -0.0358099559493, 1.01598497744, -0.00252170658424},
    {-0.118096608556, -0.0748651215345, 1.04325062662, -0.00886671126152},
    {-0.0952882457585, -0.134577275706, 1.0953593113, -0.0240245132062},
    {-0.0721464278044, -0.207279440982, 1.17149284075, -0.0506001284052},
    {-0.037137746396, -0.335592960027, 1.3282573775, -0.114441465794}
};

const f64 intervals[] = {
    0.349065850,
    0.523598776,
    0.698131701,
    0.872664626,
    1.047197551,
    1.221730476,
    1.570796327
};

double fast_sin(double x) {
    double xr = fmod(x, 2.0 * M_PI);
    if (xr < 0) xr += 2.0 * M_PI;

    int q = (int)std::floor(xr / (M_PI / 2.0));
    if (q > 3) q = 3;

    double t;
    int sign;
    switch (q) {
        case 0: t = xr;                 sign = +1; break;            // [0, π/2]
        case 1: t = M_PI - xr;          sign = +1; break;            // (π/2, π]
        case 2: t = xr - M_PI;          sign = -1; break;            // (π, 3π/2]
        default: t = 2.0 * M_PI - xr;   sign = -1; break;            // (3π/2, 2π)
    }

    double y = 0.0;
    const size_t N = ARRAY_COUNT(intervals);
    for (size_t i = 0; i < N; ++i) {
        bool in_seg = (i + 1 < N) ? (t <  intervals[i]) : (t <= intervals[i]);
        if (in_seg) {
            const double A = coefficients[i][0];
            const double B = coefficients[i][1];
            const double C = coefficients[i][2];
            const double D = coefficients[i][3];
            y = ((A * t + B) * t + C) * t + D;
            return sign * y;
        }
    }

    return sign * y;
}
