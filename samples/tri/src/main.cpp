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

#include "Eigen/Dense"

#include <iostream>


using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::pair;


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

float SampleNoise(const Vector2f& p);

float B(int n, int i, int j, int l, float s, float t) {

    int nijl = fact(n) / (fact(i) * fact(j) * fact(l)  );
    float result =  (float)nijl *
	pow(s, i) *
	pow(t, j) *
	pow(1-s-t, l);

    return result;
}

class BezIterator {

private:
    int i,j,l;
    int n;

public:

    BezIterator(int n_) {
	i = j = l = 0;
	n = n_;
    }


    bool Next(int& outI, int& outJ, int& outL) {

	 do{

	    if(i > n) {
		i = 0;

		if(j > n) {

		    j = 0;

		    if(l > n) {
			break; // done loop.
		    } else {
			++l;
		    }

		} else {
		    ++j;
		}

	    } else {

		++i;
	    }

	 } while(i+j+l != n);

	if(i+j+l == n) {

	    outI = i; outJ = j; outL = l;
	    return true;
	} else {
	    return false; // reached end.
	}



    }

};



PerlinNoise pn;

typedef MultArray<float> SamplesGrid;

// where the Vector2f is the barycentric coordinates of the sample.
typedef vector<pair<Vector2f, float> > SamplesList;

float NOISE_SCALE =0.008;


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

constexpr int DEGREE = 11;
constexpr int NUM_CONTROL_POINTS = ((DEGREE+1)*(DEGREE+2)) / 2;


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



SamplesList samplesList;
SamplesGrid samples(width, height,0.0f);

Vector3f* FitControlPoints() {
    // are to solve the equation
    // Ax = b
    // where x is the control points we wish to approximate.

    // first let us create b.

    Eigen::VectorXf b(NUM_CONTROL_POINTS);

    BezIterator bIt(DEGREE);

    int i,j,l;

    int a = 0;
    while(bIt.Next(i,j,l) ) {

	float result  = 0;

	for(const pair<Vector2f, float>& sample : samplesList) {
	    float s = sample.first.x;
	    float t = sample.first.y;

	    if(sample.second < 0 || s < 0 || t < 0) {
		LOG_I("wat1");
		exit(1);
	    }

	    result +=
		B(DEGREE,i,j,l, s,t) * sample.second;

	}

	b(a) = result;
	++a;

    }


    // next let us create A.
    Eigen::MatrixXf A(NUM_CONTROL_POINTS, NUM_CONTROL_POINTS);

    BezIterator bRowIt(DEGREE);


    int row = 0;
    int col = 0;

    int iRow,jRow,lRow;

    while(bRowIt.Next(iRow,jRow,lRow) ) {

	int iCol,jCol,lCol;

	BezIterator bColIt(DEGREE);

	col = 0;
	while(bColIt.Next(iCol,jCol,lCol) ) {

	    float result = 0;

	    for(const pair<Vector2f, float>& sample : samplesList) {
		float s = sample.first.x;
		float t = sample.first.y;

		result +=
		    B(DEGREE,iRow,jRow,lRow, s,t) *
		    B(DEGREE,iCol,jCol,lCol, s,t);


	    }

	    A(row, col) = result;

	    ++col;
	}

	++row;
    }

//   cout << "Here is the matrix A:\n" << A << endl;
    //  cout << "Here is the vector b:\n" << b << endl;
   Eigen::VectorXf x = A.colPivHouseholderQr().solve(b);
   //cout << "The solution is:\n" << x << endl;

   Vector3f* cps = new Vector3f[NUM_CONTROL_POINTS];

   for(int i = 0; i < NUM_CONTROL_POINTS; ++i) {
       LOG_I("i: %d = %f", i, x(i));
       cps[i] = x(i);
   }

   return cps;
}

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

void GatherSamples() {
        int acceptedSamples = 0;

    MultArray<bool> samplesGrid(GRID_X, GRID_Y, false);

    while(acceptedSamples < SAMPLES) {

	uint32_t i = rand() % width;
	uint32_t j = rand() % height;

            float t, u, v;

	if (!rayTriangleIntersect(i,j,t, u, v))

    continue; // reject


	float s = u;
	t = v;



	int gridX = i / GRID_CELL_X_SIZE;
	int gridY = j / GRID_CELL_Y_SIZE;

	if(samplesGrid(gridX, gridY) == true  )
	    continue; // reject.

	samplesGrid(gridX, gridY) = true;

	float sample =SampleNoise(i,j);
	samples(i,j) = sample;
	samplesList.push_back( pair<Vector2f, float>(Vector2f(s,t), sample )  );

	++acceptedSamples;

    }

}


Vector3f bezTri(int n, float s, float t, Vector3f* cols) {

//    s * cols[0] + t * cols[1] + (1 - s - t) * cols[2];

    Vector3f result(0,0,0);

    int a = 0;


    BezIterator iter(n);

    int i,j,l;

    while(iter.Next(i,j,l) ) {

//	LOG_I("i,j,l,s,t = %d, %d, %d, %f, %f",i,j,l,s,t);


	result = result +
	    B(n, i,j,l,s,t) * cols[a];

	a++;

    }

/*
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
	}*/

    //    exit(1);

    if(result.x < 0 || result.y < 0 || result.z < 0) {


	LOG_I("wat 9: %s",  string(result).c_str() );
	exit(1);
    }


    return result;
}




int main (int, char *[]) {

/*

    Eigen::MatrixXf A(6,6);
    Eigen::VectorXf b(6);

   A(0,0) = 1;
   A(0,1) = 2;
   A(0,2) = 3;

   A(1,0) = 4;
   A(1,1) = 5;
   A(1,2) = 6;

   A(2,0) = 7;
   A(2,1) = 8;
   A(2,2) = 10;

    A <<
	29,2,2,29,2,22,
	2,29,25,2,2,22,
	2,22,2,2,27,2,
	25,2,2,2,2,2,
	2,28,2,27,2,29,
	2,2,2,2,2,2;


    b <<
	2,
	2,
	2,
	2,
	2,
	2;

   cout << "Here is the matrix A:\n" << A << endl;
   cout << "Here is the vector b:\n" << b << endl;
   Eigen::VectorXf x = A.colPivHouseholderQr().solve(b);
   cout << "The solution is:\n" << x << endl;


   exit(1);
*/
    srand(time(NULL));


    LogInit();

    system("rm out.png");

    Vector3f cols1[3] = {
	{1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0}};

    Vector3f cols2[6] = {
	{1.0, 0.0, 0.0},   {0.5, 0.5, 0.0},

	{0.0, 1.0, 0.0},      {0.5, 0.0, 0.5},

	 {0.0, 0.5, 0.5}, {0.0, 0.0, 1.0}
	};


    //  bezTri(2,0,0);

    unsigned char *framebuffer = new unsigned char[width * height * 4];
    unsigned char *pix = framebuffer;

    LOG_I("gather samples");
    GatherSamples();

    LOG_I("fitting control points");
    Vector3f* cps = FitControlPoints();

    LOG_I("rendering");
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

//                col = bezTri(2, s, t, cols2);

                col = bezTri(DEGREE, s, t, cps);



/*
		float sample = SampleNoise(Vector2i(i,j));
		col =Vector3f(sample);
*/

//		    s * cols[0] + t * cols[1] + (1 - s - t) * cols[2];

//		      s*s * cols[0] + t*t * cols[1] + (1 - s - t)* (1 - s - t) * cols[2] + 2*s*t * cols[3] + 2*s*(1-s-t) * cols[4] + 2*t* (1 - s - t) * cols[5];



//                col = Vector3f(u, v, 1 - u - v);



                //*pix = Vector3f(u, v, 1 - u - v);

		alpha = 255;

		/*
		if(samples(i,j) > 0.001) {
		    col = Vector3f(1,1,0);
		    }*/


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
