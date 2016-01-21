#include "car_camera.hpp"

#include "config.hpp"

CarCamera::CarCamera(const int windowWidth, const int windowHeight, Car* car) {
    m_car = car;

    Config& config = Config::GetInstance();

    m_projectionMatrix =
	Matrix4f::CreatePerspective (45.0f, (float)windowWidth/(float)windowHeight, config.GetZNear(), config.GetZFar());
}


void CarCamera::Update(const float) {

    UpdateViewMatrix();

}

Vector3f CarCamera::GetPosition() const {
    return m_car->GetPosition();
}

void CarCamera::UpdateViewMatrix() {
    Vector3f backward = -m_car->GetForwardVector();

    Vector3f carPos = GetPosition();

    Vector3f cameraPos = carPos +
	backward * 40.0 +  // how far behind the car to position the camera.
	Vector3f(0,20,0); // camera height.

    m_viewMatrix =
	Matrix4f::CreateLookAt(
	    cameraPos,
	    carPos,
	    Vector3f(0.0, 1.0f, 0.0f) );
}
