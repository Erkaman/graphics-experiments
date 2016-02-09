#pragma once

#include "ewa/geometry_object.hpp"

class PhysicsWorld;
struct btVehicleRaycaster;
class btRaycastVehicle;
class EnvCamera;


class Car : public GeometryObject{

private:

    btVehicleRaycaster* m_vehicleRaycaster;
    btRaycastVehicle* m_raycastVehicle;

    /*
      the cameras are stored in the order:

      GL_TEXTURE_CUBE_MAP_POSITIVE_X 0
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X 1
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y 2
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 3
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z 4
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 5
    */
    EnvCamera *m_envCameras[6];


    ViewFrustum* m_lightFrustums[6];

public:
    Car();

    bool Init(const Vector3f& position);

    virtual ~Car();

    /*
    virtual void Render(
	const ICamera* camera,
	const Vector4f& lightPosition,
	const Matrix4f& lightVp,
	const DepthFBO& shadowMap);
    */

    virtual void Update(
	const ViewFrustum* cameraFrustum, const ViewFrustum* lightFrustum, ViewFrustum** envLightFrustums,
	const ViewFrustum* reflectionFrustum);

    Vector3f GetForwardVector()const;

    virtual void AddToPhysicsWorld(PhysicsWorld* physicsWorld);

    ViewFrustum** GetLightFrustums();
    EnvCamera** GetEnvCameras();

};

//could be that the heightmap normal calulcaiton does not handle steep variations in terrain, like game gems said.
