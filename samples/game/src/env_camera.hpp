#pragma once

#include "ewa/icamera.hpp"
#include "ewa/gl/gl_common.hpp"

// camera used for environment mapping.
class EnvCamera : public ICamera{

private:

    Vector3f m_position;

    int m_i;

public:

    EnvCamera(const Vector3f& position, int i);

    virtual void Update(const float delta);
    void SetPosition(const Vector3f& position);

    virtual Vector3f GetPosition() const;
};
