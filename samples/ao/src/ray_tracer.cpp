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


void RayTracer::RayTrace() {

    LOG_I("size: %d", m_geoObj->m_verticesSize);

    const int vertexSize = (3+2+3) * 4;
    const int numVertices = m_geoObj->m_verticesSize / vertexSize;

    struct Vertex {
	Vector3f point;
	Vector2f texCoord;
	Vector3f normal;
    };

    Vertex* verticesBuffer = (Vertex *)m_geoObj->m_vertices;
    std::vector<Vertex> vertices(verticesBuffer, verticesBuffer + numVertices);


    vector<Triangle> triangles;

    for(const GeometryObjectData::Chunk* chunk : m_geoObj->m_chunks) {



	assert(chunk->m_numTriangles ==  ((chunk->m_indicesSize / 2) / 3) );

	GLushort* indices = (GLushort *)chunk->m_indices;

	LOG_I("num %d", chunk->m_numTriangles);

	for(int i = 0; i < chunk->m_numTriangles; ++i) {

	    Triangle tri;

	    tri.p0 = vertices[3*i+0].point;
	    tri.p1 = vertices[3*i+1].point;
	    tri.p2 = vertices[3*i+2].point;

	    triangles.push_back(tri);
	}
    }

    constexpr int NUM_SAMPLES = 10;

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

	LOG_I("length: %s", string(v).c_str() );

	samples.push_back(v);

    }


    for(int i = 0; i < vertices.size(); ++i) {

	Vertex& v = vertices[i];

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


	    float cosTheta = Vector3f::Dot(normal, d);


	    // ray origin
	    const Vector3f o = v.point + d * 0.001f;
	    float tMin = FLT_MAX;
	    bool hit = false;

	    for(const Triangle& tri : triangles) {

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

		if(u < 0)
		    continue; // we miss this triangle.


		Vector3f q = Vector3f::Cross(s,e1);


		float v = f * Vector3f::Dot(q, d);

		if(v < 0) {
		    continue; //we miss this triangle.
		}

		float t = f * Vector3f::Dot(q, e2);

		if(t < 0)
		    continue; // we miss this triangle.

		if(t < tMin) {
		    tMin = t;
		    hit = true;
		}

		//	break;
	    }

	    if(hit) {
		++hitCount;

		LOG_I("hit at: %f", tMin );

	    }

	    numerator += hit ? cosTheta : 0;
	    denominator += cosTheta;

	}


	LOG_I("normal: %s", string(v.normal).c_str() );
	LOG_I("pos: %s", string(v.point).c_str() );
	LOG_I("hits: %d", hitCount );
	LOG_I("ao: %f", 1.0 - numerator / denominator );

	LOG_I("NEW");



    }


    LOG_I("num vertices %d", numVertices);

    LOG_I("num tris %d", triangles.size());

    // iterate over all vertices and cast rays.

    in data, modify
	    GLsizeiptr m_verticesSize;
    GLvoid* m_vertices;

    by putting ao in pos, making pos from vec3 to vec4

	also remember to modify .
    std::vector<GLuint> m_vertexAttribsSizes;


}
