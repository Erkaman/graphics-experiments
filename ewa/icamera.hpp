#pragma once

#include "math/vector3f.hpp"
#include "math/matrix4f.hpp"

#include "log.hpp"

class ICamera {

protected:

    Matrix4f m_projectionMatrix;
    Matrix4f m_viewMatrix;

public:

    virtual ~ICamera() {}

    virtual Matrix4f GetVp() const;
    virtual Matrix4f GetMvp(const Matrix4f& m) const;

    virtual Matrix4f GetViewMatrix() const;
    virtual Matrix4f GetProjectionMatrix() const;

    virtual Vector3f GetPosition() const=0;
    virtual Vector3f GetViewDir() const {LOG_E("GetViewDir not yet implemented!");}

    virtual void Update(const float delta)=0;


};
