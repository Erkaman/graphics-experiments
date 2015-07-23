#include "ewa/log.hpp"

#include "ewa/value_noise.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/color.hpp"
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
constexpr int CLOUD_MIN_SIZE=20;
static const string outFile = "small.png";
constexpr float EPSILON = 0.0001f;
constexpr int SEED = 6;
constexpr unsigned char CLOUD_ALPHA = 200;

const ValueNoise noise(SEED);
Random rng(SEED);

void FindCloud( int& xmin,int& xmax, int& ymin, int& ymax);

float SampleNoise(const Vector2f& p);

void FindCloudFloodfill(int x, int y, int& xmin,int& xmax, int& ymin, int& ymax, unordered_set<Vector2f>& visitedPixels,bool& tooBig);

void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const float scale, const string& filename);

int main (int argc, char *argv[]) {

    LogInit();

    int xmin;
    int xmax;
    int ymin;
    int ymax;

    for(int i = 0; i < 20; ++i) {
	FindCloud(xmin,xmax, ymin, ymax);
	string cloudfile = string("cloud") + std::to_string(i) + string(".png");

	const int FRAME = 2;

	SaveNoise(xmin-FRAME,xmax+FRAME, ymin-FRAME, ymax+FRAME, 10.0f, cloudfile);
    }


//    string command = "open " + cloudfile;
//    system(command.c_str());

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

	int x = rng.RandomInt(-1000000,+1000000);
	int y = rng.RandomInt(-1000000,+1000000);

	if(SampleNoise(Vector2f(x,y)) > EPSILON) {
	    xmin = x;
	    xmax = x;

	    ymin = y;
	    ymax = y;

	    bool tooBig = false;


	    //LOG_I("found cloud at: %d, %d", x,y );

	    unordered_set<Vector2f> visitedPixels;
	    FindCloudFloodfill(x,y, xmin,xmax, ymin, ymax, visitedPixels, tooBig);

	    if(tooBig) {
		LOG_I("too big, try again");
		continue; // find another cloud
	    }

	    if( abs(xmax - xmin) < CLOUD_MIN_SIZE || abs(ymax - ymin) < CLOUD_MIN_SIZE) {
		LOG_I("too small, try again");
		continue;
	    }

	    LOG_I("found cloud: %d %d %d %d", xmin, xmax, ymin, ymax);

	    return;
	}
    }
}

// if true is returned the
void FindCloudFloodfill(int x, int y, int& xmin,int& xmax, int& ymin, int& ymax, unordered_set<Vector2f>& visitedPixels, bool& tooBig) {
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
	return;
    }

    visitedPixels.insert(p);

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

void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const float scale, const string& filename) {

    const size_t height = (ymax - ymin) * scale + 1;
    const size_t width =(xmax - xmin) * scale + 1;

    unsigned char* pixels = new unsigned char[4 * height * width];

    int i = 0;

    for(size_t y = 0; y < height; y += 1) {
	for(size_t x = 0; x < width; x += 1) {

	    const Vector2f p(
		(x)/scale + xmin,
		(y)/scale + ymin);
	    const float sample =  SampleNoise(p);


	    Color sky(1.00f, 1.00f, 1.00f);
	    Color clouds(sample,sample,sample);


	    unsigned char alpha = sample < EPSILON ? 0 : CLOUD_ALPHA;
//	    unsigned char val = (unsigned char)((sample) * 255.0f);

	    Color l = Color::Lerp(sky, clouds, 0.3f);

	    pixels[i++] = (unsigned char)((l.r) * 255.0f);
	    pixels[i++] = (unsigned char)((l.g) * 255.0f);
	    pixels[i++] =(unsigned char)((l.b) * 255.0f);
	    pixels[i++] = alpha;

	}
    }

    Texture::WriteToFile(pixels,width,height,filename);

    delete pixels;
}
