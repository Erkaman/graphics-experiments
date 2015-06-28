#pragma once

#include <vector>
#include <string>

class Color {

public:

    /*
      INSTANCE VARIABLES
    */

    float r,g,b,a;

    /*
      CONSTRUCTORS
    */

    Color();

    Color(const float value);

    Color(const float r, const float g, const float b, const float a);
    Color(const float r, const float g, const float b);


    /*
      INSTANCE METHODS
    */
    void Add(std::vector<float>& v)const;

    /*
      OPERATOR OVERLOADS.
    */
    friend Color operator-(const Color& c1, const Color& c2);
    friend Color operator+(const Color& c1, const Color& c2);

    friend Color operator*(const float scale, const Color& c);
    friend Color operator*(const Color& c, const float scale);

    friend bool operator==(const Color& c1, const Color& c2);

    operator std::string() const;

    /*
      STATIC METHODS
    */

    static Color FromInt(const unsigned int r, const unsigned int g, const unsigned int b);

    /*
      Lerping from a to b, where v is a value in the range [0,1]
    */
    static Color Lerp(const Color& a, const Color& b, const float v);

};
