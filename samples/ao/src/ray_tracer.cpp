#include "ray_tracer.hpp"

#include "ewa/geometry_object_data.hpp"

#include "ewa/log.hpp"
#include "ewa/random.hpp"

#include <math.h>
#include <float.h>

using std::string;
using std::vector;

struct Triangle {
    Vector3f p0;
    Vector3f p1;
    Vector3f p2;
};

bool MahRayTrace(const Triangle& tri, const Vector3f& o, const Vector3f& d, float& t) {
    		    const Vector3f& v0 = tri.p0;
		    const Vector3f& v1 = tri.p1;
		    const Vector3f& v2 = tri.p2;

/*
  if(
  v0 == o ||
  v1 == o ||
  v2 == o
  )
  continue;
*/
		    Vector3f e1 = v1 - v0;
		    Vector3f e2 = v2 - v0;
		    Vector3f s = o - v0;



		    Vector3f p = Vector3f::Cross(d, e2);
		    float a = Vector3f::Dot(p, e1);
		    float f = 1.0f / a;

		    float u = f * Vector3f::Dot(p, s);

		    if(u < 0) {
			//	LOG_I("MISS1");
			return false;
		    }


		    Vector3f q = Vector3f::Cross(s,e1);


		    float v = f * Vector3f::Dot(q, d);

		    if(v < 0) {
			//	LOG_I("MISS2");
			return false; //we miss this triangle.
		    }

		    t = f * Vector3f::Dot(q, e2);

		    if(t < 0) {
//			LOG_I("MISS3");

			return false; // we miss this triangle.
		    }

/*
		    			LOG_I("hit da tri");

			LOG_I("d: %s", string(d).c_str() );
			LOG_I("o: %s", string(o).c_str() );

			LOG_I("v0: %s", string(v0).c_str() );
			LOG_I("v1: %s", string(v1).c_str() );
			LOG_I("v2: %s", string(v2).c_str() );
*/
		    return true;


}

RayTracer::RayTracer(GeometryObjectData* geoObj) {
    m_geoObj = geoObj;
}

void FindBase(const Vector3f& normal, Vector3f& tangent, Vector3f& bitangent) {


    if(normal.z == 0)
	tangent = Vector3f(0,0,1);
    else if(normal.y == 0)
	tangent = Vector3f(0,1,0);
    else if(normal.x == 0)
	tangent = Vector3f(1,0,0);
    else
	tangent = Vector3f(1, 1, -1.0 * (normal.x + normal.y) / normal.z).Normalize();



    bitangent = Vector3f::Cross(tangent, normal).Normalize();
}


GeometryObjectData* RayTracer::RayTrace() {
/*
    Triangle tri;


    tri.p0 = Vector3f(0,2, 0);
    tri.p1 = Vector3f(3,-2, 0);
    tri.p2 = Vector3f(-3,-2, 0);

    Vector3f o(0,0, 10);
    Vector3f d(0.1,0, -1);

    float t;
    bool b = MahRayTrace(tri, o, d, t);

    LOG_I("B: %d", b);
    LOG_I("t: %f", t);
    exit(1);
*/


//    LOG_I("vertices size: %d", m_geoObj->m_verticesSize);

    const int vertexSize = (3+2+3) * sizeof(float);
    const int numVertices = m_geoObj->m_verticesSize / vertexSize;

    LOG_I("m_geoObj->m_verticesSize: %d", m_geoObj->m_verticesSize);
    LOG_I("numVertices: %d", m_geoObj->m_verticesSize / 4);

    struct Vertex {
	Vector3f point;
	Vector2f texCoord;
	Vector3f normal;
    };

    Vertex* verticesBuffer = (Vertex *)m_geoObj->m_vertices;
    std::vector<Vertex> vertices(verticesBuffer, verticesBuffer + numVertices);

/*
    float* blabla = (float *)m_geoObj->m_vertices;
    std::vector<float> blas(blabla, blabla +   );
*/

    for(int i = 0; i < vertices.size(); ++i) {
//	LOG_I("vert pos: %s", string(vertices[i].point).c_str() );
    }

    LOG_I("vertices: %s", string(vertices[5].point).c_str() );
    LOG_I("texcoord: %s", string(vertices[5].texCoord).c_str() );

    vector<Triangle> triangles;

    for(const GeometryObjectData::Chunk* chunk : m_geoObj->m_chunks) {

	assert(chunk->m_numTriangles ==  ((chunk->m_indicesSize / 2) / 3) );

	GLushort* indices = (GLushort *)chunk->m_indices;

	LOG_I("numtriangles %d", chunk->m_numTriangles);

	for(int i = 0; i < chunk->m_numTriangles; ++i) {

	    Triangle tri;

	    tri.p0 = verticesBuffer[indices[i*3+0]].point;
	    tri.p1 = verticesBuffer[indices[i*3+1]].point;
	    tri.p2 = verticesBuffer[indices[i*3+2]].point;
/*
	    LOG_I("TRI");
	    LOG_I("v0: %s", string(tri.p0).c_str() );
	    LOG_I("v1: %s", string(tri.p1).c_str() );
	    LOG_I("v2: %s", string(tri.p2).c_str() );
*/


	    triangles.push_back(tri);
	}
    }
//    exit(1);
/*
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:168:RayTrace:v0: (0.000000, 0.000000, 1.879216)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:169:RayTrace:v1: (0.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:170:RayTrace:v2: (-1.000000, 0.000000, -1.000000)

INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:168:RayTrace:v0: (0.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:169:RayTrace:v1: (-1.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:170:RayTrace:v2: (0.000000, 0.000000, -1.000000)

INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:168:RayTrace:v0: (-1.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:169:RayTrace:v1: (0.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:170:RayTrace:v2: (0.000000, 0.263956, -1.000000)

INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:168:RayTrace:v0: (0.000000, 0.000000, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:169:RayTrace:v1: (0.000000, 0.263956, -1.000000)
INFO: /Users/eric/tuhu/samples/ao/src/ray_tracer.cpp:170:RayTrace:v2: (-1.000000, 0.263956, -1.000000)
*/
    constexpr int NUM_SAMPLES = 20;

    // generate evenly distributed unit vectors
    vector<Vector3f> samples;

    Random rng(200);

    while(samples.size() < NUM_SAMPLES) {

	float x,y,z;
	float length2;

	while(true) {

	    x = -1 + 2*rng.RandomFloat();
	    y = 0 + 1*rng.RandomFloat();
	    z = -1 + 2*rng.RandomFloat();


	    if ((length2 =x*x + y*y +z*z) < 1)
		break;
	}

	float length = sqrt(length2);

	Vector3f v(
	    x /= length,
	    y /= length,
	    z /= length);

//	LOG_I("length: %s", string(v).c_str() );

	samples.push_back(v);

    }


    struct AoVertex {
	Vector4f point;
	Vector2f texCoord;
	Vector3f normal;
    };


    // LOG_I("num vertices: %d", vertices.size());

    AoVertex* aoVertices = new AoVertex[vertices.size()];
    AoVertex* aoVerticesPointer = aoVertices;



    for(int i = 0; i < vertices.size(); ++i) {

//	LOG_I("vert: %d", i );

	float ao;

	    Vertex& v = vertices[i];

/*	if(i > 0) {
	    ao = 1.0;
	    } else'*/ {


	    Vector3f normal = v.normal;
	    Vector3f tangent;
	    Vector3f bitangent;

	    FindBase(normal, tangent, bitangent);

	    /*
	      LOG_I("normal: %s", string(normal).c_str() );
	      LOG_I("tangent: %s", string(tangent).c_str() );
	      LOG_I("bitangent: %s", string(bitangent).c_str() );

	    */


	    float numerator = 0;
	    float denominator = 0;

	    int hitCount = 0;
	    for(Vector3f sample : samples) {

		// rotate sample vector to be in the base of the point.

		// ray direction.
		Vector3f d = Vector3f(
		    Vector3f::Dot(bitangent, sample),
		    Vector3f::Dot(normal, sample),
		    Vector3f::Dot(tangent, sample)
		    ).Normalize();

/*
		LOG_I("d: %s", string(d).c_str() );

		LOG_I("length: %f", d.Length() );
*/
		float cosTheta = Vector3f::Dot(normal, d);


		// ray origin
		const Vector3f o = v.point + d * 0.01f;

//		LOG_I("o: %s", string(o).c_str() );

		float tMin = FLT_MAX;
		bool hit = false;

		for(const Triangle& tri : triangles) {

		    float t;
		    if(!MahRayTrace(tri, o, d, t))
			continue;

		    if(t < tMin) {
			tMin = t;
/*
			LOG_I("HIT");
			LOG_I("v0: %s", string(tri.p0).c_str() );
			LOG_I("v1: %s", string(tri.p1).c_str() );
			LOG_I("v2: %s", string(tri.p2).c_str() );
*/

			hit = true;
		    }

		    //	break;
		}

		if(hit) {
		    ++hitCount;

		    //    LOG_I("hit at: %f", tMin );

		} else {
		    //	    LOG_I("no hit at: %f", tMin );
		}

		numerator += hit ? cosTheta : 0;
		denominator += cosTheta;

	    }



	    //    LOG_I("div: %f/%f, ",  numerator, denominator);
	    ao = 1.0 - numerator / denominator;

/*	LOG_I("normal: %s", string(v.normal).c_str() );
	LOG_I("pos: %s", string(v.point).c_str() );
	LOG_I("hits: %d", hitCount );
	LOG_I("ao: %f",  ao);*/
	}





//	LOG_I("NEW");


	AoVertex aoVertex;

	aoVertex.point = Vector4f(v.point.x, v.point.y, v.point.z, ao);
	aoVertex.normal = v.normal;
	aoVertex.texCoord = v.texCoord;

	*(aoVerticesPointer++) = aoVertex;

    }

    m_geoObj->m_vertices = ((void *)aoVertices);
    m_geoObj->m_verticesSize = vertices.size() * sizeof(float) * (4+2+3);

    vector<GLuint> vas{4,2,3};
    m_geoObj->m_vertexAttribsSizes = vas;

//    LOG_I("vertices size: %d", m_geoObj->m_verticesSize);


    // iterate over all vertices and cast rays.

    /*
    in data, modify
	    GLsizeiptr m_verticesSize;
    GLvoid* m_vertices;

    by putting ao in pos, making pos from vec3 to vec4

	also remember to modify .
    std::vector<GLuint> m_vertexAttribsSizes;
    */

    return m_geoObj;
}
