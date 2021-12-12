#pragma once

#include <axolotl/texture.hh>
#include <axolotl/types.hh>

namespace axl {

  enum class FrameBufferTexture { Color, DepthStencil, Last };

  class FrameBuffer {
   public:
    FrameBuffer(u32 widht, u32 height);
    ~FrameBuffer();

    void RebuildFrameBuffer();
    void SetSize(u32 width, u32 height);
    void Bind();
    void Unbind();
    Texture2D GetTexture(FrameBufferTexture texture);

    static void BindDefault();

   protected:
    u32 _width;
    u32 _height;
    u32 _frame_buffer;
    i32 _prev_frame_buffer;
    v4i _prev_viewport;
    Texture2D *_textures[(u32)FrameBufferTexture::Last];
  };

} // namespace axl
