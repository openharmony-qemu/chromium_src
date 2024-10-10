// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/toast_layer_impl.h"

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <iomanip>
#include <utility>
#include <vector>

#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/shared_memory_mapping.h"
#include "base/notreached.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/task/single_thread_task_runner.h"
#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "build/build_config.h"
#include "cc/debug/debug_colors.h"
#include "cc/metrics/dropped_frame_counter.h"
#include "cc/paint/display_item_list.h"
#include "cc/paint/image_provider.h"
#include "cc/paint/paint_canvas.h"
#include "cc/paint/paint_flags.h"
#include "cc/paint/paint_image_builder.h"
#include "cc/paint/paint_shader.h"
#include "cc/paint/record_paint_canvas.h"
#include "cc/paint/skia_paint_canvas.h"
#include "cc/resources/memory_history.h"
#include "cc/trees/layer_tree_frame_sink.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/gpu/context_provider.h"
#include "components/viz/common/quads/solid_color_draw_quad.h"
#include "components/viz/common/quads/texture_draw_quad.h"
#include "components/viz/common/resources/bitmap_allocation.h"
#include "components/viz/common/resources/platform_color.h"
#include "components/viz/common/resources/resource_format_utils.h"
#include "components/viz/common/resources/shared_image_format.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/context_support.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/client/raster_interface.h"
#include "gpu/command_buffer/client/shared_image_interface.h"
#include "gpu/command_buffer/common/shared_image_trace_utils.h"
#include "gpu/command_buffer/common/shared_image_usage.h"
#include "gpu/config/gpu_feature_info.h"
#include "skia/ext/legacy_display_globals.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"
#include "third_party/skia/include/core/SkFont.h"
#include "third_party/skia/include/core/SkFontMetrics.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/core/SkTextBlob.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/skia_conversions.h"
#include "ui/gl/trace_util.h"

namespace cc {

namespace {

class DummyImageProvider : public ImageProvider {
 public:
  DummyImageProvider() = default;
  ~DummyImageProvider() override = default;
  ImageProvider::ScopedResult GetRasterContent(
      const DrawImage& draw_image) override {
    NOTREACHED();
    return ScopedResult();
  }
};

constexpr SkColor kToastBackgroundColor = SkColorSetRGB(0xfd, 0xfd, 0xfd);
constexpr SkColor kToastTextColor = SkColorSetARGB(0xe6, 0, 0, 0);
constexpr SkColor kToastShadowColor = SkColorSetARGB(0x66, 0, 0, 0);
const int kToastFontSize = 14;
const int kToastHeight = 36;
const int kToastShadowOffsetY = 10;

const int kFadeInSteps = 10;
const int kDisplaySteps = 70;
const int kFadeOutSteps = 20;
const int kTotalDisplaySteps = kFadeInSteps + kDisplaySteps + kFadeOutSteps;
const base::TimeDelta kAnimationDuration = base::Seconds(3);

int CalculateAlpha(int current_step) {
  const int max_alpha = 0xFF;
  if (current_step < kFadeInSteps) {
    return current_step * max_alpha / kFadeInSteps;
  }
  if (current_step < kFadeInSteps + kDisplaySteps) {
    return max_alpha;
  }
  return (kTotalDisplaySteps - current_step) * max_alpha / kFadeOutSteps;
}

SkColor ScaleSkColorAlpha(SkColor color, uint8_t alpha) {
  uint8_t new_alpha = SkColorGetA(color) * alpha / 0xFFu;
  return SkColorSetA(color, new_alpha);
}

}  // namespace

ToastLayerImpl::ToastLayerImpl(LayerTreeImpl* tree_impl, int id)
    : LayerImpl(tree_impl, id) {}

ToastLayerImpl::~ToastLayerImpl() {
  ReleaseResources();
}

std::unique_ptr<LayerImpl> ToastLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl) const {
  return ToastLayerImpl::Create(tree_impl, id());
}

class ToastGpuBacking : public ResourcePool::GpuBacking {
 public:
  ~ToastGpuBacking() override {
    if (mailbox.IsZero())
      return;
    if (returned_sync_token.HasData())
      shared_image_interface->DestroySharedImage(returned_sync_token, mailbox);
    else if (mailbox_sync_token.HasData())
      shared_image_interface->DestroySharedImage(mailbox_sync_token, mailbox);
  }

  void OnMemoryDump(
      base::trace_event::ProcessMemoryDump* pmd,
      const base::trace_event::MemoryAllocatorDumpGuid& buffer_dump_guid,
      uint64_t tracing_process_id,
      int importance) const override {
    if (mailbox.IsZero())
      return;

    auto tracing_guid = gpu::GetSharedImageGUIDForTracing(mailbox);
    pmd->CreateSharedGlobalAllocatorDump(tracing_guid);
    pmd->AddOwnershipEdge(buffer_dump_guid, tracing_guid, importance);
  }

  raw_ptr<gpu::SharedImageInterface> shared_image_interface = nullptr;
};

class ToastSoftwareBacking : public ResourcePool::SoftwareBacking {
 public:
  ~ToastSoftwareBacking() override {
    layer_tree_frame_sink->DidDeleteSharedBitmap(shared_bitmap_id);
  }

  void OnMemoryDump(
      base::trace_event::ProcessMemoryDump* pmd,
      const base::trace_event::MemoryAllocatorDumpGuid& buffer_dump_guid,
      uint64_t tracing_process_id,
      int importance) const override {
    pmd->CreateSharedMemoryOwnershipEdge(buffer_dump_guid,
                                         shared_mapping.guid(), importance);
  }

  raw_ptr<LayerTreeFrameSink> layer_tree_frame_sink;
  base::WritableSharedMemoryMapping shared_mapping;
};

bool ToastLayerImpl::WillDraw(
    DrawMode draw_mode,
    viz::ClientResourceProvider* resource_provider) {
  if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE &&
      !LayerImpl::WillDraw(draw_mode, resource_provider)) {
    return false;
  }

  int max_texture_size = layer_tree_impl()->max_texture_size();
  // TODO(crbug.com/1196414): Support 2D scales in heads up layers.
  internal_contents_scale_ = GetIdealContentsScaleKey();
  internal_content_bounds_ =
      gfx::ScaleToCeiledSize(bounds(), internal_contents_scale_);
  internal_content_bounds_.SetToMin(
      gfx::Size(max_texture_size, max_texture_size));

  return true;
}

void ToastLayerImpl::AppendQuads(
    viz::CompositorRenderPass* render_pass,
    AppendQuadsData* append_quads_data) {
  viz::SharedQuadState* shared_quad_state =
      render_pass->CreateAndAppendSharedQuadState();
  PopulateScaledSharedQuadState(shared_quad_state, internal_contents_scale_,
                                contents_opaque());

  // Appends a dummy quad here, which will be updated later once the resource
  // is ready in UpdateToastTexture(). We don't add a TextureDrawQuad directly
  // because we don't have a ResourceId for it yet, and ValidateQuadResources()
  // would fail. UpdateToastTexture() happens after all quads are appended for all
  // layers.
  gfx::Rect quad_rect(internal_content_bounds_);
  auto* quad = render_pass->CreateAndAppendDrawQuad<viz::SolidColorDrawQuad>();
  quad->SetNew(shared_quad_state, quad_rect, quad_rect, SkColors::kTransparent,
               false);
  ValidateQuadResources(quad);
  placeholder_quad_ = quad;
}

void ToastLayerImpl::UpdateToastTexture(
    DrawMode draw_mode,
    LayerTreeFrameSink* layer_tree_frame_sink,
    viz::ClientResourceProvider* resource_provider,
    bool gpu_raster,
    const viz::CompositorRenderPassList& list) {
  viz::DrawQuad* toast_quad = placeholder_quad_;
  // The `placeholder_quad_` is only valid for the currently drawing RenderPass,
  // and we need to get a new pointer for the next frame. It would become
  // dangling after drawing completes.
  placeholder_quad_ = nullptr;

  if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE) {
    return;
  }

  viz::RasterContextProvider* raster_context_provider = nullptr;
  absl::optional<viz::RasterContextProvider::ScopedRasterContextLock> lock;
  if (gpu_raster) {
    // TODO(penghuang): It would be better to use context_provider() instead of
    // worker_context_provider() if/when it's switched to RasterContextProvider.
    raster_context_provider = layer_tree_frame_sink->worker_context_provider();
    DCHECK(raster_context_provider);
    lock.emplace(raster_context_provider);
    DCHECK(raster_context_provider->ContextCapabilities().supports_oop_raster);
  }

  auto* context_provider = layer_tree_frame_sink->context_provider();
  if (!pool_) {
    scoped_refptr<base::SingleThreadTaskRunner> task_runner =
        layer_tree_impl()->task_runner_provider()->HasImplThread()
            ? layer_tree_impl()->task_runner_provider()->ImplThreadTaskRunner()
            : layer_tree_impl()->task_runner_provider()->MainThreadTaskRunner();
    pool_ = std::make_unique<ResourcePool>(
        resource_provider, context_provider, std::move(task_runner),
        ResourcePool::kDefaultExpirationDelay,
        layer_tree_impl()->settings().disallow_non_exact_resource_reuse);
  }

  // Return ownership of the previous frame's resource to the pool, so we
  // can reuse it once it is not busy in the display compositor. This is safe to
  // do here because the previous frame has been shipped to the display
  // compositor by the time we UpdateToastTexture for the current frame.
  if (in_flight_resource_)
    pool_->ReleaseResource(std::move(in_flight_resource_));

  // Allocate a backing for the resource if needed, either for gpu or software
  // compositing.
  ResourcePool::InUsePoolResource pool_resource;
  bool needs_clear = false;
  if (draw_mode == DRAW_MODE_HARDWARE) {
    const auto& caps = gpu_raster
                           ? raster_context_provider->ContextCapabilities()
                           : context_provider->ContextCapabilities();
    viz::SharedImageFormat format =
        gpu_raster ? viz::PlatformColor::BestSupportedRenderBufferFormat(caps)
                   : viz::PlatformColor::BestSupportedTextureFormat(caps);
    pool_resource = pool_->AcquireResource(internal_content_bounds_, format,
                                           gfx::ColorSpace());

    if (!pool_resource.gpu_backing()) {
      auto backing = std::make_unique<ToastGpuBacking>();
      auto* sii = gpu_raster ? raster_context_provider->SharedImageInterface()
                             : context_provider->SharedImageInterface();
      backing->shared_image_interface = sii;
      backing->InitOverlayCandidateAndTextureTarget(
          pool_resource.format(), caps,
          layer_tree_impl()
              ->settings()
              .resource_settings.use_gpu_memory_buffer_resources);

      uint32_t flags = gpu::SHARED_IMAGE_USAGE_DISPLAY_READ;
      if (gpu_raster) {
        flags |= gpu::SHARED_IMAGE_USAGE_RASTER |
                 gpu::SHARED_IMAGE_USAGE_OOP_RASTERIZATION;
      } else {
        flags |= gpu::SHARED_IMAGE_USAGE_GLES2;
      }
      if (backing->overlay_candidate)
        flags |= gpu::SHARED_IMAGE_USAGE_SCANOUT;
      backing->mailbox = sii->CreateSharedImage(
          pool_resource.format(), pool_resource.size(),
          pool_resource.color_space(), kTopLeft_GrSurfaceOrigin,
          kPremul_SkAlphaType, flags, "HeadsUpDisplayLayer",
          gpu::kNullSurfaceHandle);
      if (gpu_raster) {
        auto* ri = raster_context_provider->RasterInterface();
        ri->WaitSyncTokenCHROMIUM(sii->GenUnverifiedSyncToken().GetConstData());
      } else {
        auto* gl = context_provider->ContextGL();
        gl->WaitSyncTokenCHROMIUM(sii->GenUnverifiedSyncToken().GetConstData());
      }
      pool_resource.set_gpu_backing(std::move(backing));
      needs_clear = true;
    } else if (pool_resource.gpu_backing()->returned_sync_token.HasData()) {
      if (gpu_raster) {
        auto* ri = raster_context_provider->RasterInterface();
        ri->WaitSyncTokenCHROMIUM(
            pool_resource.gpu_backing()->returned_sync_token.GetConstData());
      } else {
        auto* gl = context_provider->ContextGL();
        gl->WaitSyncTokenCHROMIUM(
            pool_resource.gpu_backing()->returned_sync_token.GetConstData());
      }
      pool_resource.gpu_backing()->returned_sync_token = gpu::SyncToken();
    }
  } else {
    DCHECK_EQ(draw_mode, DRAW_MODE_SOFTWARE);

    pool_resource = pool_->AcquireResource(internal_content_bounds_,
                                           viz::SinglePlaneFormat::kRGBA_8888,
                                           gfx::ColorSpace());

    if (!pool_resource.software_backing()) {
      auto backing = std::make_unique<ToastSoftwareBacking>();
      backing->layer_tree_frame_sink = layer_tree_frame_sink;
      backing->shared_bitmap_id = viz::SharedBitmap::GenerateId();
      base::MappedReadOnlyRegion shm =
          viz::bitmap_allocation::AllocateSharedBitmap(pool_resource.size(),
                                                       pool_resource.format());
      backing->shared_mapping = std::move(shm.mapping);

      layer_tree_frame_sink->DidAllocateSharedBitmap(std::move(shm.region),
                                                     backing->shared_bitmap_id);

      pool_resource.set_software_backing(std::move(backing));
    }
  }

  if (gpu_raster) {
    // If using |gpu_raster| we DrawToastContents() directly to a gpu texture,
    // which is wrapped in an SkSurface.
    DCHECK_EQ(draw_mode, DRAW_MODE_HARDWARE);
    DCHECK(pool_resource.gpu_backing());
    auto* backing = static_cast<ToastGpuBacking*>(pool_resource.gpu_backing());

    const auto& size = pool_resource.size();
    RecordPaintCanvas canvas;
    DrawToastContents(&canvas);
    auto display_item_list = base::MakeRefCounted<DisplayItemList>();
    display_item_list->StartPaint();
    display_item_list->push<DrawRecordOp>(canvas.ReleaseAsRecord());
    display_item_list->EndPaintOfUnpaired(gfx::Rect(size));
    display_item_list->Finalize();

    auto* ri = raster_context_provider->RasterInterface();
    constexpr SkColor4f background_color = SkColors::kTransparent;
    constexpr GLuint msaa_sample_count = -1;
    constexpr bool can_use_lcd_text = true;
    ri->BeginRasterCHROMIUM(background_color, needs_clear, msaa_sample_count,
                            gpu::raster::kNoMSAA, can_use_lcd_text,
                            /*visible=*/true, gfx::ColorSpace::CreateSRGB(),
                            backing->mailbox.name);
    constexpr gfx::Vector2dF post_translate(0.f, 0.f);
    constexpr gfx::Vector2dF post_scale(1.f, 1.f);
    DummyImageProvider image_provider;
    size_t max_op_size_limit =
        gpu::raster::RasterInterface::kDefaultMaxOpSizeHint;
    ri->RasterCHROMIUM(display_item_list.get(), &image_provider, size,
                       gfx::Rect(size), gfx::Rect(size), post_translate,
                       post_scale, /*requires_clear=*/false,
                       &max_op_size_limit);
    ri->EndRasterCHROMIUM();
    backing->mailbox_sync_token =
        viz::ClientResourceProvider::GenerateSyncTokenHelper(ri);
  } else if (draw_mode == DRAW_MODE_HARDWARE) {
    // If not using |gpu_raster| but using gpu compositing, we DrawToastContents()
    // into a software bitmap and upload it to a texture for compositing.
    DCHECK(pool_resource.gpu_backing());
    auto* backing = static_cast<ToastGpuBacking*>(pool_resource.gpu_backing());
    gpu::gles2::GLES2Interface* gl = context_provider->ContextGL();

    if (!staging_surface_ ||
        gfx::SkISizeToSize(staging_surface_->getCanvas()->getBaseLayerSize()) !=
            pool_resource.size()) {
      SkSurfaceProps props = skia::LegacyDisplayGlobals::GetSkSurfaceProps();
      staging_surface_ = SkSurface::MakeRasterN32Premul(
          pool_resource.size().width(), pool_resource.size().height(), &props);
    }

    SkiaPaintCanvas canvas(staging_surface_->getCanvas());
    DrawToastContents(&canvas);

    TRACE_EVENT0("cc", "UploadToastTexture");
    SkPixmap pixmap;
    staging_surface_->peekPixels(&pixmap);

    GLuint mailbox_texture_id =
        gl->CreateAndTexStorage2DSharedImageCHROMIUM(backing->mailbox.name);
    gl->BeginSharedImageAccessDirectCHROMIUM(
        mailbox_texture_id, GL_SHARED_IMAGE_ACCESS_MODE_READWRITE_CHROMIUM);

    gl->BindTexture(backing->texture_target, mailbox_texture_id);
    DCHECK(GLSupportsFormat(pool_resource.format().resource_format()));
    // We should use gl compatible format for skia SW rasterization.
    constexpr GLenum format = SK_B32_SHIFT ? GL_RGBA : GL_BGRA_EXT;
    constexpr GLenum type = GL_UNSIGNED_BYTE;
    gl->TexSubImage2D(
        backing->texture_target, 0, 0, 0, pool_resource.size().width(),
        pool_resource.size().height(), format, type, pixmap.addr());

    gl->EndSharedImageAccessDirectCHROMIUM(mailbox_texture_id);
    gl->DeleteTextures(1, &mailbox_texture_id);
    backing->mailbox_sync_token =
        viz::ClientResourceProvider::GenerateSyncTokenHelper(gl);
  } else {
    // If not using gpu compositing, we DrawToastContent() directly into a shared
    // memory bitmap, wrapped in an SkSurface, that can be shared to the display
    // compositor.
    DCHECK_EQ(draw_mode, DRAW_MODE_SOFTWARE);
    DCHECK(pool_resource.software_backing());

    SkImageInfo info = SkImageInfo::MakeN32Premul(
        pool_resource.size().width(), pool_resource.size().height());
    auto* backing =
        static_cast<ToastSoftwareBacking*>(pool_resource.software_backing());
    SkSurfaceProps props = skia::LegacyDisplayGlobals::GetSkSurfaceProps();
    sk_sp<SkSurface> surface = SkSurface::MakeRasterDirect(
        info, backing->shared_mapping.memory(), info.minRowBytes(), &props);

    SkiaPaintCanvas canvas(surface->getCanvas());
    DrawToastContents(&canvas);
  }

  // Exports the backing to the ResourceProvider, giving it a ResourceId that
  // can be used in a DrawQuad.
  bool exported = pool_->PrepareForExport(pool_resource);
  DCHECK(exported);
  viz::ResourceId resource_id = pool_resource.resource_id_for_export();

  // Save the resource to prevent reuse until it is exported to the display
  // compositor. Next time we come here, we can release it back to the pool as
  // it will be exported by then.
  in_flight_resource_ = std::move(pool_resource);

  // This iterates over the RenderPass list of quads to find the toast quad, which
  // will always be in the root RenderPass.
  auto& render_pass = list.back();
  for (auto it = render_pass->quad_list.begin();
       it != render_pass->quad_list.end(); ++it) {
    if (*it == toast_quad) {
      const viz::SharedQuadState* sqs = toast_quad->shared_quad_state;
      gfx::Rect quad_rect = toast_quad->rect;
      gfx::Rect visible_rect = toast_quad->visible_rect;

      auto* quad =
          render_pass->quad_list.ReplaceExistingElement<viz::TextureDrawQuad>(
              it);

      // The acquired resource's size could be bigger than actually needed due
      // to reuse. In this case, only use the part of the texture that is within
      // the bounds.
      gfx::PointF uv_bottom_right(1.f, 1.f);
      if (in_flight_resource_.size() != internal_content_bounds_) {
        uv_bottom_right.set_x(
            static_cast<double>(internal_content_bounds_.width()) /
            static_cast<double>(in_flight_resource_.size().width()));
        uv_bottom_right.set_y(
            static_cast<double>(internal_content_bounds_.height()) /
            static_cast<double>(in_flight_resource_.size().height()));
      }
      const float vertex_opacity[] = {1.f, 1.f, 1.f, 1.f};
      quad->SetNew(sqs, quad_rect, visible_rect, /*needs_blending=*/true,
                   resource_id, /*premultiplied_alpha=*/true,
                   /*uv_top_left=*/gfx::PointF(),
                   /*uv_bottom_right=*/uv_bottom_right,
                   /*background_color=*/SkColors::kTransparent, vertex_opacity,
                   /*flipped=*/false,
                   /*nearest_neighbor=*/false, /*secure_output_only=*/false,
                   gfx::ProtectedVideoType::kClear);
      ValidateQuadResources(quad);
      break;
    }
  }
}

void ToastLayerImpl::ReleaseResources() {
  if (in_flight_resource_)
    pool_->ReleaseResource(std::move(in_flight_resource_));
  pool_.reset();
}

gfx::Rect ToastLayerImpl::GetEnclosingVisibleRectInTargetSpace()
    const {
  DCHECK_GT(internal_contents_scale_, 0.f);
  return GetScaledEnclosingVisibleRectInTargetSpace(internal_contents_scale_);
}

bool ToastLayerImpl::IsAnimatingToastContents() const {
  return toast_fade_step_ > 0;
}

void ToastLayerImpl::SetToastTypeface(sk_sp<SkTypeface> typeface) {
  if (typeface_ == typeface)
    return;

  DCHECK(typeface_.get() == nullptr);
  typeface_ = std::move(typeface);
  NoteLayerPropertyChanged();
}

void ToastLayerImpl::PushPropertiesTo(LayerImpl* layer) {
  LayerImpl::PushPropertiesTo(layer);

  ToastLayerImpl* layer_impl =
      static_cast<ToastLayerImpl*>(layer);
  layer_impl->SetToastTypeface(typeface_);
  layer_impl->SetToastMessage(toast_message_);
}

void ToastLayerImpl::DrawToastContents(PaintCanvas* canvas) {
  TRACE_EVENT0("cc", "DrawToastContents");

  canvas->clear(SkColors::kTransparent);

  if (!IsAnimatingToastContents()) {
    canvas->restore();
    return;
  }

  layer_tree_impl()->SetNeedsRedraw();

  canvas->save();
  canvas->scale(internal_contents_scale_);

  // Our output should be in layout space, but all of the draw commands for the
  // toast overlays here are in dips. Scale the canvas to account for this
  // difference.
  canvas->scale(layer_tree_impl()->painted_device_scale_factor());

  // tost display state : fadein -> display -> fadeout
  base::TimeDelta timeToRemain = animationEndTime_ - base::TimeTicks::Now();
  toast_fade_step_ = static_cast<int>(timeToRemain / kAnimationDuration * 100);
  if (toast_fade_step_ < 0) {
    toast_fade_step_ = 0;
  }
  int current_step = kTotalDisplaySteps - toast_fade_step_;
  int alpha = CalculateAlpha(current_step);

  SkColor background_color = ScaleSkColorAlpha(kToastBackgroundColor, alpha);

  const int padding_horizontal = kToastHeight / 2;
  const int min_width = kToastFontSize * 3 + padding_horizontal * 2;
  const int radius = kToastHeight / 2;

  const int bounds_width = bounds_width_in_dips();
  const int bounds_height =
      bounds().height() / layer_tree_impl()->painted_device_scale_factor();

  if (!toast_message_.ToastMessage() || toast_message_.ToastMessage()->empty()) {
    int width = min_width;
    int height = kToastHeight;
    int x = (bounds_width - width) / 2;
    int y = (bounds_height - height) / 2;
    SkRect rect = SkRect::MakeXYWH(x, y, width, height);
    SkScalar rx = radius;
    SkScalar ry = radius;

    PaintFlags shadow_flags;
    shadow_flags.setAntiAlias(true);
    shadow_flags.setStyle(PaintFlags::kFill_Style);
    shadow_flags.setColor(SkColors::kBlack);
    shadow_flags.setImageFilter(sk_make_sp<BlurPaintFilter>(
        3, 3, SkTileMode::kClamp, nullptr));
    canvas->drawRoundRect(rect, rx, ry, shadow_flags);

    PaintFlags flags;
    flags.setAntiAlias(true);
    flags.setColor(background_color);
    canvas->drawRoundRect(rect, rx, ry, flags);
    return;
  }

  SkFont font(typeface_, kToastFontSize);
  font.setEdging(SkFont::Edging::kAntiAlias);

  auto message_width = font.measureText(toast_message_.ToastMessage()->c_str(),
      toast_message_.ToastMessage()->length(), SkTextEncoding::kUTF8);

  SkScalar toast_width = message_width + padding_horizontal * 2;
  SkScalar x = (bounds_width - toast_width) / 2;
  SkScalar y = (bounds_height - kToastHeight) / 2;
  SkRect rect = SkRect::MakeXYWH(x, y, toast_width, kToastHeight);
  SkScalar rx = radius;
  SkScalar ry = radius;

  PaintFlags shadow_flags;
  shadow_flags.setAntiAlias(true);
  shadow_flags.setStyle(PaintFlags::kFill_Style);
  shadow_flags.setColor(SkColor4f::FromColor(background_color));
  shadow_flags.setImageFilter(sk_make_sp<DropShadowPaintFilter>(
      0, kToastShadowOffsetY, kToastHeight, kToastHeight,
      SkColor4f::FromColor(ScaleSkColorAlpha(kToastShadowColor, alpha)),
      DropShadowPaintFilter::ShadowMode::kDrawShadowOnly, nullptr));
  canvas->drawRoundRect(rect, rx, ry, shadow_flags);

  PaintFlags flags;
  flags.setAntiAlias(true);
  flags.setColor(background_color);
  canvas->drawRoundRect(rect, rx, ry, flags);

  SkFontMetrics metrics;
  font.getMetrics(&metrics);
  SkScalar font_height = metrics.fCapHeight;
  if (font_height == 0) {
    font_height = metrics.fAscent;
  }
  if (font_height == 0) {
    font_height = kToastFontSize;
  }
  x = (bounds_width - message_width) / 2;
  y = (bounds_height + font_height) / 2;
  const SkColor fontColor = ScaleSkColorAlpha(kToastTextColor, alpha);
  flags.setColor(fontColor);
  canvas->drawTextBlob(SkTextBlob::MakeFromText(
      toast_message_.ToastMessage()->c_str(),
      toast_message_.ToastMessage()->length(), font), x, y, flags);

  canvas->restore();
}

const char* ToastLayerImpl::LayerTypeAsString() const {
  return "cc::ToastLayerImpl";
}

void ToastLayerImpl::AsValueInto(
    base::trace_event::TracedValue* dict) const {
  LayerImpl::AsValueInto(dict);
  dict->SetString("layer_name", "Toast Layer");
}

void ToastLayerImpl::SetToastMessage(const LayerTreeExtraState& toast_message) {
  if (!toast_message.ShowToast()) {
    return;
  }
  if (LayerTreeExtraState::Equal(toast_message_, toast_message)) {
    return;
  }
  toast_message_ = toast_message;
  animationEndTime_ = base::TimeTicks::Now() + kAnimationDuration;
  toast_fade_step_ = kTotalDisplaySteps;
}

}  // namespace cc
