#include "ewa/log.hpp"

#include "ewa/value_noise.hpp"
#include "ewa/math/math_common.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/math/color.hpp"
#include "ewa/random.hpp"


#include <vector>
#include <string>
#include <unordered_set>

#include <stdlib.h>
#include <math.h>

using std::vector;
using std::string;

constexpr int SEED = 7;

const ValueNoise noise(SEED);

float SampleNoise(const Vector2f& p);

void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const float scale, const string& filename);


inline
float deg2rad(const float deg)
{ return deg * M_PI / 180; }

constexpr float kEpsilon = 1e-8;


bool rayTriangleIntersect(
    const Vector3f &orig, const Vector3f &dir,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2,
    float &t, float &u, float &v)
{
    // compute plane's normal
    Vector3f v0v1 = v1 - v0;
    Vector3f v0v2 = v2 - v0;
    // no need to normalize
    Vector3f N = Vector3f::Cross(v0v1,v0v2); // N
    float denom = Vector3f::Dot(N,N);

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = Vector3f::Dot(N,dir);
    if (fabs(NdotRayDirection) < kEpsilon) // almost 0
        return false; // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    float d = Vector3f::Dot(N,v0);

    // compute t (equation 3)
    t = (  Vector3f::Dot(N,orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind

    // compute the intersection point using equation 1
    Vector3f P = orig + t * dir;

    // Step 2: inside-outside test
    Vector3f C; // vector perpendicular to triangle's plane

    // edge 0
    Vector3f edge0 = v1 - v0;
    Vector3f vp0 = P - v0;
    C = Vector3f::Cross(edge0,vp0);
    if (Vector3f::Dot(N, C) < 0) return false; // P is on the right side

    // edge 1
    Vector3f edge1 = v2 - v1;
    Vector3f vp1 = P - v1;
    C = Vector3f::Cross(edge1,vp1);
    if ((u = Vector3f::Dot(N,C)) < 0)  return false; // P is on the right side

    // edge 2
    Vector3f edge2 = v0 - v2;
    Vector3f vp2 = P - v2;
    C = Vector3f::Cross(edge2,vp2);
    if ((v = Vector3f::Dot(N,C)) < 0) return false; // P is on the right side;

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle
}

int main (int, char *[]) {

    LogInit();








    Vector3f v0(-1, -1, -5);
    Vector3f v1( 1, -1, -5);
    Vector3f v2( 0,  1, -5);

    const uint32_t width = 640;
    const uint32_t height = 480;

//    Vector3f cols[3] = {{0.6, 0.4, 0.1}, {0.1, 0.5, 0.3}, {0.1, 0.3, 0.7}};
//    Vector3f cols[3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};



    Vector3f cols[6] = {
	    {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
	    {0.5, 0.5, 0.0}, {0.5, 0.0, 0.5}, {0.0, 0.5, 0.5}

	};

    unsigned char *framebuffer = new unsigned char[width * height * 4];
    unsigned char *pix = framebuffer;
    float fov = 51.52;
    float scale = tan(deg2rad(fov * 0.5));
    float imageAspectRatio = width / (float)height;
    Vector3f orig(0);
    for (uint32_t j = 0; j < height; ++j) {
        for (uint32_t i = 0; i < width; ++i) {
            // compute primary ray
            float x = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)height) * scale;
            Vector3f dir(x, y, -1);
            //cameraToWorld.multDirMatrix(Vector3f(x, y, -1), dir);
            dir.Normalize();
            float t, u, v;


	    Vector3f col;

            if (rayTriangleIntersect(orig, dir, v0, v1, v2, t, u, v)) {

//                col = u * cols[0] + v * cols[1] + (1 - u - v) * cols[2];

		float s = u;
		float t = v;

                col =

		    //s * cols[0] + t * cols[1] + (1 - s - t) * cols[2];


		      s*s * cols[0] + t*t * cols[1] + (1 - s - t)* (1 - s - t) * cols[2] +
		    2*s*t * cols[3] + 2*s*(1-s-t) * cols[4] + 2*t* (1 - s - t) * cols[5];



//                col = Vector3f(u, v, 1 - u - v);



                //*pix = Vector3f(u, v, 1 - u - v);
            } else {
		col = Vector3f(0,0,0);
	    }

	    *(pix++) = (unsigned char)((col.x) * 255.0f);
	    *(pix++) = (unsigned char)((col.y) * 255.0f);
	    *(pix++) = (unsigned char)((col.z) * 255.0f);
	    *(pix++) = (unsigned char)(255);
        }
    }


//    SaveNoise(xmin-, cloudfile);
    LOG_I("hello");

    Texture::WriteToFile(framebuffer,width,height,"out.png");


    LogDispose();

    system("open out.png");

    return 0;
}
/*
float SampleNoise(const Vector2f& p) {

    float sample = noise.Turbulence(7, p * 0.04f, 2.0f, 0.5f);

    sample -= 0.4f;

    if(sample < 0.0f) {
	sample = 0.0f;
    }

    sample = 1.0f - ( pow(0.1f,sample)  * 1.0f );

    return sample;
    }*/

/*
void SaveNoise( const int xmin, const int xmax, const int ymin, const int ymax, const float scale, const string& filename) {

    const size_t height = (size_t)((ymax - ymin) * scale + 1);
    const size_t width = (size_t)((xmax - xmin) * scale + 1);

    unsigned char* pixels = new unsigned char[4 * height * width];

    int i = 0;

    for(size_t y = 0; y < height; y += 1) {
	for(size_t x = 0; x < width; x += 1) {

	    const Vector2f p(
		(x)/scale + xmin,
		(y)/scale + ymin);
	    const float sample =  SampleNoise(p);

	    Color sky(1.0f, 1.0f, 1.0f);
	    Color clouds(sample,sample,sample);

	//    unsigned char alpha = sample < EPSILON ? 0 : CLOUD_ALPHA;
//	    unsigned char val = (unsigned char)((sample) * 255.0f);

	    Color l = Color::Lerp(sky, clouds, 0.3f);

	    pixels[i++] = (unsigned char)((l.r) * 255.0f);
	    pixels[i++] = (unsigned char)((l.g) * 255.0f);
	    pixels[i++] =(unsigned char)((l.b) * 255.0f);
	    pixels[i++] = (unsigned char)(( Clamp(sample-0.1f, 0.0f,1.0f) ) * 255.0f);
	}
    }

    Texture::WriteToFile(pixels,width,height,filename);

    delete pixels;
}
*/
