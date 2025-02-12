#include <libcgp/engine/g_buffer.hpp>

#include <libcgp/window/window.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
#include <libcgp/utils/macros.hpp>
// clang-format on

void LibGcp::GBuffer::PrepareBuffers()
{
    glGenFramebuffers(1, &g_buffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);

    const auto [w, h] = Window::GetInstance().GetWindowSize();

    /* position buffer */
    glGenTextures(1, &g_position_);
    glBindTexture(GL_TEXTURE_2D, g_position_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position_, 0);

    /* normal buffer */
    glGenTextures(1, &g_normal_);
    glBindTexture(GL_TEXTURE_2D, g_normal_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal_, 0);

    /* albedo + specular buffer */
    glGenTextures(1, &g_albedo_spec_);
    glBindTexture(GL_TEXTURE_2D, g_albedo_spec_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo_spec_, 0);

    /* attach buffers */
    constexpr uint32_t attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    /* create depth buffer */
    uint32_t rbo_depth;
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

    R_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LibGcp::GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LibGcp::GBuffer::BindForReading(Shader &shader)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_position_);
    shader.SetGLint("GBuffer.position", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_normal_);
    shader.SetGLint("GBuffer.normal", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_albedo_spec_);
    shader.SetGLint("GBuffer.albedo_spec", 2);
}
