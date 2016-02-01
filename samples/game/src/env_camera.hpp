#pragma once

#include "ewa/icamera.hpp"
#include "ewa/gl/gl_common.hpp"

// camera used for environment mapping.
class EnvCamera : public ICamera{

private:

    Vector3f m_position;

    Vector3f m_right;
    Vector3f m_up;
    Vector3f m_forward;

public:

    EnvCamera(GLenum target, const Vector3f& position);

    virtual void Update(const float delta);
    void SetPosition(const Vector3f& position);

    virtual Vector3f GetPosition() const;
};
