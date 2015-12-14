#include "car.hpp"

#include "ewa/geometry_object.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/physics_world.hpp"

#include <btBulletDynamicsCommon.h>


// hard coded for now.
//const btVector3 CAR_DIMENSIONS(8.0f, 2.0f, 4.0f);

const btVector3 CAR_DIMENSIONS(4.0f, 2.0f, 8.0f);

// change it to 4, 2, 8.

const float MASS_OFFSET = -0.2f;

const float WHEEL_RADIUS = 0.3; // 0.5f

const float FRONT_WHEEL_FRICTION = 2.0f;
const float BACK_WHEEL_FRICTION = 1.9f;

float	steeringClamp = 0.3f;

const float SUSPENSION_REST_LENGTH = 0.6f; // (see also maxSuspensionTravelCm)

const float ROLL_INFLUENCE = 0.1f;

float	steeringIncrement = 0.04f;

float	maxEngineForce = 1000.f;//this should be engine/velocity dependent
float	maxBreakingForce = 100.f;

const btVector3 FRONT_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, (CAR_DIMENSIONS.z()/2 - 0.3f - WHEEL_RADIUS));
const btVector3 BACK_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, -(CAR_DIMENSIONS.z()/2 - 0.1f - WHEEL_RADIUS));


Car* Car::Load(PhysicsWorld* physicsWorld) {

    IGeometryObject* geoObj = GeometryObject::Load("obj/car.eob",
						   Vector3f(0,-1.5,0),
						   physicsWorld );

    if(!geoObj) {
	return NULL;
    }

    return new Car(geoObj, physicsWorld);
}

Car::Car(IGeometryObject* geoObj, PhysicsWorld* physicsWorld) {

    m_geoObj = geoObj;

    btRigidBody* rigidBody = m_geoObj->GetRigidBody();

    /*

      TODO: WE MAY HAVE TO DO THIS EARLIER.

    */
//    rigidBody->setActivationState(DISABLE_DEACTIVATION); // Needed for btRaycastVehicles

    // Tuning for car (applied to all the wheels internally).
    btRaycastVehicle::btVehicleTuning tuning;
    tuning.m_suspensionStiffness = 20.0f;
    tuning.m_suspensionDamping = 2.3f;
    tuning.m_suspensionCompression = 4.4f;
    // tuning.m_frictionSlip set below for each wheel
    // rollInfluence cannot be set here, so we have to apply it to every wheel individually.

    // Create the raycasting part for the "wheels"
    m_vehicleRaycaster = physicsWorld->NewVehicleRaycaster(); // new btDefaultVehicleRaycaster(dynamicsWorld);
    m_raycastVehicle = new btRaycastVehicle(tuning, rigidBody, m_vehicleRaycaster);
    m_raycastVehicle->setCoordinateSystem(0, 1, 2);

    physicsWorld->AddVehicle(m_raycastVehicle);

    // Create the actual wheels:
    const btVector3 wheelDirection(0, -1.0f, 0); // This is the direction of the raycast.
    const btVector3 wheelAxle(-1.0f, 0, 0); // This is spinning direction (using right hand rule).

    // Right front wheel. 0
    m_raycastVehicle->addWheel(
	FRONT_WHEEL_DISTANCE, // connection point
	wheelDirection, // wheel direction
	wheelAxle, // axle
	SUSPENSION_REST_LENGTH,
	WHEEL_RADIUS,
	tuning,
	true // isFrontWheel
        );
    btWheelInfo& rightFrontWheel = m_raycastVehicle->getWheelInfo(0);
    rightFrontWheel.m_rollInfluence = ROLL_INFLUENCE;
    rightFrontWheel.m_frictionSlip = FRONT_WHEEL_FRICTION;

    // Left front wheel. 1
    m_raycastVehicle->addWheel(
	btVector3(-FRONT_WHEEL_DISTANCE.x(), FRONT_WHEEL_DISTANCE.y(), FRONT_WHEEL_DISTANCE.z()), // connection point
	wheelDirection, // wheel direction
	wheelAxle, // axle
	SUSPENSION_REST_LENGTH,
	WHEEL_RADIUS,
	tuning,
	true // isFrontWheel
        );
    btWheelInfo& leftFrontWheel = m_raycastVehicle->getWheelInfo(1);
    leftFrontWheel.m_rollInfluence = ROLL_INFLUENCE;
    leftFrontWheel.m_frictionSlip = FRONT_WHEEL_FRICTION;

    // Right back wheel. 2
    m_raycastVehicle->addWheel(
	BACK_WHEEL_DISTANCE, // connection point
	wheelDirection, // wheel direction
	wheelAxle, // axle
	SUSPENSION_REST_LENGTH,
	WHEEL_RADIUS,
	tuning,
	false // isFrontWheel
        );
    btWheelInfo& rightBackWheel = m_raycastVehicle->getWheelInfo(2);
    rightBackWheel.m_rollInfluence = ROLL_INFLUENCE;
    rightBackWheel.m_frictionSlip = BACK_WHEEL_FRICTION;

    // Left back wheel. 3
    m_raycastVehicle->addWheel(
	btVector3(-BACK_WHEEL_DISTANCE.x(), BACK_WHEEL_DISTANCE.y(), BACK_WHEEL_DISTANCE.z()), // connection point
	wheelDirection, // wheel direction
	wheelAxle, // axle
	SUSPENSION_REST_LENGTH,
	WHEEL_RADIUS,
	tuning,
	false // isFrontWheel
        );
    btWheelInfo& leftBackWheel = m_raycastVehicle->getWheelInfo(3);
    leftBackWheel.m_rollInfluence = ROLL_INFLUENCE;
    leftBackWheel.m_frictionSlip = BACK_WHEEL_FRICTION;

}


Car::~Car() {
    delete m_geoObj;
}

void Car::Render(
    const Camera& camera,
    const Vector4f& lightPosition,
    const Matrix4f& lightVp,
    const DepthFBO& shadowMap) {
    m_geoObj->Render(camera, lightPosition, lightVp, shadowMap);
}

void Car::RenderShadowMap(const Matrix4f& lightVp) {
    m_geoObj->RenderShadowMap(lightVp);
}

AABB Car::GetModelSpaceAABB()const {
    return m_geoObj->GetModelSpaceAABB();
}


void Car::Update(const float delta) {
//    LOG_I("car update");

    const KeyboardState& kbs = KeyboardState::GetInstance();

    float gEngineForce = 0.0f;
    float gBreakingForce = 0.0f;

    static float gVehicleSteering = 0.0;

    if( kbs.IsPressed(GLFW_KEY_UP) ) {
	gEngineForce = maxEngineForce;
	gBreakingForce = 0.f;
    } else if(kbs.IsPressed(GLFW_KEY_DOWN)  ) {
	gEngineForce = -maxEngineForce;
	gBreakingForce = 0.f;
    }

    if( kbs.IsPressed(GLFW_KEY_LEFT) ) {

	gVehicleSteering += steeringIncrement;
	if (gVehicleSteering > steeringClamp)
	    gVehicleSteering = steeringClamp;
    } else if( kbs.IsPressed(GLFW_KEY_RIGHT) ) {

	gVehicleSteering -= steeringIncrement;
	if (	gVehicleSteering < -steeringClamp)
	    gVehicleSteering = -steeringClamp;


    } else {
	gVehicleSteering = 0;
    }

    int wheelIndex = 2;
    m_raycastVehicle->applyEngineForce(gEngineForce,wheelIndex);
    m_raycastVehicle->setBrake(gBreakingForce,wheelIndex);
    wheelIndex = 3;
    m_raycastVehicle->applyEngineForce(gEngineForce,wheelIndex);
    m_raycastVehicle->setBrake(gBreakingForce,wheelIndex);


    wheelIndex = 0;
    m_raycastVehicle->setSteeringValue(gVehicleSteering,wheelIndex);
    wheelIndex = 1;
    m_raycastVehicle->setSteeringValue(gVehicleSteering,wheelIndex);
}
