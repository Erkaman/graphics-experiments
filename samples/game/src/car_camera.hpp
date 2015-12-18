#pragma once

#include "ewa/icamera.hpp"


#include "car.hpp"

class CarCamera : public ICamera{

private:

    Car* m_car;

    void UpdateViewMatrix();

public:

    CarCamera(const int windowWidth, const int windowHeight, Car* car);

    virtual void Update(const float);

    virtual Vector3f GetPosition() const;
};
