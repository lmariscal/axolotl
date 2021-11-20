#include <axolotl/framebuffer.h>

#include <glad.h>

namespace axl {

  FrameBuffer::FrameBuffer(u32 width, u32 height):
    _width(width),
    _height(height)
  {
    for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i)
      _textures[i] = 0;

    RebuildFrameBuffer();
  }

  FrameBuffer::~FrameBuffer() {
    glDeleteTextures((i32)FrameBufferTexture::Last, _textures);
    glDeleteFramebuffers(1, &_frame_buffer);
  }

  void FrameBuffer::RebuildFrameBuffer() {
    if (_frame_buffer) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

      glDeleteTextures((i32)FrameBufferTexture::Last, _textures);
      glDeleteFramebuffers(1, &_frame_buffer);
      _frame_buffer = 0;
    }

    glGenFramebuffers(1, &_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

    glGenTextures((i32)FrameBufferTexture::Last, _textures);

    for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i) {
      glBindTexture(GL_TEXTURE_2D, _textures[i]);

      u32 internal_format = 0;
      if (i == (i32)FrameBufferTexture::Color)
        internal_format = GL_RGB;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        internal_format = GL_DEPTH24_STENCIL8;

      u32 format = 0;
      if (i == (i32)FrameBufferTexture::Color)
        format = GL_RGB;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        format = GL_DEPTH_STENCIL;

      u32 type = 0;
      if (i == (i32)FrameBufferTexture::Color)
        type = GL_UNSIGNED_BYTE;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        type = GL_UNSIGNED_INT_24_8;

      glTexImage2D(GL_TEXTURE_2D, 0, internal_format, _width, _height, 0, format, type, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textures[(i32)FrameBufferTexture::Color], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _textures[(i32)FrameBufferTexture::DepthStencil], 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log::error("Framebuffer not complete!");
      glDeleteTextures((i32)FrameBufferTexture::Last, _textures);
      glDeleteFramebuffers(1, &_frame_buffer);
      _frame_buffer = 0;
    }

    log::debug("Framebuffer {}, width {}, height {} created", _frame_buffer, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
  }

  void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBuffer::SetSize(u32 width, u32 height) {
    _width = width;
    _height = height;
  }

  u32 FrameBuffer::GetTextureID(FrameBufferTexture texture) {
    return _textures[(i32)texture];
  }

} // namespace axl
