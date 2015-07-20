import png
import random

f = open('noise.png', 'wb')      # binary mode is important

size = 32

w = png.Writer(size, size, greyscale=True)

data = []

for y in range(size):
    row = []
    for x in range(size):
        row.append(random.randint(0,64))
    data.append(row)

w.write(f, data)
f.close()
