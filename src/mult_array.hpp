#pragma once

#include <stddef.h>
#include <utility>
#include <assert.h>
#include <string>
#include <string.h>

template<typename T>
class MultArray {

private:

    size_t m_xsize;
    size_t m_ysize;
    size_t m_totalsize;
    T* m_data;

    size_t Index(const size_t x, const size_t y)const {
	return y * m_xsize + x;
    }

public:

    /*
      CONSTRUCTOR
     */
    MultArray(const size_t xsize, const size_t ysize):
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

	for(size_t i = 0; i < m1.m_totalsize; ++i) {
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

	for(size_t y = 0; y < m_ysize; ++y) {
	    for(size_t x = 0; x < m_xsize; ++x) {

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

    const T& Get(const size_t x, const size_t y)const {

	assert(x < m_xsize);
	assert(y < m_ysize);

	return m_data[Index(x,y)];
    }

    T& Get(const size_t x, const size_t y) {
	assert(x < m_xsize);
	assert(y < m_ysize);

	return m_data[Index(x,y)];
    }

    size_t GetXsize()const {
	return m_xsize;
    }

    size_t GetYsize()const {
	return m_ysize;
    }

    size_t GetTotalsize()const {
	return m_totalsize;
    }

};
