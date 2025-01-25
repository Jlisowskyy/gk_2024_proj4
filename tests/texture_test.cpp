#include <open_gl_test.hpp>

#include <libcgp/texture.hpp>
#include <utility>

class TextureTest : public OpenGLTest
{
};

TEST_F(TextureTest, TextureTest)
{
    auto texture        = LibGcp::MakeTextureFromImage("container");
    const auto texture2 = std::move(texture);
}
