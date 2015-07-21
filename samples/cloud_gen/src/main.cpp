#include "ewa/log.hpp"

#include "ewa/value_noise.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/math/vector2f.hpp"


#include <vector>
#include <string>

#include <stdlib.h>
#include <math.h>

using std::vector;
using std::string;

constexpr int SIZE = 256;
static const string outFile = "small.png";

int main (int argc, char *argv[]) {

    LogInit();

    unsigned char* pixels = new unsigned char[4 * SIZE * SIZE];

    const ValueNoise noise(2);

    int i = 0;
    for(int x = 0; x < SIZE; ++x) {
	for(int y = 0; y < SIZE; ++y) {

	    Vector2f p(x,y);
	    p *= 0.02f;

	    float sample = noise.Turbulence(7, p, 2, 0.5);

	    sample -= 0.4;

	    if(sample < 0) {
		sample = 0;
	    }

	    sample = 1 - ( pow(0.1f,sample)  * 1 );

	    unsigned char alpha = sample < 0.0001f ? 0 : 255;

	    unsigned char val = (unsigned char)((sample) * 255.0f);

	    pixels[i++] = val;
	    pixels[i++] = val;
	    pixels[i++] = val;
	    pixels[i++] = alpha;

	}
    }

    Texture::WriteToFile(pixels,SIZE,SIZE,outFile);

    delete pixels;

    string command = "open " + outFile;
    system(command.c_str());

    LogDispose();

    return 0;
}
