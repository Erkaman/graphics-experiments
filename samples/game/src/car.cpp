#include "car.hpp"

#include "ewa/geometry_object.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/physics_world.hpp"

#include "ewa/bt_util.hpp"
#include "ewa/common.hpp"

#include "physics_mask.hpp"


#include <btBulletDynamicsCommon.h>

// hard coded for now.
const btVector3 CAR_DIMENSIONS(4.0f, 2.0f, 8.0f);

const float MASS_OFFSET = -0.2f;
const float WHEEL_RADIUS = 0.3f;
const float FRONT_WHEEL_FRICTION = 1.9f;
const float BACK_WHEEL_FRICTION = 1.9f;
const float SUSPENSION_REST_LENGTH = 0.6f; // (see also maxSuspensionTravelCm)
const float ROLL_INFLUENCE = 0.4f;
const unsigned int CAR_ID = 1337;

float	maxEngineForce = 1000.f;

const btVector3 FRONT_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, (CAR_DIMENSIONS.z()/2 - 0.3f - WHEEL_RADIUS));
const btVector3 BACK_WHEEL_DISTANCE(CAR_DIMENSIONS.x()/2 - 0.1f, MASS_OFFSET, -(CAR_DIMENSIONS.z()/2 - 0.1f - WHEEL_RADIUS));


bool Car::Init(const Vector3f& position) {

    bool ret = GeometryObject::Init("obj/car.eob",
				    position, // translation
				    btQuaternion::getIdentity(), // rotation
				    1.0f, // scale
				    CAR_ID,

				    COL_CAR,
				    carCollidesWith
	);

	return ret;
}

Car::Car(): m_raycastVehicle(NULL) {
}


Car::~Car() {
}

void Car::Update(const ViewFrustum& cameraFrustum, const ViewFrustum& lightFrustum) {

    GeometryObject::Update(cameraFrustum, lightFrustum);

    if(!m_raycastVehicle) {
	// if not yet added to physicsworld, there is nothing we can do.
	return;
    }

    const KeyboardState& kbs = KeyboardState::GetInstance();


    const float steeringClamp = 0.3f;
    const float steeringIncrement = 0.04f;

    float engineForce = 0.0f;
    float breakingForce = 0.0f;

    static float vehicleSteering = 0.0;

    if( kbs.IsPressed(GLFW_KEY_UP) ) {
	engineForce = maxEngineForce;
	breakingForce = 0.f;
    } else if(kbs.IsPressed(GLFW_KEY_DOWN)  ) {
	engineForce = -maxEngineForce;
	breakingForce = 0.f;
    }

    if( kbs.IsPressed(GLFW_KEY_LEFT) ) {

	vehicleSteering += steeringIncrement;
	if (vehicleSteering > steeringClamp)
	    vehicleSteering = steeringClamp;
    } else if( kbs.IsPressed(GLFW_KEY_RIGHT) ) {

	vehicleSteering -= steeringIncrement;
	if (vehicleSteering < -steeringClamp)
	    vehicleSteering = -steeringClamp;


    } else {
	vehicleSteering = 0;
    }

    m_raycastVehicle->applyEngineForce(engineForce,2);
    m_raycastVehicle->setBrake(breakingForce,2);
    m_raycastVehicle->applyEngineForce(engineForce,3);
    m_raycastVehicle->setBrake(breakingForce,3);

    m_raycastVehicle->setSteeringValue(vehicleSteering,0);
    m_raycastVehicle->setSteeringValue(vehicleSteering,1);

}

Vector3f Car::GetForwardVector()const {
    if(!m_raycastVehicle) {
	return Vector3f(1,0,0); // default vector.
    } else {
	return fromBtVec(m_raycastVehicle->getForwardVector());
    }
}

void Car::AddToPhysicsWorld(PhysicsWorld* physicsWorld) {

    GeometryObject::AddToPhysicsWorld(physicsWorld);
    btRigidBody* rigidBody = GetRigidBody();


    rigidBody->setActivationState(DISABLE_DEACTIVATION); // Needed for btRaycastVehicles

    // Tuning for car (applied to all the wheels internally).
    btRaycastVehicle::btVehicleTuning tuning;
    tuning.m_suspensionStiffness = 20.0f;
    tuning.m_suspensionDamping = 2.3f;
    tuning.m_suspensionCompression = 4.4f;

    m_vehicleRaycaster = physicsWorld->NewVehicleRaycaster();
    m_raycastVehicle = new btRaycastVehicle(tuning, rigidBody, m_vehicleRaycaster);
    m_raycastVehicle->setCoordinateSystem(0, 1, 2);

    physicsWorld->AddVehicle(m_raycastVehicle);

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

/*
:m_suspensionStiffness(btScalar(5.88)),
stiffness: N/m

     m_suspensionCompression(btScalar(0.83)),
measures in meters.

     m_suspensionDamping(btScalar(0.88)),
     N s / m

     m_maxSuspensionTravelCm(btScalar(500.)),
     cm

     m_frictionSlip(btScalar(10.5)),


     m_maxSuspensionForce(btScalar(6000.))
*/


/*m_maxSuspensionTravelCm could be preventing it from going further. Be sure to multiply by scale!


m_frictionSlip, is friciton, and friciton is unitless i I think.
*/
