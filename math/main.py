import numpy as np
import math
from scipy.interpolate import make_interp_spline, PPoly
import matplotlib.pyplot as plt

x = np.linspace(0, np.pi/2, 10)
y = np.sin(x)

S = make_interp_spline(x, y, k=3)
PP = PPoly.from_spline(S)

knots = PP.x
coeffs = PP.c
nseg = coeffs.shape[1]

for i in range(nseg):
    x0, x1 = knots[i], knots[i+1]
    a, b, c, d = coeffs[:, i]

    A = a
    B = b - 3.0*a*x0
    C = 3.0*a*x0*x0 - 2.0*b*x0 + c
    D = -a*x0**3 + b*x0**2 - c*x0 + d

    if(x0 == x1): continue
    print(f"Segment {i}:  x in [{x0:.9f}, {x1:.9f}]")
    print(f"    Local:    {a:.12g}*(x-{x0:.9f})^3 + {b:.12g}*(x-{x0:.9f})^2 + {c:.12g}*(x-{x0:.9f}) + {d:.12g}")
    print(f"    Expanded: {A:.12g}*x^3 + {B:.12g}*x^2 + {C:.12g}*x + {D:.12g}\n")

xes = np.linspace(0, np.pi/2, 1000)
sin_true = np.sin(xes)
sin_spline = S(xes)
print("Max abs error:", np.max(np.abs(sin_true - sin_spline)))

plt.plot(xes, sin_true, label="sin")
plt.plot(xes, sin_spline, label="cubic spline")
plt.legend(); plt.show()

