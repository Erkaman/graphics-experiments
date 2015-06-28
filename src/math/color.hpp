#pragma once

class Color {

public:

    /*
      INSTANCE VARIABLES
     */

    float r,g,b,a;

    /*
      CONSTRUCTORS
     */

    Color():r(0.0f),g(0.0f),b(0.0f), a(0.0f) {  }

    Color(const float value): r(value), g(value),b(value), a(value){  }

    Color(const float r, const float g, const float b, const float a):r(r), g(g),b(b), a(a) { }

    Color(const float r, const float g, const float b):r(r), g(g),b(b), a(1.0f) { }
};
