import random

def print_rands():

    for i in range(512):
        print random.uniform(-1,1.0), ",",

        if i % 20 == 0:
            print ""

def print_shuffle():

    li = range(0,512)
    random.shuffle(li)
    print ','.join(map(str, li))

print_rands()

#print_shuffle()
