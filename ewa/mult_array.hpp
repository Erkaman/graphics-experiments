#pragma once

#include <stddef.h>
#include <utility>
#include <assert.h>
#include <string>

#include "math/math_common.hpp"
#include <string.h>

template<typename T>
class MultArray {

private:

    signed int m_xsize;
    signed int m_ysize;
    signed int m_totalsize;
    T* m_data;

    signed int Index(const signed int x, const signed int y)const {
	return y * m_xsize + x;
    }

public:

    typedef T* iterator;

    /*
      CONSTRUCTOR
     */
    MultArray(const signed int xsize, const signed int ysize):
	m_xsize(xsize), m_ysize(ysize), m_totalsize(m_xsize*m_ysize),
	m_data(new T[m_totalsize]){

    }

    /*
      DESTRUCTOR
     */
    ~MultArray() {
	delete [] m_data;
    }

    /*
      Copy constructor.
     */
    MultArray (const MultArray& other) :
        m_xsize(other.m_xsize),m_ysize(other.m_ysize), m_totalsize(other.m_totalsize), m_data (new T[other.m_totalsize]) {
	memcpy(this->m_data, other.m_data, sizeof(T) * other.m_totalsize);
    }

    /*
      Move constructor.
     */
    MultArray (MultArray&& other) noexcept :
       m_data (other.m_data), m_xsize(other.m_xsize),m_ysize(other.m_ysize), m_totalsize(other.m_totalsize) {
	// reset the resources of other.
        other.m_data = nullptr;
    }

    /*
      Copy assignment operator.
     */
    MultArray& operator= (const MultArray& other)
    {
        MultArray tmp(other); // re-use copy-constructor
        *this = std::move(tmp); // re-use move-assignment
        return *this;
    }

    /*
      Move assignment operator.
     */
    MultArray& operator=(MultArray&& other){
	if (this!=&other) {
	    delete[] m_data;

	    // pilfer
	    m_data = other.m_data;
	    m_xsize = other.m_xsize;
	    m_ysize = other.m_ysize;
	    m_totalsize = other.m_totalsize;

	    other.m_data=nullptr;
	}
	return *this;
    }

    /*
      OPERATOR OVERLOADS
     */
    friend bool operator==(const MultArray& m1, const MultArray& m2) {
	if(m1.m_xsize != m2.m_xsize || m1.m_ysize != m2.m_ysize) {
	    return false;
	}

	for(signed int i = 0; i < m1.m_totalsize; ++i) {
	    if(m1.m_data[i] != m2.m_data[i])
		return false;
	}

	return true;
    }

    friend bool operator!=(const MultArray& m1, const MultArray& m2)  {
	return !(m1 == m2);
    }


    operator std::string() const {

	std::string str;

	str += "{\n";

	for(signed int y = 0; y < m_ysize; ++y) {
	    for(signed int x = 0; x < m_xsize; ++x) {

		str += std::to_string(m_data[Index(x,y)]);

		if(x+1 < m_xsize) {
		    str += ", ";
		}
	    }
	    if(y+1 < m_ysize)
		str += "\n";
	}

	str += "\n}";

	return str;
    }

    /*
      GETTERS
    */
    iterator Begin()const {
	return m_data;
    }

    iterator End()const {
	return m_data + m_totalsize;
    }

    const T&operator()(const signed int x, const signed int y) const{
	assert(x < m_xsize);
	assert(y < m_ysize);

	return m_data[Index(x,y)];

    }

    T&operator()(const signed int x, const signed int y) {
	assert(x < m_xsize);
	assert(y < m_ysize);

	return m_data[Index(x,y)];
    }

    const T& GetWrap(const signed int x, const signed int y) {
	return m_data[Index(
		Clamp(x, (signed int)0, m_xsize-1),
		Clamp(y, (signed int)0, m_ysize-1))];
    }

    signed int GetXsize()const {
	return m_xsize;
    }

    signed int GetYsize()const {
	return m_ysize;
    }

    signed int GetTotalsize()const {
	return m_totalsize;
    }

    T* GetData() const {
	return m_data;
    }

};
