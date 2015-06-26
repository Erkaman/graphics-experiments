#pragma once

class Quat4f {

public:

    float x,y,z,w;

    Quat4f(float x_, float y_, float z_, float w_): x(x_), y(y_),z(z_),w(w_){
        Normalize();
    }

   Quat4f& operator *=(const Quat4f& that){

        float w_temp = this->w*that.w - this->x*that.x - this->y*that.y - this->z*that.z;
        float x_temp = this->w*that.x + that.w*this->x + this->y*that.z - this->z*that.y;
        float y_temp = this->w*that.y + that.w*this->y - this->x*that.z + this->z*that.x;
        float z_temp = this->w*that.z + that.w*this->z + this->x*that.y - this->y*that.x;

	x = x_temp;
	y = y_temp;
	z = z_temp;
	w = w_temp;

        return (*this);
    }

    Quat4f& Conjugate(){
        x = -x;
        y = -y;
        z = -z;

        return *this;
    }

    Quat4f& Normalize(){
        float norm = (x*x + y*y + z*z + w*w);

        if (norm > 0.0f) {
            norm = 1.0f/(float)sqrt(norm);
            x = norm*x;
            y = norm*y;
            z = norm*z;
            w = norm*w;
        } else {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }
        return (*this);
    }

    operator std::string() const {
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z)  + ", " + std::to_string(w)+ ")";
    }

};
