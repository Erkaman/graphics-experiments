import png
import random

f = open('noise.png', 'wb')      # binary mode is important

size = 256

w = png.Writer(size, size, greyscale=True)

data = []

for y in range(size):
    row = []
    for x in range(size):
        row.append(random.randint(0,255))
    data.append(row)

w.write(f, data)
f.close()
