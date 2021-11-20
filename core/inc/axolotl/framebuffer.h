#pragma once

#include <axolotl/types.h>

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
    u32 GetTextureID(FrameBufferTexture texture);

   protected:
    u32 _width;
    u32 _height;
    u32 _frame_buffer;
    u32 _textures[(u32)FrameBufferTexture::Last];
  };

} // namespace axl
