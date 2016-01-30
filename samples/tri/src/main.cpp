#include "ewa/log.hpp"

#include "ewa/math/math_common.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector2i.hpp"
#include "ewa/mult_array.hpp"

#include "perlin.hpp"

#include <math.h>
#include <map>

#include "util.hpp"

using std::vector;
using std::string;

float SampleNoise(const Vector2f& p);


PerlinNoise pn;

typedef MultArray<float> Samples;

float NOISE_SCALE =0.02;


    float TRI_SCALE= 1.0f;;

float SampleNoise(Vector2i p) {
    return pn.noise(p.x*NOISE_SCALE,p.y*NOISE_SCALE,0);
}

float SampleNoise(uint32_t x, uint32_t y) {
    return SampleNoise(Vector2i(x,y));
}

Vector3f v0(-1.0f * TRI_SCALE, -1.0f * TRI_SCALE, -5.0f * TRI_SCALE);
Vector3f v1( 1.0f * TRI_SCALE, -1.0f * TRI_SCALE, -5.0f * TRI_SCALE);
Vector3f v2( 0.0f,  1.0f * TRI_SCALE, -5.0f * TRI_SCALE);

constexpr uint32_t width = 640;
constexpr uint32_t height = 480;

constexpr int GRID_X = 30;
constexpr int GRID_Y = 30;

constexpr int GRID_CELL_X_SIZE = width / GRID_X;
constexpr int GRID_CELL_Y_SIZE = height / GRID_Y;

int SAMPLES = 200;

float fov = 51.52;

float scale = tan(deg2rad(fov * 0.3));

float imageAspectRatio = width / (float)height;
Vector3f orig(0);

bool rayTriangleIntersect(
    uint32_t i, uint32_t j,

    float &t, float &u, float &v) {

    float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
    float y = (1 - 2 * (j + 0.5) / (float)height) * scale;
    Vector3f dir(x, y, -1);
    //cameraToWorld.multDirMatrix(Vector3f(x, y, -1), dir);
    dir.Normalize();

    return rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v);
}


int fact(int n) {
    if(n <= 1)
	return 1;

    return n * fact(n-1);
}

float pow(float b, int e) {
    if(e == 0)
	return 1.0;

    return b * pow(b,e-1);
}

float B(int n, int i, int j, int l, float s, float t) {

    int nijl = fact(n) / (fact(i) * fact(j) * fact(l)  );
    return (float)nijl *
	pow(s, i) *
	pow(t, j) *
	pow(1-s-t, l);

}

Vector3f bezTri(int n, float s, float t, Vector3f* cols) {

//    s * cols[0] + t * cols[1] + (1 - s - t) * cols[2];

    Vector3f result(0,0,0);

    int a = 0;
    for(int l = 0; l <= n; ++l) {

	for(int j = 0; j <= n; ++j) {

	    for(int i = 0; i <= n; ++i) {

		if(i+j+l == n) {



		    result = result +
			B(n, i,j,l,s,t) * cols[a];

		    a++;

		    //	       printf("i,j,l: %d,%d,%d\n", i,j,l);
		}
	    }

	}
    }

    //    exit(1);
    return result;
}




int main (int, char *[]) {

    srand(time(NULL));


    LogInit();

    system("rm out.png");

//    Vector3f cols[3] = {{0.6, 0.4, 0.1}, {0.1, 0.5, 0.3}, {0.1, 0.3, 0.7}};
    Vector3f cols1[3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};



    Vector3f cols2[6] = {
	{1.0, 0.0, 0.0},   {0.5, 0.5, 0.0},

	{0.0, 1.0, 0.0},      {0.5, 0.0, 0.5},

	 {0.0, 0.5, 0.5}, {0.0, 0.0, 1.0}
	};


    //  bezTri(2,0,0);


    unsigned char *framebuffer = new unsigned char[width * height * 4];
    unsigned char *pix = framebuffer;

    Samples samples(width, height,0.0f);
    MultArray<bool> samplesGrid(GRID_X, GRID_Y, false);
    int acceptedSamples = 0;


    while(acceptedSamples < SAMPLES) {

	uint32_t i = rand() % width;
	uint32_t j = rand() % height;

            float t, u, v;

	if (!rayTriangleIntersect(i,j,t, u, v))
	    continue; // reject

//	const Vector2i point(i,j);


//	LOG_I("grid: %d, %d", i,j);


	int gridX = i / GRID_CELL_X_SIZE;
	int gridY = j / GRID_CELL_Y_SIZE;

//	LOG_I("grid: %d, %d", gridX, gridY);

	if(samplesGrid(gridX, gridY) == true  )
	    continue; // reject.

	samplesGrid(gridX, gridY) = true;

	samples(i,j) = SampleNoise(i,j);
	++acceptedSamples;

    }

    for (uint32_t j = 0; j < height; ++j) {
        for (uint32_t i = 0; i < width; ++i) {
            // compute primary ray

            float t, u, v;

	    Vector3f col;
	    unsigned char alpha;

            if (rayTriangleIntersect(i,j,t, u, v)) {

//                col = u * cols[0] + v * cols[1] + (1 - u - v) * cols[2];

		float s = u;
		float t = v;


                col = bezTri(1, s, t, cols1);


		float sample = pn.noise(i*NOISE_SCALE,j*NOISE_SCALE,0);
		col =Vector3f(sample);


//		    s * cols[0] + t * cols[1] + (1 - s - t) * cols[2];

//		      s*s * cols[0] + t*t * cols[1] + (1 - s - t)* (1 - s - t) * cols[2] + 2*s*t * cols[3] + 2*s*(1-s-t) * cols[4] + 2*t* (1 - s - t) * cols[5];



//                col = Vector3f(u, v, 1 - u - v);



                //*pix = Vector3f(u, v, 1 - u - v);

		alpha = 255;

		if(samples(i,j) > 0.001) {
		    col = Vector3f(1,1,0);
		}


            } else {
		col = Vector3f(0,0,0);
		alpha = 0;
	    }

	    *(pix++) = (unsigned char)((col.x) * 255.0f);
	    *(pix++) = (unsigned char)((col.y) * 255.0f);
	    *(pix++) = (unsigned char)((col.z) * 255.0f);
	    *(pix++) = (unsigned char)(alpha);
        }
    }


//    SaveNoise(xmin-, cloudfile);
    LOG_I("hello");

    Texture::WriteToFile(framebuffer,width,height,"out.png");


    LogDispose();

    system("open out.png");

    return 0;
}
