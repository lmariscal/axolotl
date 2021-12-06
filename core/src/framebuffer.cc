#include <axolotl/framebuffer.hh>

#include <glad.h>

namespace axl {

  FrameBuffer::FrameBuffer(u32 width, u32 height):
    _width(width),
    _height(height)
  {
    for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i)
      _textures[i] = nullptr;

    RebuildFrameBuffer();
  }

  FrameBuffer::~FrameBuffer() {
    for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i) {
      delete _textures[i];
      _textures[i] = nullptr;
    }
    glDeleteFramebuffers(1, &_frame_buffer);
  }

  void FrameBuffer::RebuildFrameBuffer() {
    if (_frame_buffer) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

      for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i) {
        delete _textures[i];
        _textures[i] = nullptr;
      }
      glDeleteFramebuffers(1, &_frame_buffer);
      _frame_buffer = 0;
    }

    glGenFramebuffers(1, &_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);

    for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i) {
      TextureData texture_data;

      TextureInternalFormat internal_format;
      if (i == (i32)FrameBufferTexture::Color)
        internal_format = TextureInternalFormat::RGB;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        internal_format = TextureInternalFormat::DepthStencil;

      TextureFormat format;
      if (i == (i32)FrameBufferTexture::Color)
        format = TextureFormat::RGB;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        format = TextureFormat::DepthStencil;

      TextureDataType type;
      if (i == (i32)FrameBufferTexture::Color)
        type = TextureDataType::U8;
      else if (i == (i32)FrameBufferTexture::DepthStencil)
        type = TextureDataType::U24_8;

      texture_data.size = { _width, _height };
      texture_data.internal_format = internal_format;
      texture_data.format = format;
      texture_data.data_type = type;

      _textures[i] = new Texture2D("", TextureType::Last, texture_data);
    }
    TextureStore::ProcessQueue();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
        *_textures[(i32)FrameBufferTexture::DepthStencil], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        *_textures[(i32)FrameBufferTexture::Color], 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      log::error("Framebuffer not complete!");
      for (i32 i = 0; i < (i32)FrameBufferTexture::Last; ++i) {
        delete _textures[i];
        _textures[i] = nullptr;
      }
      glDeleteFramebuffers(1, &_frame_buffer);
      _frame_buffer = 0;
    }

    log::debug("Framebuffer {}, width {}, height {} created", _frame_buffer, _width, _height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBuffer::BindDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBuffer::Bind() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prev_frame_buffer);
    glGetIntegerv(GL_VIEWPORT, value_ptr(_prev_viewport));

    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer);
    glViewport(0, 0, _width, _height);
  }

  void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, _prev_frame_buffer);
    glViewport(_prev_viewport[0], _prev_viewport[1], _prev_viewport[2], _prev_viewport[3]);
  }

  void FrameBuffer::SetSize(u32 width, u32 height) {
    _width = width;
    _height = height;
  }

  Texture2D FrameBuffer::GetTexture(FrameBufferTexture texture) {
    return *_textures[(i32)texture];
  }

} // namespace axl
