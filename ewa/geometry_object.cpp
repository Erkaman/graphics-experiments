#include "geometry_object.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/common.hpp"
#include "ewa/camera.hpp"
#include "ewa/file.hpp"
#include "ewa/cube.hpp"
#include "eob_file.hpp"
#include "resource_manager.hpp"
#include "config.hpp"
#include "physics_world.hpp"
#include "gl/depth_fbo.hpp"

#include <btBulletDynamicsCommon.h>

using std::string;
using std::vector;


class MyMotionState : public btMotionState
{
protected:
    GeometryObject* m_obj;
    btTransform mInitialPosition;

public:
    MyMotionState(const btTransform &initialPosition, GeometryObject *obj)
    {
        m_obj = obj;
	mInitialPosition = initialPosition;
    }

    virtual ~MyMotionState()
    {
    }

    virtual void getWorldTransform(btTransform &worldTrans) const
    {
        worldTrans = mInitialPosition;
    }

    virtual void setWorldTransform(const btTransform &worldTrans)
    {
        if(m_obj == NULL)
            return; // silently return before we set a node

        btQuaternion rot = worldTrans.getRotation();

        //mSceneNode ->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = worldTrans.getOrigin();
        m_obj->SetPosition(Vector3f(pos.x(), pos.y(), pos.z()) );
    }
};

btVector3 toBtVec(const Vector3f& v) {
    return btVector3(v.x,v.y,v.z);
}

btQuaternion toBtQuat(const Vector4f& v) {
    return btQuaternion(v.x,v.y,v.z, v.w);
}

Texture* LoadTexture(const string& filename) {
    Texture* texture = Texture2D::Load(filename);
    if(!texture)
	return NULL;

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    return texture;
}

GeometryObject* GeometryObject::Load(const std::string& filename, const Vector3f& position, PhysicsWorld* physicsWorld) {


    GeometryObjectData* data = EobFile::Read(filename);

    if(!data) {
	return NULL;
    }

    GeometryObject* geoObj = new GeometryObject();


    geoObj->SetPosition(position);

    /*
      Load collision shape into physics engine.
     */
    geoObj->CreateCollisionShape(data->m_collisionShape, data->m_entityInfo, physicsWorld);

    /*
      Bounding Volume
     */
    geoObj->m_aabb = data->aabb;
    geoObj->m_aabbWireframe = Cube::Load();


    string basePath = File::GetFilePath(filename);

    geoObj->m_hasNormalMap = false;
    geoObj->m_hasSpecularMap = false;
    geoObj->m_hasHeightMap = false;

    for(size_t i = 0; i < data->m_chunks.size(); ++i) {
	Material* mat = data->m_chunks[i]->m_material;

	if(mat->m_textureFilename != ""){ // empty textures should remain empty.
	    mat->m_textureFilename = File::AppendPaths(basePath, mat->m_textureFilename);
	}

	if(mat->m_normalMapFilename != ""){ // empty textures should remain empty->
	    mat->m_normalMapFilename = File::AppendPaths(basePath, mat->m_normalMapFilename);
	    geoObj->m_hasNormalMap = true;
	    geoObj->m_hasHeightMap = mat->m_hasHeightMap;
	}

	if(mat->m_specularMapFilename != ""){ // empty textures should remain empty->
	    mat->m_specularMapFilename = File::AppendPaths(basePath, mat->m_specularMapFilename);
	    geoObj->m_hasSpecularMap = true;
	}
    }

    string shaderName = "shader/simple";



    vector<string> defines;

    if(geoObj->m_hasSpecularMap) {
	defines.push_back("SPEC_MAPPING");
    }

    if(geoObj->m_hasHeightMap) {
	defines.push_back("HEIGHT_MAPPING");
    } else if(geoObj->m_hasNormalMap) { // only a normal map, no height map.
	defines.push_back("NORMAL_MAPPING");
    }

    geoObj->m_defaultShader = ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);

    if(!geoObj->m_defaultShader) {
	return NULL;
    }

    geoObj->m_depthShader = ShaderProgram::Load("shader/output_depth");

    if(!geoObj->m_depthShader) {
	return NULL;
    }


    for(size_t i = 0; i < data->m_chunks.size(); ++i) {
	GeometryObjectData::Chunk* baseChunk = data->m_chunks[i];

	Chunk* newChunk = new Chunk;

	newChunk->m_vertexBuffer = VBO::CreateInterleaved(
	    data->m_vertexAttribsSizes);

	newChunk->m_indexBuffer = VBO::CreateIndex(data->m_indexType);


	newChunk->m_numTriangles = baseChunk->m_numTriangles;

	newChunk->m_vertexBuffer->Bind();
	newChunk->m_vertexBuffer->SetBufferData(baseChunk->m_verticesSize, baseChunk->m_vertices);
	newChunk->m_vertexBuffer->Unbind();

	newChunk->m_indexBuffer->Bind();
	newChunk->m_indexBuffer->SetBufferData(baseChunk->m_indicesSize, baseChunk->m_indices);
	newChunk->m_indexBuffer->Unbind();


	if(baseChunk->m_material->m_textureFilename != "") {
	    newChunk->m_texture = LoadTexture(baseChunk->m_material->m_textureFilename);
	    if(!newChunk->m_texture) {
		return NULL;
	    }
	} else {
	    newChunk->m_texture = NULL;
	}

	if(geoObj->m_hasNormalMap) {
	    newChunk->m_normalMap = LoadTexture(baseChunk->m_material->m_normalMapFilename);
	    if(!newChunk->m_normalMap) {
		return NULL;
	    }
	} else {
	    newChunk->m_normalMap = NULL;
	}

	if(geoObj->m_hasSpecularMap) {
	    newChunk->m_specularMap = LoadTexture(baseChunk->m_material->m_specularMapFilename);
	    if(!newChunk->m_specularMap) {
		return NULL;
	    }
	} else {
	    newChunk->m_specularMap = NULL;
	}

	newChunk->m_shininess = baseChunk->m_material->m_shininess;
	newChunk->m_specularColor = baseChunk->m_material->m_specularColor;

	geoObj->m_chunks.push_back(newChunk);
    }

    return geoObj;

}


GeometryObject::~GeometryObject() {

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	MY_DELETE(chunk->m_vertexBuffer);
	MY_DELETE(chunk->m_indexBuffer);

	MY_DELETE(chunk);
    }
}

void GeometryObject::RenderShadowMap(const Matrix4f& lightVp) {

    m_depthShader->Bind();

    const Matrix4f mvp = lightVp * m_modelMatrix;
    m_depthShader->SetUniform("mvp", mvp  );

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

    m_depthShader->Unbind();

}

void GeometryObject::Render(const Camera& camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {

    m_defaultShader->Bind();

    m_defaultShader->SetPhongUniforms(

	m_modelMatrix
	, camera, lightPosition,
	lightVp);

    m_defaultShader->SetUniform("shadowMap", (int)shadowMap.GetTargetTextureUnit() );
    Texture::SetActiveTextureUnit(shadowMap.GetTargetTextureUnit());
    shadowMap.GetRenderTargetTexture().Bind();



    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];


	if(m_hasHeightMap) {
	    Config& config = Config::GetInstance();

	    m_defaultShader->SetUniform("zNear", config.GetZNear());
	    m_defaultShader->SetUniform("zFar", config.GetZFar());
	}


	if(chunk->m_texture != NULL) {
	    m_defaultShader->SetUniform("diffMap", 0);
	    Texture::SetActiveTextureUnit(0);
	    chunk->m_texture->Bind();



	}

	if(chunk->m_normalMap != NULL) {
	    m_defaultShader->SetUniform("normalMap", 1);
	    Texture::SetActiveTextureUnit(1);
	    chunk->m_normalMap->Bind();
	}

	if(chunk->m_specularMap != NULL) {
	    m_defaultShader->SetUniform("specMap", 2);
	    Texture::SetActiveTextureUnit(2);
	    chunk->m_specularMap->Bind();
	} else {
	    // if no spec map, the model has the same specular color all over the texture.
	    m_defaultShader->SetUniform("specColor", chunk->m_specularColor);
	}

	m_defaultShader->SetUniform("specShiny", chunk->m_shininess);

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);

	if(chunk->m_texture != NULL) {
	    chunk->m_texture->Unbind();
	}

	if(chunk->m_normalMap != NULL) {
	    chunk->m_normalMap->Unbind();
	}

	if(chunk->m_specularMap != NULL) {
	    chunk->m_specularMap->Unbind();
	}
    }

    shadowMap.GetRenderTargetTexture().Unbind();


    m_defaultShader->Unbind();


/*
    m_aabb.min = Vector3f((m_modelMatrix * Vector4f(m_aabb.min, 1.0f)));
    m_aabb.max = Vector3f((m_modelMatrix * Vector4f(m_aabb.max, 1.0f)));

    Vector3f center = (m_aabb.min + m_aabb.max) * 0.5f;

    Vector3f radius = m_aabb.max - center;

    m_aabbWireframe->SetModelMatrix(
//	Matrix4f::CreateTranslation(center) *
	Matrix4f::CreateScale(radius)
	);
    */



    Vector3f center = (m_aabb.min + m_aabb.max) * 0.5f;

    Vector3f radius = m_aabb.max - center;

    m_aabbWireframe->SetModelMatrix(
	m_modelMatrix*
	Matrix4f::CreateTranslation(center) *
	Matrix4f::CreateScale(radius)
	);


    m_aabbWireframe->Render(camera.GetVp() );
}

void  GeometryObject::SetModelMatrix(const Matrix4f& modelMatrix) {
    m_modelMatrix = modelMatrix;
}


AABB GeometryObject::GetModelSpaceAABB()const {

    AABB temp;

    temp.min = Vector3f((m_modelMatrix * Vector4f(m_aabb.min, 1.0f)));
    temp.max = Vector3f((m_modelMatrix * Vector4f(m_aabb.max, 1.0f)));

    return temp;
}

void GeometryObject::SetPosition(const Vector3f& position) {
    this->m_position = position;

    // update model matrix.
    this->SetModelMatrix(Matrix4f::CreateTranslation(position));

}


void GeometryObject::CreateCollisionShape(
    const CollisionShape* colShape, const EntityInfo* entityInfo,
    PhysicsWorld* physicsWorld) {

    if(!colShape)
	return; // do nothing for nothing.

    btCollisionShape* btShape = NULL;


    /*
      create collison shape.
     */
    if(colShape->m_shape == BoxShape) {
	btShape = new btBoxShape(toBtVec(colShape->m_halfExtents) );
    } else if(colShape->m_shape == SphereShape) {
	btShape = new btSphereShape(colShape->m_radius);
    } else {
    }

    /*
      Create motion state
     */
    btTransform transform(toBtQuat(colShape->m_rotate), toBtVec(m_position));
    btMotionState* btMotionState = new MyMotionState(transform, this);

    btVector3 inertia(0, 0, 0);

    // static objects dont move, so they have no intertia
    if(!entityInfo->m_isStatic) {
        btShape->calculateLocalInertia(entityInfo->m_mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo ci(entityInfo->m_mass, btMotionState, btShape, inertia);

    m_rigidBody = new btRigidBody(ci);
    physicsWorld->AddRigidBody(m_rigidBody);
}
