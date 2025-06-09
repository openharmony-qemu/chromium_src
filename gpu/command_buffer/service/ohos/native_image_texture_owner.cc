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

#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"

#include <memory>

#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/decoder_context.h"
#include "gpu/command_buffer/service/feature_info.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_gl_owner.h"
#include "gpu/command_buffer/service/texture_base.h"
#include "ui/gl/scoped_binders.h"
#include "ui/gl/scoped_make_current.h"

namespace gpu {
namespace {

std::unique_ptr<AbstractTextureOHOS> CreateTexture(
    SharedContextState* context_state) {
  DCHECK(context_state);

  gles2::FeatureInfo* feature_info = context_state->feature_info();
  if (feature_info && feature_info->is_passthrough_cmd_decoder()) {
    return AbstractTextureOHOS::CreateForPassthrough(gfx::Size());
  }

  return AbstractTextureOHOS::CreateForValidating(gfx::Size());
}

}  // namespace

NativeImageTextureOwner::NativeImageTextureOwner(
    bool binds_texture_on_update,
    std::unique_ptr<AbstractTextureOHOS> texture,
    scoped_refptr<SharedContextState> context_state)
    : base::RefCountedDeleteOnSequence<NativeImageTextureOwner>(
          base::SingleThreadTaskRunner::GetCurrentDefault()),
      binds_texture_on_update_(binds_texture_on_update),
      context_state_(std::move(context_state)),
      texture_(std::move(texture)),
      task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
  DCHECK(context_state_);
  context_state_->AddContextLostObserver(this);
}

NativeImageTextureOwner::NativeImageTextureOwner(
    bool binds_texture_on_update,
    std::unique_ptr<AbstractTextureOHOS> texture)
    : base::RefCountedDeleteOnSequence<NativeImageTextureOwner>(
          base::SingleThreadTaskRunner::GetCurrentDefault()),
      binds_texture_on_update_(binds_texture_on_update),
      texture_(std::move(texture)),
      task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {}

NativeImageTextureOwner::~NativeImageTextureOwner() {
  bool have_context = true;
  absl::optional<ui::ScopedMakeCurrent> scoped_make_current;
  if (!context_state_) {
    have_context = false;
  } else {
    if (!context_state_->IsCurrent(nullptr, /*needs_gl=*/true)) {
      scoped_make_current.emplace(context_state_->context(),
                                  context_state_->surface());
      have_context = scoped_make_current->IsContextCurrent();
    }
    context_state_->RemoveContextLostObserver(this);
  }
  if (!have_context)
    texture_->NotifyOnContextLost();

  texture_.reset();
  context_state_.reset();
}

// static
scoped_refptr<NativeImageTextureOwner> NativeImageTextureOwner::Create(
    scoped_refptr<SharedContextState> context_state,
    Mode mode) {
  auto texture = CreateTexture(context_state.get());
  return new NativeImageTextureGlOwner(std::move(texture),
                                       std::move(context_state));
}

GLuint NativeImageTextureOwner::GetTextureId() const {
  return texture_->service_id();
}

TextureBase* NativeImageTextureOwner::GetTextureBase() const {
  return texture_->GetTextureBase();
}

void NativeImageTextureOwner::OnContextLost() {
  ReleaseResources();
  context_state_->RemoveContextLostObserver(this);
  context_state_.reset();
}

}  // namespace gpu
