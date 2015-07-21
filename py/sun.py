import png
import random

import os
import math

# VARIABLES ARE HERE.

OUT_FILE = '../samples/game/img/sun.png'
#sun_color = [251,250,185]
sun_color = [251,250,210]
size = 256

##########################

f = open(OUT_FILE, 'wb')      # binary mode is important


w = png.Writer(size, size, alpha='RGBA')

data = []

def lerp(color1, color2, x):

    # f(0)  = 0
    # f(1) = 1
    # f(0.1) = 0.3
    l = -1.03351 * (math.exp(-3.422887 * x) - 1)
#    l = x

    mc1 = [x * (1 - l) for x in color1]
    mc2 = [x * (l) for x in color2]

    return [int(x+y) for x,y in zip(mc1, mc2 ) ]


def dist(x,y):
    dx = abs(x - 128)
    dy = abs(y - 128)
    return math.sqrt(dx * dx + dy * dy)


sun_full = sun_color + [255]
sun_fade = sun_color + [0]

for y in range(size):
    for x in range(size):

        d = dist(x,y)

        if d < 70:
            c = sun_full
        elif d >= 70 and d < 110:
            c = lerp(sun_full, sun_fade, (d-70.0)/40.0 )
        else:
            c = [0,0,0,0]

        data.extend(c)

w.write_array(f, data)
f.close()

os.system("open " + OUT_FILE)
