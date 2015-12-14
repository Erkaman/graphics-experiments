#pragma once

class ICamera {

public:

    virtual ~ICamera() {}

    virtual Matrix4f GetMvpFromM() const=0;
    virtual Matrix4f GetMvpFromM(const Matrix4f& m) const=0;

    virtual Matrix4f GetViewMatrix() const=0;

    virtual Matrix4f GetProjectionMatrix() const=0;


    virtual Matrix4f GetModelViewMatrix() const=0;
    virtual Matrix4f GetModelViewMatrix(const Matrix4f& modelMatrix) const=0;

    virtual Matrix4f GetVp() const =0;

    virtual Matrix4f GetMvpFromMv(const Matrix4f& modelViewMatrix) const=0;

    virtual Vector3f GetPosition() const=0;

    virtual void Update(const float delta)=0;
};
