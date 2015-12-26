#include "geometry_object.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/bt_util.hpp"

#include "ewa/common.hpp"
#include "ewa/icamera.hpp"
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


Matrix4f fromBtMat(const btMatrix3x3& m) {

    return Matrix4f(
	m[0].x(),m[0].y(),m[0].z(),0,
	m[1].x(),m[1].y(),m[1].z(),0,
	m[2].x(),m[2].y(),m[2].z(),0,
	0            , 0           , 0           ,1
	);


}

ATTRIBUTE_ALIGNED16(class) MyMotionState : public btMotionState
{
protected:
    GeometryObject* m_obj;
    btTransform mInitialPosition;

public:

	BT_DECLARE_ALIGNED_ALLOCATOR();

    MyMotionState(const btTransform& initialPosition, GeometryObject *obj)
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
/*
	btMatrix3x3 r = worldTrans.getBasis();

	Matrix4f rot(
	    r[0].x(),r[0].y(),r[0].z(),0,
	    r[1].x(),r[1].y(),r[1].z(),0,
	    r[2].x(),r[2].y(),r[2].z(),0,
	    0            , 0           , 0           ,1
	    );
*/
        btVector3 pos = worldTrans.getOrigin();
        m_obj->SetPosition(Vector3f(pos.x(), pos.y(), pos.z()) );
	m_obj->SetRotation(/*rot*/ worldTrans.getRotation() );
    }
};

static Texture* LoadTexture(const string& filename) {
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

GeometryObject::GeometryObject(): m_rigidBody(NULL) {}


GeometryObject* GeometryObject::Load(const std::string& filename, const Vector3f& position, PhysicsWorld* physicsWorld,
    unsigned int id) {


    GeometryObjectData* data = EobFile::Read(filename);

    if(!data) {
	return NULL;
    }

    GeometryObject* geoObj = new GeometryObject();
    geoObj->m_data = data;
    geoObj->m_id = id;
    geoObj->m_filename = filename;


    geoObj->SetPosition(position);
    geoObj->SetEditPosition( Vector3f(0.0f) );
    geoObj->SetRotation( btQuaternion::getIdentity() );
    geoObj->SetEditRotation( btQuaternion::getIdentity() );


    /*
      Load collision shape into physics engine.
     */

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

    string shaderName = "shader/geo_obj_render";



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

    geoObj->m_outlineShader = ShaderProgram::Load("shader/geo_obj_draw_outline");

    geoObj->m_depthShader = ShaderProgram::Load("shader/geo_obj_output_depth");

    geoObj->m_outputIdShader = ShaderProgram::Load("shader/geo_obj_output_id");


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

    Matrix4f modelMatrix = GetModelMatrix();

    const Matrix4f mvp = lightVp * modelMatrix;
    m_depthShader->SetUniform("mvp", mvp  );

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

    m_depthShader->Unbind();

}

void GeometryObject::RenderWithOutlines(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap) {

    // Enable stencil tests.
    GL_C(glEnable(GL_STENCIL_TEST));
    GL_C(glStencilFunc(GL_ALWAYS,1,1));
    GL_C(glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE));
    GL_C(glStencilMask(1));
    GL_C(glClearStencil(0));
    GL_C(glClear(GL_STENCIL_BUFFER_BIT));

    // render normally. All the pixels that are covered with the objects, will be assigned a value of 1
    // in the stencil buffer.
    Render(camera, lightPosition, lightVp,shadowMap);


    /*
      Only render where the corresponding pixel in the stencil buffer is zero.
      This ensures that the outline is drawn around the object.
     */
    GL_C(glStencilFunc(GL_EQUAL,0,1));
    GL_C(glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP));
    GL_C(glStencilMask(0x00));

    m_outlineShader->Bind();

    Matrix4f modelMatrix = GetModelMatrix( Matrix4f::CreateScale(Vector3f(1.05)) );

    const Matrix4f mvp = camera->GetVp() * modelMatrix;
    m_outlineShader->SetUniform("mvp", mvp  );

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

    m_outlineShader->Unbind();


    // render.

    GL_C(glDisable(GL_STENCIL_TEST));
}

void GeometryObject::RenderId(
	const ICamera* camera) {

    m_outputIdShader->Bind();

    Matrix4f modelMatrix = GetModelMatrix( );

    const Matrix4f mvp = camera->GetVp() * modelMatrix;
    m_outputIdShader->SetUniform("mvp", mvp  );
    m_outputIdShader->SetUniform("id", (float)m_id  );

    for(size_t i = 0; i < m_chunks.size(); ++i) {
	Chunk* chunk = m_chunks[i];

	VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
    }

    m_outputIdShader->Unbind();
}

void GeometryObject::Render(const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {

    Matrix4f modelMatrix = GetModelMatrix();

    m_defaultShader->Bind();

    m_defaultShader->SetPhongUniforms(

	modelMatrix
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
	modelMatrix *
	Matrix4f::CreateTranslation(center) *
	Matrix4f::CreateScale(radius)
	);


    m_aabbWireframe->Render(camera->GetVp());
}

AABB GeometryObject::GetModelSpaceAABB()const {

    AABB temp;


    Matrix4f modelMatrix = GetModelMatrix();

    temp.min = Vector3f((modelMatrix * Vector4f(m_aabb.min, 1.0f)));
    temp.max = Vector3f((modelMatrix * Vector4f(m_aabb.max, 1.0f)));

    return temp;
}

void GeometryObject::SetPosition(const Vector3f& position) {
    this->m_position = position;
}

void GeometryObject::SetRotation(const btQuaternion& rotation) {

    m_rotation = rotation;

//    this->SetModelMatrix(Matrix4f::CreateTranslation(m_position) * rotation );
}

/*
void GeometryObject::CreateCollisionShape(
    const CollisionShape* colShape, const EntityInfo* entityInfo, PhysicsWorld* physicsWorld) {

}
*/
void GeometryObject::ApplyCentralForce(const Vector3f& force) {
    if(m_rigidBody)
	m_rigidBody->applyCentralForce(toBtVec(force));

}

void GeometryObject::ApplyForce(const Vector3f& force, const Vector3f& relPos) {

    if(m_rigidBody)
	m_rigidBody->applyForce(toBtVec(force), toBtVec(relPos) );
}

btRigidBody* GeometryObject::GetRigidBody() const {
    return m_rigidBody;
}


btMotionState* GeometryObject::GetMotionState() const {
    return m_motionState;
}

Vector3f GeometryObject::GetPosition() const {
    return m_position;
}

void GeometryObject::AddToPhysicsWorld(PhysicsWorld* physicsWorld) {
/*    if(m_rigidBody) {
	LOG_I("rigidbody null");
	}*/

    CollisionShape* colShape = m_data->m_collisionShape;
    EntityInfo* entityInfo = m_data->m_entityInfo;

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
    } else if(colShape->m_shape == CylinderShape) {
	btShape = new btCylinderShape(toBtVec(colShape->m_halfExtents) );
    } else {

	LOG_E("undefined collision shape: %d", colShape->m_shape );

	return;
    }

    /*
      Create motion state
     */
    btTransform transform(m_rotation* toBtQuat(colShape->m_rotate), toBtVec(m_position + colShape->m_origin));
    m_motionState = new MyMotionState(transform, this);

    btVector3 inertia(0, 0, 0);

    // static objects dont move, so they have no intertia
    if(!entityInfo->m_isStatic) {
        btShape->calculateLocalInertia(entityInfo->m_mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo ci(entityInfo->m_mass, m_motionState, btShape, inertia);

    m_rigidBody = new btRigidBody(ci);

//    physicsWorld->AddRigidBody(m_rigidBody);

    physicsWorld->AddRigidBody(m_rigidBody);
}

Matrix4f GeometryObject::GetModelMatrix(const Matrix4f& scaling)const {
    return

	Matrix4f::CreateTranslation(m_position + m_editPosition) * // translate
	fromBtMat( btMatrix3x3(m_rotation * m_editRotation) ) * // rotate
    	scaling; // scale
}

void GeometryObject::SetEditPosition(const Vector3f& editPosition) {
    m_editPosition = editPosition;
}

void GeometryObject::SetEditRotation(const btQuaternion& editRotation) {
    m_editRotation = editRotation;
}

btQuaternion GeometryObject::GetRotation() const {
    return m_rotation;
}

std::string GeometryObject::GetFilename() const {
    return m_filename;
}
