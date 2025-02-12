#ifndef ENGINE_G_BUFFER_HPP_
#define ENGINE_G_BUFFER_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/primitives/shader.hpp>

#include <cstdint>

LIBGCP_DECL_START_

class GBuffer
{
    // ------------------------------
    // Class internals
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    // ------------------------------
    // Class interaction
    // ------------------------------

    void PrepareBuffers();

    void BindForWriting();

    void BindForReading(Shader& shader);

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    uint32_t g_buffer_{};
    uint32_t g_position_{};
    uint32_t g_normal_{};
    uint32_t g_albedo_spec_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_G_BUFFER_HPP_
