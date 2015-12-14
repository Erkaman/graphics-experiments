#pragma once

class ICamera {

public:

    virtual ~ICamera() {}

    virtual Matrix4f GetVp() const=0;
    virtual Matrix4f GetMvp(const Matrix4f& m) const=0;

    virtual Matrix4f GetViewMatrix() const=0;
    virtual Matrix4f GetProjectionMatrix() const=0;


    virtual Vector3f GetPosition() const=0;

    virtual void Update(const float delta)=0;
};
