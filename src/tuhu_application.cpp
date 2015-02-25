#include "tuhu_application.h"

void TuhuApplication::Init() {

      // tell GL to only draw onto a pixel if the shape is closer to the viewer

	  GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
}
