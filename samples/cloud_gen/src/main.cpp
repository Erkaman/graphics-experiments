#include "ewa/log.hpp"

#include "ewa/value_noise.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/random.hpp"


#include <vector>
#include <string>
#include <unordered_set>

#include <stdlib.h>
#include <math.h>

using std::vector;
using std::string;
using std::unordered_set;

constexpr int SIZE = 256;
constexpr int CLOUD_MAX_SIZE=100;
static const string outFile = "small.png";
constexpr float EPSILON = 0.0001f;
constexpr int SEED = 2;

const ValueNoise noise(SEED);
Random rng(SEED);

void FindCloud( int& xmin,int& xmax, int& ymin, int& ymax);

float SampleNoise(const Vector2f& p);

void FindCloudFloodfill(int x, int y, int& xmin,int& xmax, int& ymin, int& ymax, unordered_set<Vector2f>& visitedPixels,bool& tooBig);

void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const string& filename);


int main (int argc, char *argv[]) {

    LogInit();

    int xmin;
    int xmax;
    int ymin;
    int ymax;

    FindCloud(xmin,xmax, ymin, ymax);


    string cloudfile = "cloud.png";

    SaveNoise(xmin,xmax, ymin, ymax, cloudfile);


    SaveNoise(0, SIZE-1, 0, SIZE-1, outFile);

    string command = "open " + cloudfile;
    system(command.c_str());

    LogDispose();

    return 0;
}

float SampleNoise(const Vector2f& p) {

    float sample = noise.Turbulence(7, p * 0.04f, 2, 0.5);

    sample -= 0.4;

    if(sample < 0) {
	sample = 0;
    }

    sample = 1 - ( pow(0.1f,sample)  * 1 );

    return sample;
}


void FindCloud( int& xmin,int& xmax, int& ymin, int& ymax) {

    while(true) {

	int x = rng.RandomInt(0,SIZE);
	int y = rng.RandomInt(0,SIZE);

	if(SampleNoise(Vector2f(x,y)) > EPSILON) {


	    xmin = x;
	    xmax = x;

	    ymin = y;
	    ymax = y;

	    bool tooBig = false;

	    unordered_set<Vector2f> visitedPixels;

	    //LOG_I("found cloud at: %d, %d", x,y );

	    FindCloudFloodfill(x,y, xmin,xmax, ymin, ymax, visitedPixels, tooBig);

	    if(tooBig) {

		LOG_I("too big, try again");

		continue; // find another cloud
	    }

	    LOG_I("minmax: %d %d %d %d", xmin, xmax, ymin, ymax);

	    return;
	}
    }
}

// if true is returned the
void FindCloudFloodfill(int x, int y, int& xmin,int& xmax, int& ymin, int& ymax, unordered_set<Vector2f>& visitedPixels, bool& tooBig) {

    //  LOG_I("x, y: %d %d", x,y);

    Vector2f p(x,y);

    if(tooBig) {
	return; // too big, stop the entire recursion
    }

    if(abs(xmax -xmin) > CLOUD_MAX_SIZE || abs(ymax -ymin) > CLOUD_MAX_SIZE) {
	tooBig = true;
	return; // too big, shut down recursion
    }

    const float sample = SampleNoise(p);
    if(
	(sample < EPSILON) || // invisible pixel
	visitedPixels.find(p) != visitedPixels.end() // already visited pixel
	) {
	// no longer a cloud. stop recursion.
//	LOG_I("return: %d %d",x,y);
	return;
    }

    visitedPixels.insert(p);

//    LOG_I("visit: %s = %f", string(p).c_str(),  sample);

    // update min and max.

    if(x < xmin) {
	xmin = x;
    }
    if(y < ymin) {
	ymin = y;
    }

    if(x > xmax) {
	xmax = x;
    }
    if(y > ymax) {
	ymax = y;
    }

    FindCloudFloodfill(x+1,y,xmin,xmax, ymin, ymax,visitedPixels, tooBig);
    FindCloudFloodfill(x-1,y,xmin,xmax, ymin, ymax,visitedPixels, tooBig);
    FindCloudFloodfill(x,y+1,xmin,xmax, ymin, ymax,visitedPixels, tooBig);
    FindCloudFloodfill(x,y-1,xmin,xmax, ymin, ymax,visitedPixels, tooBig);
}

void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const string& filename) {

    const size_t height = (ymax - ymin + 1);
    const size_t width =(xmax - xmin + 1);

    unsigned char* pixels = new unsigned char[4 * height * width];

    int i = 0;

    for(int y = ymin; y <= ymax; ++y) {
	for(int x = xmin; x <= xmax; ++x) {

//INFO: /Users/eric/tuhu/samples/cloud_gen/src/main.cpp:124:FindCloudFloodfill:visit: (125.000000, 36.000000) = 0.669835

	    const Vector2f p(x,y);
	    const float sample =  SampleNoise(p);

/*	    if(x == 125 && y == 36) {
		LOG_I("%f", sample);
		}*/

	    unsigned char alpha = sample < EPSILON ? 0 : 255;

/*	    if(x == 125) {
		LOG_I("visit: %s = %f", string(p).c_str(),  sample);
	    }*/

	    unsigned char val = (unsigned char)((sample) * 255.0f);

	    pixels[i++] = val;
	    pixels[i++] = val;
	    pixels[i++] = val;
	    pixels[i++] = alpha;

	}
    }

    Texture::WriteToFile(pixels,width,height,filename);

    delete pixels;
}
