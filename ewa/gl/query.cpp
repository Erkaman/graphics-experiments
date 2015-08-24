#include "gl/query.hpp"

Query::Query(GLenum target): m_target(target) {
    GL_C(glGenQueries(1, &m_id);)
}

Query::~Query() {
    GL_C(glDeleteQueries(1, &m_id) );
}

void Query::Bind() {
    GL_C(glBeginQuery(m_target, m_id));
}

void Query::Unbind() {
    GL_C(glEndQuery(m_target));
}

GLuint Query::GetResult() {
    GLuint result;
    glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);

    return result;
}
