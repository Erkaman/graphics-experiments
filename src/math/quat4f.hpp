#pragma once

#include <string>

class Quat4f {

public:

    /*
      INSTANCE VARIABLES
     */

    float x,y,z,w;

    /*
      CONSTRUCTORS
     */

    Quat4f(float x_, float y_, float z_, float w_);

    /*
      OPERATOR OVERLOADS
     */

    Quat4f& operator *=(const Quat4f& that);
    friend Quat4f operator *(const Quat4f& q1, const Quat4f& q2);

    /*
      INSTANCE METHODS.
     */

    Quat4f& Conjugate();
    Quat4f& Normalize();
    operator std::string() const;

};
