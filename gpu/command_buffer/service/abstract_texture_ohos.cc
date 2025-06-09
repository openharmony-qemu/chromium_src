/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_surface.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"

namespace gpu {
namespace {
GLuint CreateTextureWithLinearFilter() {
  const auto target = GL_TEXTURE_EXTERNAL_OES;
  GLuint service_id = 0;
  auto* api = gl::g_current_gl_context;
  api->glGenTexturesFn(1, &service_id);
  gl::ScopedTextureBinder binder(target, service_id);
  api->glTexParameteriFn(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  api->glTexParameteriFn(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  api->glTexParameteriFn(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  api->glTexParameteriFn(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return service_id;
}
}  // namespace

std::unique_ptr<AbstractTextureOHOS> AbstractTextureOHOS::CreateForValidating(
    gfx::Size size) {
  GLuint service_id = CreateTextureWithLinearFilter();

  auto* texture = gpu::gles2::CreateGLES2TextureWithLightRef(
      service_id, GL_TEXTURE_EXTERNAL_OES);
  gfx::Rect cleared_rect;
  texture->SetLevelInfo(GL_TEXTURE_EXTERNAL_OES, 0, GL_RGBA, size.width(),
                        size.height(), 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                        cleared_rect);
  texture->SetImmutable(true, false);

  return std::make_unique<AbstractTextureOHOS>(texture);
}

std::unique_ptr<AbstractTextureOHOS> AbstractTextureOHOS::CreateForPassthrough(
    gfx::Size size) {
  GLuint service_id = CreateTextureWithLinearFilter();
  auto texture = base::MakeRefCounted<gles2::TexturePassthrough>(
      service_id, GL_TEXTURE_EXTERNAL_OES);

  return std::make_unique<AbstractTextureOHOS>(std::move(texture), size);
}

std::unique_ptr<AbstractTextureOHOS> AbstractTextureOHOS::CreateForTesting(
    GLuint texture_id) {
  auto texture = std::make_unique<gpu::TextureBase>(texture_id);
  return std::make_unique<AbstractTextureOHOS>(std::move(texture));
}

AbstractTextureOHOS::AbstractTextureOHOS(
    std::unique_ptr<gpu::TextureBase> texture)
    : texture_for_testing_(std::move(texture)) {}
AbstractTextureOHOS::AbstractTextureOHOS(gles2::Texture* texture)
    : texture_(texture), api_(gl::g_current_gl_context) {}
AbstractTextureOHOS::AbstractTextureOHOS(
    scoped_refptr<gles2::TexturePassthrough> texture,
    const gfx::Size& size)
    : texture_passthrough_(std::move(texture)),
      texture_passthrough_size_(size),
      api_(gl::g_current_gl_context) {
  DCHECK(texture_passthrough_ &&
         texture_passthrough_->target() == GL_TEXTURE_EXTERNAL_OES);
}

AbstractTextureOHOS::~AbstractTextureOHOS() {
  // If context is not lost, then the texture should be destroyed on same
  // context it was create on.
  if ((texture_ || texture_passthrough_) && have_context_) {
    DCHECK_EQ(api_, gl::g_current_gl_context);
  }

  if (texture_) {
    texture_.ExtractAsDangling()->RemoveLightweightRef(have_context_);
  }
}

void AbstractTextureOHOS::NotifyOnContextLost() {
  if (texture_passthrough_) {
    texture_passthrough_->MarkContextLost();
  }
  have_context_ = false;
}

void AbstractTextureOHOS::BindToServiceId(GLuint service_id) {
  if (texture_) {
    texture_->BindToServiceId(service_id);
    texture_->SetLevelCleared(texture_->target(), /*level=*/0, true);
  } else if (texture_passthrough_) {
    texture_passthrough_->BindToServiceId(service_id);
  }
}

TextureBase* AbstractTextureOHOS::GetTextureBase() const {
  if (texture_) {
    return texture_;
  }
  if (texture_passthrough_) {
    return texture_passthrough_.get();
  }
  if (texture_for_testing_) {
    return texture_for_testing_.get();
  }
  return nullptr;
}

}  // namespace gpu
