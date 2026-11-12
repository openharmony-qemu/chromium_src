/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <bitset>
#include <native_buffer/native_buffer.h>
#include <sstream>
#include <utility>
#include <vector>

#include "arkweb/ohos_adapter_ndk/ohos_adapter_helper_ext.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "base/timer/elapsed_timer.h"
#include "components/viz/common/gpu/vulkan_context_provider.h"
#include "components/viz/service/display_embedder/output_presenter_ohos.h"
#include "components/viz/service/display_embedder/skia_output_surface_dependency.h"
#include "content/public/common/content_switches.h"
#include "gpu/command_buffer/service/external_semaphore.h"
#include "gpu/ipc/common/nweb_native_window_tracker.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/gpu_fence_handle.h"
#include "ui/gfx/overlay_plane_data.h"

namespace viz {
namespace {
constexpr int kPoint2Size = 2;
constexpr int kNumberOfBuffers = 4;
constexpr int kColorChannelSize = 4;
constexpr int kColorRed = 0;
constexpr int kColorGreen = 1;
constexpr int kColorBlue = 2;
constexpr int kColorAlpha = 3;
OHOS::NWeb::OhosAdapterHelper& GetAdapterHelper() {
  return OHOS::NWeb::OhosAdapterHelper::GetInstance();
}

gfx::Size GetRotatedSize(const gfx::Size size,
                         gfx::OverlayTransform transform) {
  const bool needs_swapped_dims =
      transform == gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90 ||
      transform == gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270 ||
      transform == gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_90 ||
      transform == gfx::OVERLAY_TRANSFORM_FLIP_VERTICAL_CLOCKWISE_270;
  return needs_swapped_dims ? gfx::Size(size.height(), size.width()) : size;
}

int32_t FromGfxOverlayTransform(gfx::OverlayTransform transform) {
  switch (transform) {
    case gfx::OVERLAY_TRANSFORM_NONE:
      return OH_TRANSFORM_ROTATE_NONE;
    case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_90:
      return OH_TRANSFORM_ROTATE_90;
    case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_180:
      return OH_TRANSFORM_ROTATE_180;
    case gfx::OVERLAY_TRANSFORM_ROTATE_CLOCKWISE_270:
      return OH_TRANSFORM_ROTATE_270;
    default:
      NOTREACHED();
  }
}
} // namespace

class OutputPresenterOHOS::SurfaceControl {
 public:
  using ReleaseCallback =
      base::OnceCallback<void(base::ScopedFD release_fence_fd)>;
  using SurfaceControlAdapter = OHOS::NWeb::SurfaceControlAdapter;
  using TransactionAdapter = OHOS::NWeb::SurfaceTransactionAdapter;

  explicit SurfaceControl(SurfaceControl* parent,
                          std::string name,
                          SurfaceControlAdapter& surface_adapter,
                          TransactionAdapter& transaction_adapter)
      : parent_(parent ? parent->surface_ : nullptr),
        name_(std::move(name)),
        surface_control_adapter_(surface_adapter),
        transaction_adapter_(transaction_adapter) {
    TRACE_EVENT1("viz", "Create SurfaceControl", "name", name_.c_str());
    if (!parent || !parent->surface_) {
      LOG(ERROR) << "DelegateTag Creating SurfaceControl, parent is null";
    }
  }

  explicit SurfaceControl(OHNativeWindow* window,
                          SurfaceControlAdapter& surface_adapter,
                          TransactionAdapter& transaction_adapter,
                          OH_SurfaceTransaction* transaction,
                          bool in_process)
      : name_("delegate_container"),
        surface_control_adapter_(surface_adapter),
        transaction_adapter_(transaction_adapter) {
    if (!window) {
      LOG(ERROR) << "DelegateTag Creating SurfaceControl, window is null";
    }
    surface_ = reinterpret_cast<OH_SurfaceControl*>(surface_control_adapter_
      .CreateContainerSurfaceControl(transaction, window, name_.c_str(), in_process));
    is_root_ = true;
    if (!surface_) {
      LOG(ERROR) << "DelegateTag Creating failed, surface_ is null";
    }
  }

  ~SurfaceControl() {
    DCHECK(destroyed_);
    TRACE_EVENT1("viz", "~SurfaceControl", "name", name_.c_str());
    if (surface_) {
      surface_control_adapter_.Release(surface_);
    }
  }

  void ClearBufferQueueCache(OH_SurfaceTransaction* transaction, bool is_clean_all) {
    if (surface_ == nullptr) {
      LOG(ERROR) << "DelegateTag ClearBufferQueueCache, surface_ is nullptr";
      return;
    }

    transaction_adapter_.ClearBufferQueueCache(transaction, surface_, is_clean_all);
  }

  void Destroy(OH_SurfaceTransaction* transaction) {
    if (destroyed_) {
      LOG(ERROR) << "DelegateTag surface is already destroyed";
      return;
    }
    destroyed_ = true;
    transaction_adapter_.Reparent(transaction, surface_, nullptr);
    transaction_adapter_.SetVisibility(transaction, surface_,
        OH_SURFACE_TRANSACTION_VISIBILITY_HIDE);
  }

  OH_SurfaceControl* ReleaseSurface() {
    DCHECK(parent_);
    OH_SurfaceControl* surface = surface_;
    surface_ = nullptr;
    destroyed_ = true;
    return surface;
  }

  void SetSurface(SurfaceControl* surface) {
    DCHECK(!surface_);
    if (!surface) {
      LOG(ERROR) << "DelegateTag Set surface failed, surface_ is null";
      return;
    }
    surface_ = surface->ReleaseSurface();
    DCHECK(surface->dirty_bits_.none());

    auto check_and_set = [this](const auto& lhs, const auto& rhs, auto bit) {
      if (lhs != rhs) {
        dirty_bits_.set(bit);
      }
    };

    check_and_set(z_order_, surface->z_order_, kZOrder);
    check_and_set(bounds_, surface->bounds_, kBounds);
    check_and_set(pivot_, surface->pivot_, kPivot);
    check_and_set(background_color_, surface->background_color_, kBackgroundColor);
    check_and_set(alpha_, surface->alpha_, kAlpha);
    check_and_set(buffer_transform_, surface->buffer_transform_, kBufferTransform);
    check_and_set(src_rect_, surface->src_rect_, kSrcRect);
    check_and_set(display_rect_, surface->display_rect_, kDisplayRect);
  }

  void SetZOrder(int32_t z_order) {
    UpdateProperty(z_order_, z_order, kZOrder);
  }

  void SetGeometry(const gfx::RectF& src_rect,
                   const gfx::RectF& display_rect,
                   int32_t transform) {
    UpdateProperty(src_rect_, src_rect, kSrcRect);
    UpdateProperty(display_rect_, display_rect, kDisplayRect);
    UpdateProperty(buffer_transform_, transform, kBufferTransform);
  }

  void SetBounds(const gfx::RectF& bounds) {
    UpdateProperty(bounds_, bounds, kBounds);
  }

  void SetPivot(float x, float y) {
    if (pivot_[0] != x || pivot_[1] != y) {
      pivot_ = {x, y};
      dirty_bits_.set(kPivot);
    }
  }

  void SetBackgroundColor(const std::array<float, kColorChannelSize>& color) {
    UpdateProperty(background_color_, color, kBackgroundColor);
  }

  void SetAlpha(float alpha) { UpdateProperty(alpha_, alpha, kAlpha); }

  void SetBufferTransform(int32_t transform) {
    UpdateProperty(buffer_transform_, transform, kBufferTransform);
  }

  void SetBuffer(OH_NativeBuffer* buffer,
                 base::ScopedFD fence_fd,
                 const gfx::Rect& damage_rect,
                 ReleaseCallback release_callback) {
    DCHECK(buffer);
    DCHECK(release_callback);
    DCHECK(!release_callback_);
    if (!release_callback) {
      LOG(ERROR) << "DelegateTag: SetBuffer release_callback is nullptr";
    }
    if (release_callback_) {
      LOG(ERROR) << "DelegateTag: SetBuffer Unexpected release_callback_ presence";
    }
    DCHECK_NE(buffer_, buffer);
    buffer_ = buffer;
    fence_fd_ = std::move(fence_fd);
    damage_rect_ = damage_rect;
    release_callback_ = std::move(release_callback);
    dirty_bits_.set(kBuffer);
  }

  void SetMailbox(const gpu::Mailbox& mailbox) { mailbox_ = mailbox; }

  void Sync(OH_SurfaceTransaction* transaction, bool in_process) {
    DCHECK(!destroyed_);
    if (!surface_ && !is_root_) {
      surface_ = reinterpret_cast<OH_SurfaceControl*>(surface_control_adapter_
        .CreateChildSurfaceControl(transaction, name_.c_str(), in_process));
      if (surface_ == nullptr) {
        LOG(ERROR) << "DelegateTag create surface_ failed";
      }
      transaction_adapter_.SetHardwareEnableHint(transaction, surface_, true);
      transaction_adapter_.Reparent(transaction, surface_, parent_);
      transaction_adapter_.SetVisibility(
          transaction, surface_, OH_SURFACE_TRANSACTION_VISIBILITY_SHOW);
      transaction_adapter_.SetZOrder(transaction, surface_, z_order_);
      dirty_bits_.reset(kZOrder);
    }
    if (dirty_bits_.test(kZOrder)) {
      transaction_adapter_.SetZOrder(transaction, surface_, z_order_);
    }
    if (dirty_bits_.test(kBounds)) {
      transaction_adapter_.SetBounds(transaction, surface_, bounds_.x(),
          bounds_.y(), bounds_.width(), bounds_.height());
    }
    if (dirty_bits_.test(kPivot)) {
      transaction_adapter_.SetPivot(
          transaction, surface_, pivot_[0], pivot_[1]);
    }
    if (dirty_bits_.test(kBackgroundColor)) {
      transaction_adapter_.SetBackgroundColor(
          transaction, surface_,
          background_color_[kColorRed], background_color_[kColorGreen],
          background_color_[kColorBlue], background_color_[kColorAlpha]);
    }
    if (dirty_bits_.test(kAlpha)) {
      transaction_adapter_.SetBufferAlpha(transaction, surface_, alpha_);
    }
    if (dirty_bits_.test(kBufferTransform)) {
      transaction_adapter_.SetBufferTransform(
          transaction, surface_, buffer_transform_);
    }
    if (dirty_bits_.test(kSrcRect)) {
      transaction_adapter_.SetSrcRect(
          transaction, surface_,
          src_rect_.x(), src_rect_.y(),
          src_rect_.width(), src_rect_.height());
    }
    if (dirty_bits_.test(kDisplayRect)) {
      transaction_adapter_.SetDisplayRect(
          transaction, surface_,
          display_rect_.x(), display_rect_.y(),
          display_rect_.width(), display_rect_.height());
    }
    if (dirty_bits_.test(kBuffer)) {
      DCHECK(buffer_);
      auto context = release_callback_ ?
          std::make_unique<ReleaseCallback>(std::move(release_callback_)) :
          std::unique_ptr<ReleaseCallback>();
      transaction_adapter_.SetBuffer(
          transaction,
          surface_,
          buffer_,
          fence_fd_.release(),
          context.release(),
          [](void* context, int32_t release_fence_fd) {
            std::unique_ptr<ReleaseCallback> callback(
                reinterpret_cast<ReleaseCallback*>(context));
            if (callback) {
              std::move(*callback).Run(base::ScopedFD(release_fence_fd));
            }
          });
      OH_Rect rect = {damage_rect_.x(), damage_rect_.y(), damage_rect_.width(),
                      damage_rect_.height()};
      transaction_adapter_.SetDamageRegion(transaction, surface_, &rect, 1);
    }
    dirty_bits_.reset();
  }

  const gfx::RectF& bounds() const { return bounds_; }
  OH_NativeBuffer* buffer() const { return buffer_; }
  bool has_surface() const { return surface_ != nullptr; }
  OH_SurfaceControl* surface() const { return surface_; }
  const gpu::Mailbox& mailbox() const { return mailbox_; }
  const std::string& name() const { return name_; }
  int32_t z_order() const { return z_order_; }

 private:
  template <typename T, typename U>
  void UpdateProperty(T& field, U&& new_value, DirtyBits bit) {
    if (field != new_value) {
      field = std::forward<U>(new_value);
      dirty_bits_.set(bit);
    }
  }

  std::bitset<kCount> dirty_bits_;
  OH_SurfaceControl* parent_ = nullptr;
  OH_SurfaceControl* surface_ = nullptr;
  bool destroyed_ = false;
  std::string name_;
  int32_t z_order_ = -1;
  gfx::RectF bounds_;
  gfx::RectF src_rect_;
  gfx::RectF display_rect_;
  std::array<float, kPoint2Size> pivot_ = { 0.5, 0.5 };
  std::array<float, kColorChannelSize> background_color_ = {0.0f, 0.0f, 0.0f, 0.0f};
  float alpha_ = 1.0f;
  int32_t buffer_transform_ = OH_TRANSFORM_ROTATE_NONE;
  bool is_root_ = false;

  OH_NativeBuffer* buffer_ = nullptr;
  base::ScopedFD fence_fd_;
  gfx::Rect damage_rect_;
  ReleaseCallback release_callback_;
  gpu::Mailbox mailbox_;
  SurfaceControlAdapter& surface_control_adapter_;
  TransactionAdapter& transaction_adapter_;
};

std::unique_ptr<OutputPresenterOHOS> OutputPresenterOHOS::Create(
    SkiaOutputSurfaceDependency* deps) {
  TRACE_EVENT0("viz", "OutputPresenterOHOS::Create");
  LOG(INFO) << "DelegateTag OutputPresenterOHOS Create";
  if (!deps) {
    LOG(ERROR) << "DelegateTag deps is null";
    return nullptr;
  }

  return std::make_unique<OutputPresenterOHOS>(deps);
}

OutputPresenterOHOS::OutputPresenterOHOS(SkiaOutputSurfaceDependency* deps)
    : dependency_(deps),
      task_runner_(base::SequencedTaskRunner::GetCurrentDefault()),
      surface_control_adapter_(GetAdapterHelper().GetSurfaceControlAdapter()),
      transaction_adapter_(GetAdapterHelper().GetSurfaceTransactionAdapter()) {
  TRACE_EVENT0("viz", "OutputPresenterOHOS::OutputPresenterOHOS");
  auto* tracker = NWebNativeWindowTracker::GetInstance();
  void* window = tracker->GetNativeWindow(dependency_->GetSurfaceHandle());
  transaction_ = reinterpret_cast<OH_SurfaceTransaction*>(transaction_adapter_.
      CreateSurfaceTransaction(reinterpret_cast<OHNativeWindow*>(window)));

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kSingleProcess) ||
      base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kInProcessGPU)) {
    in_process_ = true;
  }

  root_surface_ = std::make_unique<SurfaceControl>(
      reinterpret_cast<OHNativeWindow*>(window),
      surface_control_adapter_, transaction_adapter_, transaction_, in_process_);

  auto callback = base::BindRepeating(&OutputPresenterOHOS::OnComplete,
                                      weak_factory_.GetWeakPtr());

  // Make sure OnComplete is called on the correct thread.
  callback = base::BindPostTask(task_runner_, std::move(callback));
  on_complete_callback_ = std::make_unique<OnCompleteCallback>(callback);
  OHOS::NWeb::SurfaceControlOnCompleteCallback func =
      [](void* context, uint64_t timestamp) {
        auto* callback = reinterpret_cast<OnCompleteCallback*>(context);
        if (callback) {
          callback->Run(timestamp);
        }
      };
  transaction_adapter_.SetOnComplete(
      transaction_, on_complete_callback_.get(), func);
}

OutputPresenterOHOS::~OutputPresenterOHOS() {
  TRACE_EVENT0("viz", "~OutputPresenterOHOS");
  LOG(INFO) << "DelegateTag ~OutputPresenterOHOS";
  for (auto& surface : previous_frame_) {
    if (surface) {
      surface->Destroy(transaction_);
      surface.reset();
    }
  }
  for (auto& surface : pending_frame_) {
    if (surface) {
      surface->Destroy(transaction_);
      surface.reset();
    }
  }
  if (root_surface_) {
    root_surface_->Destroy(transaction_);
  }
  transaction_adapter_.Commit(transaction_);
  transaction_adapter_.SetOnComplete(transaction_, nullptr, nullptr);
  transaction_adapter_.DeleteTransaction(transaction_);
}

void OutputPresenterOHOS::InitializeCapabilities(
    OutputSurface::Capabilities* capabilities) {
  capabilities->number_of_buffers = kNumberOfBuffers;
  capabilities->pending_swap_params.max_pending_swaps = kNumberOfBuffers - 1;
  // We expect origin of buffers is at top left.
  capabilities->output_surface_origin = gfx::SurfaceOrigin::kTopLeft;
  capabilities->supports_post_sub_buffer = true;
  capabilities->supports_surfaceless = true;
  capabilities->needs_background_image = false;
  capabilities->supports_non_backed_solid_color_overlays = true;
  capabilities->orientation_mode = OutputSurface::OrientationMode::kHardware;

  capabilities->sk_color_type_map[SinglePlaneFormat::kRGBA_8888] =
      kRGBA_8888_SkColorType;
  capabilities->sk_color_type_map[SinglePlaneFormat::kBGRA_8888] =
      kRGBA_8888_SkColorType;
}

bool OutputPresenterOHOS::Reshape(const ReshapeParams& params) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  TRACE_EVENT0("viz", "OutputPresenterOHOS::Reshape");
  size_ = params.GfxSize();
  transform_ = params.transform;
  if (root_surface_) {
    auto rect = gfx::RectF(GetRotatedSize(size_, transform_));
    root_surface_->SetBounds(rect);
  }
  DiscardCompositingResources(false, false);
  return true;
}

void OutputPresenterOHOS::Present(SwapCompletionCallback completion_callback,
                                  BufferPresentedCallback presentation_callback,
                                  gfx::FrameData data) {
  TRACE_EVENT0("viz", "OutputPresenterOHOS::Present");
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  root_surface_->Sync(transaction_, in_process_);

  // Sort pending frame by z order
  std::sort(pending_frame_.begin(), pending_frame_.end(),
            [](const auto& a, const auto& b) {
              return a->z_order() < b->z_order();
            });

  int32_t first_surface_z = (!pending_frame_.empty() && pending_frame_.front())
                            ? pending_frame_.front()->z_order() : 0;

  // Remove reused item.
  std::erase(previous_frame_, nullptr);

  for (auto& surface : pending_frame_) {
    // Try reuse OH_SurfaceControl from previous frame.
    if (!previous_frame_.empty() && !surface->has_surface() &&
        surface->buffer()) {
      auto it = std::find_if(previous_frame_.begin(), previous_frame_.end(),
          [&](const auto& frame) {
            return frame && frame->name() == surface->name();
          });
      if (it != previous_frame_.end()) {
        surface->SetSurface(it->get());
        previous_frame_.erase(it);
      }
    }
    // negative z order doesn't work, offset to support underlay
    if (first_surface_z < 0) {
      int32_t z_order = surface->z_order() - first_surface_z;
      surface->SetZOrder(z_order);
    }
    surface->Sync(transaction_, in_process_);
  }

  // Clear all unused surface from previous frame.
  for (auto& surface : previous_frame_) {
    if (surface) {
      surface->Destroy(transaction_);
      surface.reset();
    }
  }
  previous_frame_.clear();

  bool has_previous = !completion_datas_.empty();
  completion_datas_.emplace_back(std::move(completion_callback),
                                 root_overlay_access_);
  if (has_previous) {
    auto& prev_data = completion_datas_[completion_datas_.size() - 2];
    if (prev_data.access && root_overlay_access_ && prev_data.access.get() == root_overlay_access_.get()) {
      // If root plane buffer is not changed, reset previous frame access to
      // nullptr, so TriggerOnCompleteCallbacks() can trigger the previous
      // completion callback without waiting for OnOverlayReleased() being called.
      prev_data.access.reset();
    }
  }

  root_overlay_access_.reset();
  presentation_callbacks_.emplace_back(std::move(presentation_callback));

  transaction_adapter_.Commit(transaction_);
  std::swap(previous_frame_, pending_frame_);
}

#if BUILDFLAG(ARKWEB_AI_HDR)
gfx::SizeF OutputPresenterOHOS::MaybeCorrectBufferSize(
    const OutputPresenter::OverlayPlaneCandidate& overlay_plane,
    ScopedOverlayAccess* access,
    const gfx::SizeF& buffer_size) {
  if (!overlay_plane.is_ai_hdr_active || !access) {
    return buffer_size;
  }
  OH_NativeBuffer* nb = access->GetOHNativeBuffer();
  if (!nb) {
    return buffer_size;
  }
  OH_NativeBuffer_Config config;
  OH_NativeBuffer_GetConfig(nb, &config);
  if (config.width > 0 && config.height > 0 &&
      (config.width != overlay_plane.resource_size_in_pixels.width() ||
       config.height != overlay_plane.resource_size_in_pixels.height())) {
    TRACE_EVENT2("viz", "OutputPresenterOHOS buffer_size corrected",
                 "stale_size", buffer_size.ToString(),
                 "actual_size", gfx::SizeF(config.width, config.height).ToString());
    return gfx::SizeF(config.width, config.height);
  }
  return buffer_size;
}
#endif

void OutputPresenterOHOS::ScheduleOverlayPlane(
    const OutputPresenter::OverlayPlaneCandidate& overlay_plane,
    ScopedOverlayAccess* access) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

  DCHECK_EQ(overlay_plane.is_solid_color, access == nullptr)
      << "access and is_solid_color don't match";

  const bool is_root = overlay_plane.is_root_render_pass;
  const gfx::RectF& display_rect = overlay_plane.display_rect;
  gfx::RectF bounds = display_rect;
  gfx::RectF src_rect = display_rect;
  std::string surface_name = "delegate_child";
  if (!is_root &&
      overlay_plane.priority_hint == gfx::OverlayPriorityHint::kVideo) {
    surface_name = "delegate_child_video";
  }
  TRACE_EVENT1("viz", "OutputPresenterOHOS::ScheduleOverlayPlane",
    "name", surface_name.c_str());

  if (is_root) {
    DCHECK(!root_overlay_access_);
    root_overlay_access_ = access->GetWeakPtr();
  } else {
#if BUILDFLAG(ARKWEB_AI_HDR)
    gfx::SizeF buffer_size =
        MaybeCorrectBufferSize(overlay_plane, access,
                               gfx::SizeF(overlay_plane.resource_size_in_pixels));
#else
    const gfx::SizeF buffer_size(overlay_plane.resource_size_in_pixels);
#endif
    src_rect = gfx::ScaleRect(overlay_plane.uv_rect, buffer_size);
    if (src_rect.width() == 0) {
      src_rect.set_width(1);
      if (src_rect.right() > buffer_size.width()) {
        src_rect.set_x(buffer_size.width() - 1);
      }
    }
    if (src_rect.height() == 0) {
      src_rect.set_height(1);
      if (src_rect.bottom() > buffer_size.height()) {
        src_rect.set_y(buffer_size.height() - 1);
      }
    }
    src_rect.Intersect(gfx::RectF(buffer_size));
    bounds = gfx::RectF(display_rect.origin(), src_rect.size());
  }

  OH_NativeBuffer* const buffer =
      access ? access->GetOHNativeBuffer() : nullptr;
  SurfaceControl* surface = nullptr;

  if (buffer) {
    // Find surface with same buffer in previous frame first.
    auto it = std::find_if(previous_frame_.begin(), previous_frame_.end(),
        [surface_name, buffer](const auto& surface) {
          return surface && surface->name() == surface_name &&
                 surface->buffer() == buffer;
        });
    if (it == previous_frame_.end()) {
      // Find surface with same display rect in previous frame.
      it = std::find_if(previous_frame_.begin(), previous_frame_.end(),
          [surface_name, bounds](const auto& surface) {
            return surface && surface->name() == surface_name &&
                   surface->bounds() == bounds;
          });
    }

    if (it != previous_frame_.end()) {
      pending_frame_.push_back(std::move(*it));
    } else {
      DCHECK(root_surface_);
      // Cannot find a surface with same buffer or bounds, so create a new one.
      pending_frame_.emplace_back(std::make_unique<SurfaceControl>(
          root_surface_.get(), surface_name,
          surface_control_adapter_, transaction_adapter_));
    }

    const auto& mailbox = access->representation()->mailbox();
    surface = pending_frame_.back().get();
    if (surface && surface->buffer() != buffer) {
      base::ScopedFD fence_fd = access->TakeAcquireFence().Release();
      auto callback = base::BindOnce(&OutputPresenterOHOS::OnOverlayReleased,
                                     weak_factory_.GetWeakPtr(), buffer);

      // Make sure OnOverlayReleased() is called on GPU main thread.
      callback = base::BindPostTask(task_runner_, std::move(callback));

      // We need track buffer which are being used by RS. For root surface,
      // the buffer is considered being released when OnComplete is called.
      access->InUseByWindowServerInc();

      auto damage_rect = overlay_plane.damage_rect.IsEmpty()
          ? gfx::Rect(overlay_plane.resource_size_in_pixels)
          : ToEnclosingRect(overlay_plane.damage_rect);
      surface->SetBuffer(buffer, std::move(fence_fd), damage_rect,
                         std::move(callback));
      surface->SetMailbox(mailbox);
      DCHECK(in_used_buffers_.find(buffer) == in_used_buffers_.end());
      in_used_buffers_[buffer] = std::make_pair(access->GetWeakPtr(), is_root);

      switch (access->representation()->surface_origin()) {
        case kTopLeft_GrSurfaceOrigin:
          surface->SetBufferTransform(OH_TRANSFORM_ROTATE_NONE);
          break;
        case kBottomLeft_GrSurfaceOrigin:
          surface->SetBufferTransform(OH_TRANSFORM_FLIP_V);
          break;
      }
    } else if (surface && surface->mailbox() != mailbox) {
      // For some video buffers, the buffers could be same, but shared image
      // backings are different. In that case, we need to update the access and
      // mailbox.
      access->InUseByWindowServerInc();

      auto iter = in_used_buffers_.find(buffer);
      if (iter != in_used_buffers_.end()) {
        auto& item = iter->second;
        auto old_access = item.first;
        item.first = access->GetWeakPtr();
        if (old_access) {
          old_access->InUseByWindowServerDec();
        }
      } else {
        in_used_buffers_[buffer] = std::make_pair(access->GetWeakPtr(), is_root);
      }
      surface->SetMailbox(mailbox);
    }
  }

  if (surface == nullptr) {
    LOG(ERROR) << "DelegateTag surface is nullptr, is_root: " << is_root;
    return;
  }

  // Make sure surface is visible.
  surface->SetBounds(bounds);
  surface->SetZOrder(overlay_plane.plane_z_order);
  surface->SetAlpha(overlay_plane.opacity);
  surface->SetPivot(0, 0);

  // Only root buffer is always in hardware physical direction(portrait), so it
  // need to be rotated to the current screen logic rotation direction.
  auto transform =
      is_root ? FromGfxOverlayTransform(transform_) : OH_TRANSFORM_ROTATE_NONE;

  surface->SetBufferTransform(transform);
  surface->SetGeometry(src_rect, display_rect, transform);

  // Set surface's background color.
  if (overlay_plane.color.has_value()) {
    surface->SetBackgroundColor(
        {overlay_plane.color->fR, overlay_plane.color->fG,
         overlay_plane.color->fB, overlay_plane.color->fA});
  }
}

void OutputPresenterOHOS::OnComplete(uint64_t timestamp) {
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  DCHECK(!completion_datas_.empty());
  DCHECK(!presentation_callbacks_.empty());
  TRACE_EVENT0("viz", "OutputPresenterOHOS::OnComplete");
  for (auto& data : completion_datas_) {
    if (!data.completed) {
      data.completed = true;
      TriggerOnCompleteCallbacks();
      break;
    }
  }
}

void OutputPresenterOHOS::TriggerOnCompleteCallbacks() {
  while (!completion_datas_.empty()) {
    auto& data = completion_datas_.front();
    if (data.access || !data.completed) {
      break;
    }
    TRACE_EVENT1("viz", "OutputPresenterOHOS::TriggerOnCompleteCallbacks",
      "completion callback size", completion_datas_.size());
    auto completion_callback = std::move(data.callback);
    completion_datas_.pop_front();

    gfx::SwapCompletionResult swap_result(gfx::SwapResult::SWAP_ACK);
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(std::move(completion_callback), std::move(swap_result)));

    if (!presentation_callbacks_.empty()) {
      auto presentation_callback = std::move(presentation_callbacks_.front());
      presentation_callbacks_.pop_front();

      gfx::PresentationFeedback feedback(base::TimeTicks::Now(), base::TimeDelta(),
                                         /* flags= */0);
      task_runner_->PostTask(
          FROM_HERE, base::BindOnce(std::move(presentation_callback), std::move(feedback)));
    } else {
      LOG(ERROR) << "DelegateTag presentation_callbacks_ is empty";
    }
  }
}

void OutputPresenterOHOS::OnOverlayReleased(OH_NativeBuffer* buffer,
                                            base::ScopedFD release_fence_fd) {
  TRACE_EVENT0("viz", "OutputPresenterOHOS::OnOverlayReleased");
  DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
  auto it = in_used_buffers_.find(buffer);
  if (it == in_used_buffers_.end()) {
    LOG(WARNING) << "DelegateTag Cannot find in used buffer";
    return;
  }
  auto [access, is_root] = it->second;
  in_used_buffers_.erase(it);
  if (access.get()) {
    if (release_fence_fd.is_valid()) {
      gfx::GpuFenceHandle fence_handle;
      fence_handle.Adopt(std::move(release_fence_fd));
      access->SetReleaseFence(std::move(fence_handle));
    }
    // Decrease in use count, when count is 0, viz will return the buffer to
    // render, so the buffer can be reused or released.
    access->InUseByWindowServerDec();
  } else {
    LOG(ERROR) << "DelegateTag access has expired before overlay released, is_root: " << is_root;
  }

  if (is_root) {
    for (auto& data : completion_datas_) {
      if (data.access && access && data.access.get() == access.get()) {
        data.access.reset();
        TriggerOnCompleteCallbacks();
        break;
      }
    }
  }
}

void OutputPresenterOHOS::DiscardCompositingResources(bool is_clean_all, bool is_clean_video_surface) {
  LOG(INFO) << "DelegateTag call ClearBufferQueueCache id: " << dependency_->GetSurfaceHandle();
  for (const auto& surface : previous_frame_) {
    if (surface && (surface->name() == "delegate_child" || is_clean_video_surface)) {
      surface->ClearBufferQueueCache(transaction_, is_clean_all);
    }
  }

  for (auto it = in_used_buffers_.begin(); it != in_used_buffers_.end();) {
    if (it->second.second) {
      it = in_used_buffers_.erase(it);
    } else {
      ++it;
    }
  }

  while (!completion_datas_.empty()) {
    auto& data = completion_datas_.front();
    auto completion_callback = std::move(data.callback);
    completion_datas_.pop_front();

    gfx::SwapCompletionResult swap_result(gfx::SwapResult::SWAP_ACK);
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(std::move(completion_callback), std::move(swap_result)));

    if (!presentation_callbacks_.empty()) {
      auto presentation_callback = std::move(presentation_callbacks_.front());
      presentation_callbacks_.pop_front();

      gfx::PresentationFeedback feedback(base::TimeTicks::Now(), base::TimeDelta(),
                                         /* flags= */0);
      task_runner_->PostTask(
          FROM_HERE, base::BindOnce(std::move(presentation_callback), std::move(feedback)));
    } else {
      LOG(ERROR) << "DelegateTag presentation_callbacks_ is empty";
    }
  }
}
}  // namespace viz