#pragma once

#include <string>
#include "common.hpp"
#include "math_common.hpp"

class Vector3f {

public:

    float x,y,z;

public:

    Vector3f():x(0.0f),y(0.0f),z(0.0f) {  }

    Vector3f(const float value_): x(value_), y(value_),z(value_){  }

    Vector3f(const float x_, const float y_, const float z_): x(x_),y(y_),z(z_){ }

    Vector3f(const Vector3f& that): x(that.x), y(that.y), z(that.z){ }

    friend Vector3f operator+(const Vector3f& v1, const Vector3f& v2);
    friend bool operator==(const Vector3f& v1, const Vector3f& v2);
    friend bool operator!=(const Vector3f& v1, const Vector3f& v2);

    operator std::string() const {
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }


/*
  public Vector3f add(final Vector3f that){
  this.x += that.x;
  this.y += that.y;
  this.z += that.z;
  return this;
  }

  public static Vector3f add(final Vector3f a, final Vector3f b) {
  return new Vector3f(a.x + b.x, a.y + b.y, a.z + b.z);
  }

  public Vector3f add(final Vector3i that){
  this.x += that.x;
  this.y += that.y;
  this.z += that.z;
  return this;
  }

  public Vector3f sub(final Vector3f that){
  this.x -= that.x;
  this.y -= that.y;
  this.z -= that.z;
  return this;
  }

  public static Vector3f sub(final Vector3f a, final Vector3f b) {
  return new Vector3f(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  public void setSub(final Vector3f a, final Vector3f b) {
  this.x = a.x - b.x;
  this.y = a.y - b.y;
  this.z = a.z - b.z;
  }

  public Vector3f mul(final Vector3f that){
  this.x *= that.x;
  this.y *= that.y;
  this.z *= that.z;
  return this;
  }

  public static Vector3f mul(final Vector3f a, final Vector3f b) {
  return new Vector3f(a.x * b.x, a.y * b.y, a.z * b.z);
  }

  public Vector3f div(final Vector3f that){
  this.x /= that.x == 0 ? 1 : that.x;
  this.y /= that.y == 0 ? 1 : that.x;
  this.z /= that.z == 0 ? 1 : that.x;
  return this;
  }

  public static Vector3f div(final Vector3f a, final Vector3f b){
  return new Vector3f(a.x / b.x, a.y / b.y, a.z / b.z);
  }

  public float dot(final Vector3f that){
  return this.x * that.x + this.y * that.y + this.z * that.z;
  }

  public Vector3f cross(final Vector3f that){

  this.x = this.y * that.z - that.y * this.z;
  this.y = that.x * this.z - this.x * that.z;
  this.z = this.x * that.y - that.x * this.y;
  return this;
  }

  public static Vector3f cross(final Vector3f a, final Vector3f b){

  float x, y, z;

  x = a.y * b.z - b.y * a.z;
  y = b.x * a.z - a.x * b.z;
  z = a.x * b.y - b.x * a.y;

  return new Vector3f(x, y, z);
  }


  public void setCross(final Vector3f a, final Vector3f b){
  this.x = a.y * b.z - b.y * a.z;
  this. y = b.x * a.z - a.x * b.z;
  this.z = a.x * b.y - b.x * a.y;
  }

  public Vector3f scale(final float amount){
  this.x *= amount;
  this.y *= amount;
  this.z *= amount;
  return this;
  }

  public static Vector3f scale(final Vector3f vec, final float amount){
  float x, y, z;

  x = vec.x * amount;
  y = vec.y * amount;
  z = vec.z * amount;

  return new Vector3f(x, y, z);
  }

  public Vector3f invScale(final float amount){
  if(amount != 0){
  return this.scale(1.0f/amount);
  }
  return this;
  }

  public static Vector3f invScale(final Vector3f vec, final float amount){
  if(amount != 0){
  return scale(vec, 1.0f/amount);
  }
  return vec;
  }

  public Vector3f rotateX(float angle) {
  float cosAngle = (float)Math.cos(angle);
  float sinAngle = (float)Math.sin(angle);

  this.y = this.y * cosAngle - this.z * sinAngle;
  this.z = this.y * sinAngle + this.z * cosAngle;
  return this;
  }

  public Vector3f rotateY(float angle) {
  float cosAngle = (float)Math.cos(angle);
  float sinAngle = (float)Math.sin(angle);

  this.x = this.x * cosAngle + this.z * sinAngle;
  this.z = -this.x * sinAngle + this.z * cosAngle;
  return this;
  }

  public Vector3f rotateZ(float angle) {
  float cosAngle = (float)Math.cos(angle);
  float sinAngle = (float)Math.sin(angle);

  this.x = this.x * cosAngle - this.y * sinAngle;
  this.y = this.x * sinAngle + this.y * cosAngle;
  return this;
  }

  public Vector3f rotate(float angle, Vector3f axis) {
  float sinHalfAngle = (float) Math.sin(Math.toRadians(angle / 2));
  float cosHalfAngle = (float) Math.cos(Math.toRadians(angle / 2));

  float rX = axis.x * sinHalfAngle;
  float rY = axis.y * sinHalfAngle;
  float rZ = axis.z * sinHalfAngle;
  float rW = cosHalfAngle;

  Quat4f result = new Quat4f(rX, rY, rZ, rW);
  Quat4f rotation = new Quat4f(rX, rY, rZ, rW).conjugate();
  Quat4f p = new Quat4f(this.x, this.y ,this.z, 0.0f);

  result.mul(p).mul(rotation);

  this.x = result.x;
  this.y = result.y;
  this.z = result.z;

  return this;
  }


  public static Vector3f rotate(Vector3f vec, float angle, Vector3f axis) {
  float sinHalfAngle = (float) Math.sin(Math.toRadians(angle / 2));
  float cosHalfAngle = (float) Math.cos(Math.toRadians(angle / 2));

  float rY = axis.y * sinHalfAngle;
  float rZ = axis.z * sinHalfAngle;
  float rW = cosHalfAngle;

  Vector4f result = new Vector4f(rX, rY, rZ, rW).normalize();
  Vector4f rotation = new Vector4f(rX, rY, rZ, rW).normalize().conjugate();
  Vector4f p = new Vector4f(vec.x, vec.y ,vec.z, 0.0f).normalize();

  result.mul(p).mul(rotation);

  return new Vector3f(result.x, result.y, result.z);
  }

  public Vector3f normalize(){
  float len = this.length();
  if(len == 0){
  return new Vector3f(0,0,0);
  }
  float inverseLength = 1/len;
  this.x *= inverseLength;
  this.y *= inverseLength;
  this.z *= inverseLength;
  return this;
  }

  public static Vector3f normalize(Vector3f v){
  float len = v.length();
  if(len == 0){
  return new Vector3f(0,0,0);
  }
  float inverseLength = 1/len;
  Vector3f result = new Vector3f();
  result.x = v.x * inverseLength;
  result.y = v.y * inverseLength;
  result.z = v.z * inverseLength;
  return result;
  }

  public Vector3f negate(){
  return this.scale(-1);
  }

  public static Vector3f negate(final Vector3f a){
  return new Vector3f(-a.x, -a.y, -a.z);
  }

  public float length(){
  return (float)Math.sqrt(this.x*this.x + this.y*this.y + this.z*this.z);
  }

  public float compare(Vector3f that){
  return this.length() - that.length();
  }

  public final float angle(Vector3f v1)
  {
  if(this.length() == 0 || v1.length() == 0){
  return 0;
  }
  double vDot = this.dot(v1) / ( this.length()*v1.length() );
  if( vDot < -1.0) vDot = -1.0;
  if( vDot >  1.0) vDot =  1.0;
  return((float) (Math.acos( vDot )));
  }

  public boolean equals(Vector3f that){
  float EPSILON = 0.00000001f;
  return Math.abs(this.x - that.x) < EPSILON &&
  Math.abs(this.y - that.y) < EPSILON &&
  Math.abs(this.z - that.z) < EPSILON;
  }
*/

};

Vector3f operator+(const Vector3f& v1, const Vector3f& v2) {
    return Vector3f(
	v1.x+v2.x,
	v1.y+v2.y,
	v1.z+v2.z);
}

bool operator==(const Vector3f& v1, const Vector3f& v2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(v1.x - v2.x) <= EPSILON && fabs(v1.y - v2.y) <= EPSILON && fabs(v1.z - v2.z) <= EPSILON;
}

bool operator!=(const Vector3f& v1, const Vector3f& v2) {
    return !(v1 == v2);
}
