// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on shared_image_video_surface_texture.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#include "gpu/command_buffer/service/ohos/native_image_image_backing.h"

#include <utility>

#include "base/task/single_thread_task_runner.h"
#include "components/viz/common/resources/resource_sizes.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/command_buffer/service/abstract_texture_ohos.h"
#include "gpu/command_buffer/service/memory_tracking.h"
#include "gpu/command_buffer/service/ohos/native_image_texture_owner.h"
#include "gpu/command_buffer/service/shared_context_state.h"
#include "gpu/command_buffer/service/shared_image/shared_image_representation.h"
#include "gpu/command_buffer/service/shared_image/skia_gl_image_representation.h"
#include "gpu/command_buffer/service/skia_utils.h"
#include "gpu/command_buffer/service/texture_manager.h"
#include "ohos_video_image_backing.h"
#include "ui/gl/gl_utils.h"

namespace gpu {

NativeImageImageBacking::NativeImageImageBacking(
    const Mailbox& mailbox,
    const gfx::Size& size,
    const gfx::ColorSpace color_space,
    GrSurfaceOrigin surface_origin,
    SkAlphaType alpha_type,
    std::string debug_label,
    scoped_refptr<StreamTextureSharedImageInterface> stream_texture_sii,
    scoped_refptr<SharedContextState> context_state)
    : OhosVideoImageBacking(mailbox,
                            size,
                            color_space,
                            surface_origin,
                            alpha_type,
                            std::move(debug_label),
                            /*is_thread_safe=*/false),
      stream_texture_sii_(std::move(stream_texture_sii)),
      context_state_(std::move(context_state)),
      gpu_main_task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {
  DCHECK(stream_texture_sii_);
  DCHECK(context_state_);

  context_state_->AddContextLostObserver(this);
}

NativeImageImageBacking::~NativeImageImageBacking() {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());

  if (context_state_) {
    context_state_->RemoveContextLostObserver(this);
  }
  context_state_.reset();
  if (stream_texture_sii_ != nullptr) {
    stream_texture_sii_->ReleaseResources();
    stream_texture_sii_.reset();
  }
}

size_t NativeImageImageBacking::GetEstimatedSizeForMemoryDump() const {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());
#if false
  return stream_texture_sii_->IsUsingGpuMemory() ? GetEstimatedSize() : 0;
#endif
    return 0;
}

void NativeImageImageBacking::OnContextLost() {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());

  stream_texture_sii_->ReleaseResources();
  context_state_->RemoveContextLostObserver(this);
  context_state_ = nullptr;
}

class NativeImageImageBacking::SharedImageRepresentationGLTextureVideo
    : public GLTextureImageRepresentation {
 public:
  SharedImageRepresentationGLTextureVideo(
      SharedImageManager* manager,
      NativeImageImageBacking* backing,
      MemoryTypeTracker* tracker,
      std::unique_ptr<AbstractTextureOHOS> texture)
      : GLTextureImageRepresentation(manager, backing, tracker),
        texture_(std::move(texture)) {}

  ~SharedImageRepresentationGLTextureVideo() override {
    if (!has_context()) {
      texture_->NotifyOnContextLost();
    }
  }

  SharedImageRepresentationGLTextureVideo(
      const SharedImageRepresentationGLTextureVideo&) = delete;
  SharedImageRepresentationGLTextureVideo& operator=(
      const SharedImageRepresentationGLTextureVideo&) = delete;

  gles2::Texture* GetTexture(int plane_index) override {
    DCHECK_EQ(plane_index, 0);

    auto* texture = gles2::Texture::CheckedCast(texture_->GetTextureBase());
    DCHECK(texture);

    return texture;
  }

  bool BeginAccess(GLenum mode) override {
    DCHECK(mode == GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);

    auto* video_backing = static_cast<NativeImageImageBacking*>(backing());
    video_backing->BeginGLReadAccess(texture_->service_id());
    return true;
  }

  void EndAccess() override {}

 private:
  std::unique_ptr<AbstractTextureOHOS> texture_;
};

class NativeImageImageBacking::
    SharedImageRepresentationGLTexturePassthroughVideo
    : public GLTexturePassthroughImageRepresentation {
 public:
  SharedImageRepresentationGLTexturePassthroughVideo(
      SharedImageManager* manager,
      NativeImageImageBacking* backing,
      MemoryTypeTracker* tracker,
      std::unique_ptr<AbstractTextureOHOS> abstract_texture)
      : GLTexturePassthroughImageRepresentation(manager, backing, tracker),
        abstract_texture_(std::move(abstract_texture)),
        passthrough_texture_(gles2::TexturePassthrough::CheckedCast(
            abstract_texture_->GetTextureBase())) {
    CHECK(passthrough_texture_);
  }

  ~SharedImageRepresentationGLTexturePassthroughVideo() override {
    if (!has_context()) {
      abstract_texture_->NotifyOnContextLost();
    }
  }

  SharedImageRepresentationGLTexturePassthroughVideo(
      const SharedImageRepresentationGLTexturePassthroughVideo&) = delete;
  SharedImageRepresentationGLTexturePassthroughVideo& operator=(
      const SharedImageRepresentationGLTexturePassthroughVideo&) = delete;

  const scoped_refptr<gles2::TexturePassthrough>& GetTexturePassthrough(
      int plane_index) override {
    DCHECK_EQ(plane_index, 0);
    return passthrough_texture_;
  }

  bool BeginAccess(GLenum mode) override {
    DCHECK(mode == GL_SHARED_IMAGE_ACCESS_MODE_READ_CHROMIUM);

    auto* video_backing = static_cast<NativeImageImageBacking*>(backing());
    video_backing->BeginGLReadAccess(passthrough_texture_->service_id());
    return true;
  }

  void EndAccess() override {}

 private:
  std::unique_ptr<AbstractTextureOHOS> abstract_texture_;
  scoped_refptr<gles2::TexturePassthrough> passthrough_texture_;
};

std::unique_ptr<GLTextureImageRepresentation>
NativeImageImageBacking::ProduceGLTexture(SharedImageManager* manager,
                                          MemoryTypeTracker* tracker) {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());

  if (!stream_texture_sii_->HasTextureOwner()) {
    return nullptr;
  }

  auto texture = GenAbstractTexture(/*passthrough=*/false);
  if (!texture) {
    return nullptr;
  }

  DCHECK(stream_texture_sii_->TextureOwnerBindsTextureOnUpdate());
  texture->BindToServiceId(stream_texture_sii_->GetTextureBase()->service_id());

  return std::make_unique<SharedImageRepresentationGLTextureVideo>(
      manager, this, tracker, std::move(texture));
}

std::unique_ptr<GLTexturePassthroughImageRepresentation>
NativeImageImageBacking::ProduceGLTexturePassthrough(
    SharedImageManager* manager,
    MemoryTypeTracker* tracker) {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());

  if (!stream_texture_sii_->HasTextureOwner()) {
    return nullptr;
  }

  auto texture = GenAbstractTexture(/*passthrough=*/true);
  if (!texture) {
    return nullptr;
  }

  DCHECK(stream_texture_sii_->TextureOwnerBindsTextureOnUpdate());
  texture->BindToServiceId(stream_texture_sii_->GetTextureBase()->service_id());

  return std::make_unique<SharedImageRepresentationGLTexturePassthroughVideo>(
      manager, this, tracker, std::move(texture));
}

std::unique_ptr<SkiaGaneshImageRepresentation>
NativeImageImageBacking::ProduceSkiaGanesh(
    SharedImageManager* manager,
    MemoryTypeTracker* tracker,
    scoped_refptr<SharedContextState> context_state) {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(context_state);

  if (!stream_texture_sii_->HasTextureOwner()) {
    return nullptr;
  }

  if (!context_state->GrContextIsGL()) {
    DCHECK(false);
    return nullptr;
  }

  DCHECK(context_state->GrContextIsGL());
  auto* texture_base = stream_texture_sii_->GetTextureBase();
  DCHECK(texture_base);
  const bool passthrough =
      (texture_base->GetType() == gpu::TextureBase::Type::kPassthrough);

  auto texture = GenAbstractTexture(passthrough);
  if (!texture) {
    return nullptr;
  }

  DCHECK(stream_texture_sii_->TextureOwnerBindsTextureOnUpdate());
  texture->BindToServiceId(stream_texture_sii_->GetTextureBase()->service_id());

  std::unique_ptr<gpu::GLTextureImageRepresentationBase> gl_representation;
  if (passthrough) {
    gl_representation =
        std::make_unique<SharedImageRepresentationGLTexturePassthroughVideo>(
            manager, this, tracker, std::move(texture));
  } else {
    gl_representation =
        std::make_unique<SharedImageRepresentationGLTextureVideo>(
            manager, this, tracker, std::move(texture));
  }
  return SkiaGLImageRepresentation::Create(std::move(gl_representation),
                                           std::move(context_state), manager,
                                           this, tracker);
}
 
void NativeImageImageBacking::BeginGLReadAccess(
    const GLuint service_id) {
  stream_texture_sii_->UpdateAndBindTexImage(service_id);
}

class NativeImageImageBacking::SharedImageRepresentationOverlayVideo {
  //    : public gpu::LegacyOverlayImageRepresentation {
 public:
  SharedImageRepresentationOverlayVideo(gpu::SharedImageManager* manager,
                                        NativeImageImageBacking* backing,
                                        gpu::MemoryTypeTracker* tracker);
  // : gpu::LegacyOverlayImageRepresentation(manager, backing, tracker) {}

  SharedImageRepresentationOverlayVideo(
      const SharedImageRepresentationOverlayVideo&) = delete;
  SharedImageRepresentationOverlayVideo& operator=(
      const SharedImageRepresentationOverlayVideo&) = delete;

 protected:
#if false
  void RenderToOverlay() {
    DCHECK(!stream_image()->HasTextureOwner())
        << "CodecImage must be already in overlay";
    TRACE_EVENT0("media", "OverlayVideoImageRepresentation::RenderToOverlay");
    stream_image()->RenderToOverlay();
  }
 
  void NotifyOverlayPromotion(bool promotion,
                              const gfx::Rect& bounds) {
    stream_image()->NotifyOverlayPromotion(promotion, bounds);
  }
#endif
 private:
#if false
  StreamTextureSharedImageInterface* stream_image() {
    auto* video_backing =
        static_cast<SharedImageVideoOhosNativeImage*>(backing());
    DCHECK(video_backing);
    return video_backing->stream_texture_sii_.get();
  }
#endif
};
 
#if false
std::unique_ptr<gpu::LegacyOverlayImageRepresentation>
SharedImageVideoOhosNativeImage::ProduceLegacyOverlay(
    gpu::SharedImageManager* manager,
    gpu::MemoryTypeTracker* tracker) {
  DCHECK(gpu_main_task_runner_->RunsTasksInCurrentSequence());
 
  return std::make_unique<SharedImageRepresentationOverlayVideo>(manager, this,
                                                           tracker);
}
#endif
}  // namespace gpu
