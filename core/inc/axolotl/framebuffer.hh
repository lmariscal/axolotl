#pragma once

#include <axolotl/types.hh>

#include <axolotl/texture.hh>

namespace axl {

  enum class FrameBufferTexture {
    Color,
    DepthStencil,
    Last
  };

  struct FrameBuffer {
   public:
    FrameBuffer(u32 widht, u32 height);
    ~FrameBuffer();

    void RebuildFrameBuffer();
    void SetSize(u32 width, u32 height);
    void Bind();
    void Unbind();
    Texture GetTexture(FrameBufferTexture texture);

   protected:
    u32 _width;
    u32 _height;
    u32 _frame_buffer;
    Texture *_textures[(u32)FrameBufferTexture::Last];
  };

} // namespace axl
