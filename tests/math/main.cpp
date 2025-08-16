#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <immintrin.h>

#include "types.cpp"

struct Points {
    f64* data;
    u64 count;
};

Points sampleLinearly(f64 from, f64 to, u64 count) {
    Points points;
    points.data = static_cast<f64*>(malloc(sizeof(f64) * (count+1)));
    points.count = count + 1;

    f64 gap = (to - from) / static_cast<f64>(count);

    f64* ptr = points.data;
    for (u64 i = 0; i <= count; ++i) {
        *(ptr + i) = from + i * gap;
    }

    return points;
}

f64 maxAbsErr(f64 func1(f64), f64 func2(f64), Points pts) {
    f64 max = 0.0;

    for (u64 i = 0; i < pts.count; ++i) {
        f64 absDiff = fabs(func1(pts.data[i]) - func2(pts.data[i]));
        max = max < absDiff ? absDiff : max;
    }

    return max;
}

f64 intrinsic_sqrt(f64 x) {
    __m128d src = _mm_set_sd(x);

    __m128d res = _mm_sqrt_sd(src, src);

    return _mm_cvtsd_f64 (res);
}

f64 fast_sin(f64 x) {
    return 0.0;
}

f64 fast_cos(f64 x) {
    return 0.0;
}

f64 fast_asin(f64 x) {
    return 0.0;
}

f64 fast_sqrt(f64 x) {
    return 0.0;
}

int main() {
    Points sinRange = sampleLinearly(-1.0 * M_PI, M_PI, 1000);
    Points cosRange = sampleLinearly(-1.0 * M_PI / 2.0, M_PI / 2.0, 1000);
    Points zeroToOne = sampleLinearly(0, 1, 1000);

    f64 sinMaxAbsErr  = maxAbsErr(fast_sin,  sin,  sinRange);
    f64 cosMaxAbsErr  = maxAbsErr(fast_cos,  cos,  cosRange);
    f64 asinMaxAbsErr = maxAbsErr(fast_asin, asin, zeroToOne);
    f64 sqrtMaxAbsErr = maxAbsErr(fast_sqrt, sqrt, zeroToOne);

    printf("sin(x) %f\n",  sinMaxAbsErr);
    printf("cos(x) %f\n",  cosMaxAbsErr);
    printf("asin(x) %f\n", asinMaxAbsErr);
    printf("sqrt(x) %f\n", sqrtMaxAbsErr);
}
