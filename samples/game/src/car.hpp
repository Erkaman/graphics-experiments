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

      GL_TEXTURE_CUBE_MAP_POSITIVE_X
      GL_TEXTURE_CUBE_MAP_NEGATIVE_X
      GL_TEXTURE_CUBE_MAP_POSITIVE_Y
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
      GL_TEXTURE_CUBE_MAP_POSITIVE_Z
      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
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
	const ViewFrustum* cameraFrustum, const ViewFrustum* lightFrustum, ViewFrustum** envLightFrustums);

    Vector3f GetForwardVector()const;

    virtual void AddToPhysicsWorld(PhysicsWorld* physicsWorld);

    ViewFrustum** GetLightFrustums();

};

//could be that the heightmap normal calulcaiton does not handle steep variations in terrain, like game gems said.
