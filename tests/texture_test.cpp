#include <open_gl_test.hpp>

#include <libcgp/texture.hpp>

class TextureTest : public OpenGLTest
{
};

TEST_F(TextureTest, TextureTest) { const auto texture = LibGcp::MakeTextureFromImage("container"); }
