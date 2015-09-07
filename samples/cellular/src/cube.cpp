#include "cube.hpp"

#include "ewa/perlin_seed.hpp"
#include "ewa/common.hpp"

#include "ewa/gl/shader_program.hpp"
#include "ewa/gl/texture2d.hpp"
#include "ewa/gl/vbo.hpp"

#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector3f.hpp"

#include "ewa/eob_file.hpp"
#include "ewa/file.hpp"


using std::vector;

Cube::Cube(const Vector3f& position ) {

    m_modelMatrix = Matrix4f::CreateTranslation(position);

    GeometryObject::Init("obj/cube.eob");
}

Cube::~Cube() {
}
