#ifndef PRIMITIVES_QUAD_HPP_
#define PRIMITIVES_QUAD_HPP_

#include <cstdint>
#include <libcgp/defines.hpp>

LIBGCP_DECL_START_

class Quad
{
    public:
    ~Quad();

    void Destroy();
    void Init();
    void Draw() const;

    protected:
    uint32_t vao_{};
    uint32_t vbo_{};
};

LIBGCP_DECL_END_

#endif  // PRIMITIVES_QUAD_HPP_
