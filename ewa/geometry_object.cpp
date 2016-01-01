#include "geometry_object.hpp"

#include "math/vector3f.hpp"
#include "math/vector4f.hpp"

#include "ewa/gl/vbo.hpp"
#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"

#include "ewa/bt_util.hpp"

#include "ewa/common.hpp"
#include "ewa/view_frustum.hpp"
#include "ewa/icamera.hpp"
#include "ewa/file.hpp"
#include "ewa/cube.hpp"
#include "eob_file.hpp"
#include "resource_manager.hpp"
#include "config.hpp"
#include "physics_world.hpp"
#include "gl/depth_fbo.hpp"

#include <btBulletDynamicsCommon.h>

#include <map>

using std::string;
using std::vector;
using std::map;

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

struct Chunk {
    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;
    GLuint m_numTriangles;

    float m_shininess;
    Vector3f m_specularColor;
};

// contains the info needed to render a batch of GeoObjs
struct GeoObjBatch{
public:

    std::vector<Chunk*> m_chunks;

    GeometryObjectData* m_data;

    ShaderProgram* m_defaultShader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.

    bool m_hasHeightMap;

    // the geoObjs that needs the info of this struct to be rendered.
    vector<GeometryObject*> m_geoObjs;


    // the material.
    Texture* m_texture;
    Texture* m_normalMap;
    Texture* m_specularMap;

};

class GeoObjManager {

private:


    GeoObjManager() {
	m_outputIdShader = ShaderProgram::Load("shader/geo_obj_output_id");

	m_outlineShader = ShaderProgram::Load("shader/geo_obj_draw_outline");

	m_outputDepthShader = ShaderProgram::Load("shader/geo_obj_output_depth");

	m_aabbWireframe = Cube::Load();
    }

public:
    Cube* m_aabbWireframe;


    map<string, GeoObjBatch*> m_batches;

    ShaderProgram* m_outputIdShader;

    ShaderProgram* m_outlineShader;

    ShaderProgram* m_outputDepthShader;




    static GeoObjManager& GetInstance(){
	static GeoObjManager instance;

	return instance;
    }

    GeometryObjectData* LoadObj(const std::string& filename, GeometryObject* geoObj) {

	// check if already a batch for this object has been created.
	map<string, GeoObjBatch*>::iterator it = m_batches.find(filename);
	if(it != m_batches.end() ) {

	    // add it as an object to be batched
	    it->second->m_geoObjs.push_back(geoObj);



	    return it->second->m_data;
	}

	// else, we load the object and create a batch for that object.
	GeoObjBatch* geoObjBatch = new GeoObjBatch();
	GeometryObjectData* data = EobFile::Read(filename);
	geoObjBatch->m_data = data;

	m_batches[filename] = geoObjBatch;
	geoObjBatch->m_geoObjs.push_back(geoObj);

	if(!data) {
	    return NULL;
	}


	/*
	  Load the textures of the object.
	 */

	string basePath = File::GetFilePath(filename);

	geoObjBatch->m_hasHeightMap = false;

	geoObjBatch->m_texture = NULL;
	geoObjBatch->m_normalMap = NULL;
	geoObjBatch->m_specularMap = NULL;

	for(size_t i = 0; i < data->m_chunks.size(); ++i) {
	    Material* mat = data->m_chunks[i]->m_material;

	    if(mat->m_textureFilename != ""){ // empty textures should remain empty.

		geoObjBatch->m_texture = LoadTexture(File::AppendPaths(basePath, mat->m_textureFilename));

		if(!geoObjBatch->m_texture) {
		    // loading texture failed.
		    return NULL;
		}

	    }

	    if(mat->m_normalMapFilename != ""){ // empty textures should remain empty->


		geoObjBatch->m_normalMap = LoadTexture(File::AppendPaths(basePath, mat->m_normalMapFilename));
		if(!geoObjBatch->m_normalMap) {
		    return NULL;
		}

		geoObjBatch->m_hasHeightMap = mat->m_hasHeightMap;
	    }

	    if(mat->m_specularMapFilename != ""){ // empty textures should remain empty->



		geoObjBatch->m_specularMap = LoadTexture(File::AppendPaths(basePath, mat->m_specularMapFilename));

		if(!geoObjBatch->m_specularMap) {
		    return NULL;
		}


	    }
	}

	string shaderName = "shader/geo_obj_render";

	/*
	  Next, we create a shader that supports all the texture types.
	 */


	vector<string> defines;

	if(geoObjBatch->m_specularMap) {
	    defines.push_back("SPEC_MAPPING");
	}

	if(geoObjBatch->m_hasHeightMap) {
	    defines.push_back("HEIGHT_MAPPING");
	} else if(geoObjBatch->m_normalMap) { // only a normal map, no height map.
	    defines.push_back("NORMAL_MAPPING");
	}

	geoObjBatch->m_defaultShader = ResourceManager::LoadShader(shaderName + "_vs.glsl", shaderName + "_fs.glsl", defines);




	/*
	  Next, we create VBOs from the vertex data in the chunks.
	 */
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


	    newChunk->m_shininess = baseChunk->m_material->m_shininess;
	    newChunk->m_specularColor = baseChunk->m_material->m_specularColor;

	    geoObjBatch->m_chunks.push_back(newChunk);
	}
	return data;
    }

};


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

        btVector3 pos = worldTrans.getOrigin();
        m_obj->SetPosition(Vector3f(pos.x(), pos.y(), pos.z()) );
	m_obj->SetRotation(/*rot*/ worldTrans.getRotation() );
    }
};

bool GeometryObject::Init(
    const std::string& filename,
    const Vector3f& position,
    const btQuaternion& rotation,
    unsigned int id) {

    m_id = id;
    m_filename = filename;

    m_culled = false;
    SetSelected(false);
    SetPosition(position);
    SetEditPosition( Vector3f(0.0f) );
    SetRotation( rotation  );
    SetEditRotation( btQuaternion::getIdentity() );

    GeometryObjectData* data = GeoObjManager::GetInstance().LoadObj(filename, this);



    if(!data) {
	return false;
    }

    /*
      Bounding Volume
    */
    m_aabb = data->aabb;


    m_data = data;



    return true;

}

GeometryObject::~GeometryObject() {
/*
  for(size_t i = 0; i < m_chunks.size(); ++i) {
  Chunk* chunk = m_chunks[i];

  MY_DELETE(chunk->m_vertexBuffer);
  MY_DELETE(chunk->m_indexBuffer);

  MY_DELETE(chunk);
  }*/
    // should instead do this in GeoObjManager.
}

void GeometryObject::RenderShadowMap(const Matrix4f& lightVp) {
/*
    auto& batches = GeoObjManager::GetInstance().GetBatches();

    ShaderProgram* outputDepthShader = GeoObjManager::GetInstance().m_outputDepthShader;

    // bind shader of all the batches.
    outputDepthShader->Bind();

    // render all batches, one after one.
    for(auto& itBatch : batches) {

	const GeoObjBatch* batch = itBatch.second;

	// render the objects of the batch, one after one.
	for(GeometryObject* geoObj : batch->m_geoObjs ) {

	    if(geoObj->m_culled) {
		continue; // if culled, do nothing.
	    }

	    Matrix4f modelMatrix = geoObj->GetModelMatrix( );


	    const Matrix4f mvp = lightVp * modelMatrix;
	    outputDepthShader->SetUniform("mvp", mvp  );


	    for(size_t i = 0; i < batch->m_chunks.size(); ++i) {

		Chunk* chunk = batch->m_chunks[i];

		VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
	    }

	}

    }

    outputDepthShader->Unbind();
*/



/*
  m_depthShader->Bind();

  Matrix4f modelMatrix = GetModelMatrix();

  const Matrix4f mvp = lightVp * modelMatrix;
  m_depthShader->SetUniform("mvp", mvp  );

  for(size_t i = 0; i < m_chunks.size(); ++i) {
  Chunk* chunk = m_chunks[i];

  VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
  }

  m_depthShader->Unbind();
*/
}


void GeometryObject::RenderIdAll(
    const ICamera* camera) {

    map<string, GeoObjBatch*>& batches = GeoObjManager::GetInstance().m_batches;

    ShaderProgram* outputIdShader = GeoObjManager::GetInstance().m_outputIdShader;

    // bind shader of all the batches.
    outputIdShader->Bind();

    // render all batches, one after one.
    for(auto& itBatch : batches) {

	const GeoObjBatch* batch = itBatch.second;

	// render the objects of the batch, one after one.
	for(GeometryObject* geoObj : batch->m_geoObjs ) {

	    if(geoObj->m_culled) {
		continue; // if culled, do nothing.
	    }

	    Matrix4f modelMatrix = geoObj->GetModelMatrix( );

	    const Matrix4f mvp = camera->GetVp() * modelMatrix;
	    outputIdShader->SetUniform("mvp", mvp  );
	    outputIdShader->SetUniform("id", (float)geoObj->m_id  );

	    for(size_t i = 0; i < batch->m_chunks.size(); ++i) {

		Chunk* chunk = batch->m_chunks[i];


		VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
	    }

	}

    }

    outputIdShader->Unbind();

}

void GeometryObject::RenderAll(const ICamera* camera, const Vector4f& lightPosition, const Matrix4f& lightVp, const DepthFBO& shadowMap) {

    int total = 0;
    int nonCulled = 0;


    auto& batches = GeoObjManager::GetInstance().m_batches;

    GeometryObject* selectedObj = NULL;
    const GeoObjBatch* selectedBatch = NULL;

    // render all batches, one after one.
    for(auto& itBatch : batches) {

	const GeoObjBatch* batch = itBatch.second;

	// bind shader of the batch.
	batch->m_defaultShader->Bind();

	batch->m_defaultShader->SetUniform("shadowMap", (int)shadowMap.GetTargetTextureUnit() );
	Texture::SetActiveTextureUnit(shadowMap.GetTargetTextureUnit());
	shadowMap.GetRenderTargetTexture().Bind();


	if(batch->m_hasHeightMap) {
	    Config& config = Config::GetInstance();

	    batch->m_defaultShader->SetUniform("zNear", config.GetZNear());
	    batch->m_defaultShader->SetUniform("zFar", config.GetZFar());
	}

	if(batch->m_texture != NULL) {
	    batch->m_defaultShader->SetUniform("diffMap", 0);
	    Texture::SetActiveTextureUnit(0);
	    batch->m_texture->Bind();
	}

	if(batch->m_normalMap != NULL) {
	    batch->m_defaultShader->SetUniform("normalMap", 1);
	    Texture::SetActiveTextureUnit(1);
	    batch->m_normalMap->Bind();
	}

	if(batch->m_specularMap != NULL) {
	    batch->m_defaultShader->SetUniform("specMap", 2);
	    Texture::SetActiveTextureUnit(2);
	    batch->m_specularMap->Bind();
	}

	// render the objects of the batch, one after one.
	for(GeometryObject* geoObj : batch->m_geoObjs ) {

	    ++total;

	    if(geoObj->m_culled) {
		continue; // if culled, do nothing.
	    }

	    ++nonCulled;

	    Matrix4f modelMatrix = geoObj->GetModelMatrix();

	    batch->m_defaultShader->SetPhongUniforms(
		modelMatrix
		, camera, lightPosition,
		lightVp);

	    if(geoObj->IsSelected() ) {
		// for the selected object, we also render to the stencil buffer
		// so that we later can draw an outline.

		GL_C(glEnable(GL_STENCIL_TEST));
		GL_C(glStencilFunc(GL_ALWAYS,1,1));
		GL_C(glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE));
		GL_C(glStencilMask(1));
		GL_C(glClearStencil(0));
		GL_C(glClear(GL_STENCIL_BUFFER_BIT));

		selectedObj = geoObj;
		selectedBatch = batch;
	    }

	    for(size_t i = 0; i < batch->m_chunks.size(); ++i) {

		Chunk* chunk = batch->m_chunks[i];

		if(batch->m_specularMap == NULL) {
		    // if no spec map, the chunk has the same specular color all over the texture.
		    batch->m_defaultShader->SetUniform("specColor", chunk->m_specularColor);
		}

		batch->m_defaultShader->SetUniform("specShiny", chunk->m_shininess);

		VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
	    }

	    if(geoObj->IsSelected() ) {
		// dont render to stencil buffer for remaining objects.
		GL_C(glStencilMask(0));
	    }

	}

	if(batch->m_texture != NULL) {
	    batch->m_texture->Unbind();
	}

	if(batch->m_normalMap != NULL) {
	    batch->m_normalMap->Unbind();
	}

	if(batch->m_specularMap != NULL) {
	    batch->m_specularMap->Unbind();
	}

	shadowMap.GetRenderTargetTexture().Unbind();
	batch->m_defaultShader->Unbind();
    }

    if(selectedObj != NULL) {
	// also render outline for selected object.
	GL_C(glStencilFunc(GL_EQUAL,0,1));
	GL_C(glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP));
	GL_C(glStencilMask(0x00));


	ShaderProgram* outlineShader = GeoObjManager::GetInstance().m_outlineShader;

	outlineShader->Bind();

	Matrix4f modelMatrix = selectedObj->GetModelMatrix( Matrix4f::CreateScale(Vector3f(1.05)) );

	const Matrix4f mvp = camera->GetVp() * modelMatrix;
	outlineShader->SetUniform("mvp", mvp  );

	for(size_t i = 0; i < selectedBatch->m_chunks.size(); ++i) {
	    Chunk* chunk = selectedBatch->m_chunks[i];

	    VBO::DrawIndices(*chunk->m_vertexBuffer, *chunk->m_indexBuffer, GL_TRIANGLES, (chunk->m_numTriangles)*3);
	}

	outlineShader->Unbind();

	GL_C(glDisable(GL_STENCIL_TEST));
    }

    /*
      Render the AABBs of all objects
    */



    // render all batches, one after one.
    for(auto& itBatch : batches) {

	const GeoObjBatch* batch = itBatch.second;

	// render the objects of the batch, one after one.
	for(GeometryObject* geoObj : batch->m_geoObjs ) {

	    Vector3f center = (geoObj->m_aabb.min + geoObj->m_aabb.max) * 0.5f;
	    Vector3f radius = geoObj->m_aabb.max - center;
	    Matrix4f modelMatrix = geoObj->GetModelMatrix();

	    Cube* aabbWireframe = GeoObjManager::GetInstance().m_aabbWireframe;

	    aabbWireframe->SetModelMatrix(
		modelMatrix *
		Matrix4f::CreateTranslation(center) *
		Matrix4f::CreateScale(radius)
		);


	    aabbWireframe->Render(camera->GetVp());

	}
	}

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
}

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

bool GeometryObject::IsSelected()const {
    return m_selected;
}

void GeometryObject::SetSelected(bool selected) {
    m_selected = selected;
}

void GeometryObject::Update(const ViewFrustum& viewFrustum) {

    m_culled = !viewFrustum.IsAABBInFrustum(GetModelSpaceAABB());
}

IGeometryObject* GeometryObject::Duplicate(unsigned int id) {
    GeometryObject* newObj = new GeometryObject();

    newObj->Init(m_filename, m_position, m_rotation, id);

    return newObj;
}


unsigned int GeometryObject::GetId() {
    return m_id;
}


void GeometryObject::Delete(IGeometryObject* geoObj) {

    // remove the obj from its corresponding batch:

    // find the batch:x
    GeoObjBatch* batch = GeoObjManager::GetInstance().m_batches[geoObj->GetFilename()];


    vector<GeometryObject*>::iterator it;

    for(it = batch->m_geoObjs.begin(); it !=batch->m_geoObjs.end(); ++it ) {

	if(*it == geoObj) {
	    break;
	}
    }

    batch->m_geoObjs.erase(it);

}
