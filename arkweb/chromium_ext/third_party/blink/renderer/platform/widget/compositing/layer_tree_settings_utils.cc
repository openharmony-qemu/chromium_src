/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/widget/compositing/layer_tree_settings_utils.h"
#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/system/sys_info.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/platform/platform.h"
#include "ui/base/ui_base_features.h"
#include "ui/base/ui_base_switches.h"
#include "ui/native_theme/native_theme_features.h"
#include "ui/native_theme/native_theme_utils.h"
#include "ui/native_theme/overlay_scrollbar_constants_aura.h"

namespace blink {

extern bool IsSmallScreen(const gfx::Size& size);

void AdjustMemoryLimitBasedOnScreenWidth(
    cc::ManagedMemoryPolicy& actual,
    const gfx::Size& initial_screen_size,
    float initial_device_scale_factor)
{
  // Ignore what the system said and give all clients the same maximum
  // allocation on desktop platforms.
  actual.bytes_limit_when_visible = 512 * 1024 * 1024;
  actual.priority_cutoff_when_visible =
      gpu::MemoryAllocation::CUTOFF_ALLOW_NICE_TO_HAVE;

  // For large monitors (4k), double the tile memory to avoid frequent out of
  // memory problems. 4k could mean a screen width of anywhere from 3840 to 4096
  // (see https://en.wikipedia.org/wiki/4K_resolution). We use 3500 as a proxy
  // for "large enough".
  static const int kLargeDisplayThreshold = 3500;
  int display_width =
      std::round(initial_screen_size.width() * initial_device_scale_factor);
  if (display_width >= kLargeDisplayThreshold) {
    actual.bytes_limit_when_visible *= 2;
  }
}

size_t EstimatePhysicalMemory()
{
#if defined(OS_ANDROID)
  size_t physical_memory_mb = 0;
  // We can't query available GPU memory from the system on Android.
  // Physical memory is also mis-reported sometimes (eg. Nexus 10 reports
  // 1262MB when it actually has 2GB, while Razr M has 1GB but only reports
  // 128MB java heap size). First we estimate physical memory using both.
  size_t dalvik_mb = base::SysInfo::DalvikHeapSizeMB();
  size_t physical_mb = base::SysInfo::AmountOfPhysicalMemoryMB();

  if (base::SysInfo::IsLowEndDevice()) {
    // TODO(crbug.com/742534): The code below appears to no longer work.
    // |dalvik_mb| no longer follows the expected heuristic pattern, causing us
    // to over-estimate memory on low-end devices. This entire section probably
    // needs to be re-written, but for now we can address the low-end Android
    // issues by ignoring |dalvik_mb|.
    physical_memory_mb = physical_mb;
  } else if (dalvik_mb >= 256) {
    physical_memory_mb = dalvik_mb * 4;
  } else {
    physical_memory_mb = std::max(dalvik_mb * 4, (physical_mb * 4) / 3);
  }
  return physical_memory_mb;
#endif
}

void CalculateVisibleMemoryLimit(cc::ManagedMemoryPolicy& actual,
                                 size_t physical_memory_mb)
{
  // Now we take a default of 1/8th of memory on high-memory devices,
  // and gradually scale that back for low-memory devices (to be nicer
  // to other apps so they don't get killed). Examples:
  // Nexus 4/10(2GB)    256MB (normally 128MB)
  // Droid Razr M(1GB)  114MB (normally 57MB)
  // Galaxy Nexus(1GB)  100MB (normally 50MB)
  // Xoom(1GB)          100MB (normally 50MB)
  // Nexus S(low-end)   8MB (normally 8MB)
  // Note that the compositor now uses only some of this memory for
  // pre-painting and uses the rest only for 'emergencies'.
  if (actual.bytes_limit_when_visible == 0) {
    // NOTE: Non-low-end devices use only 50% of these limits,
    // except during 'emergencies' where 100% can be used.
    if (physical_memory_mb >= 1536) {
      actual.bytes_limit_when_visible = physical_memory_mb / 8;  // >192MB
    } else if (physical_memory_mb >= 1152) {
      actual.bytes_limit_when_visible = physical_memory_mb / 8;  // >144MB
    } else if (physical_memory_mb >= 768) {
      actual.bytes_limit_when_visible = physical_memory_mb / 10;  // >76MB
    } else if (physical_memory_mb >= 513) {
      actual.bytes_limit_when_visible = physical_memory_mb / 12;  // <64MB
    } else {
      // Devices with this little RAM have very little headroom so we hardcode
      // the limit rather than relying on the heuristics above.  (They also use
      // 4444 textures so we can use a lower limit.)
      actual.bytes_limit_when_visible = 8;
    }

    actual.bytes_limit_when_visible =
        actual.bytes_limit_when_visible * 1024 * 1024;
    // Clamp the observed value to a specific range on Android or ARKWEB.
    actual.bytes_limit_when_visible = std::max(
        actual.bytes_limit_when_visible, static_cast<size_t>(8 * 1024 * 1024));
    actual.bytes_limit_when_visible =
        std::min(actual.bytes_limit_when_visible,
                 static_cast<size_t>(256 * 1024 * 1024));
  }
  actual.priority_cutoff_when_visible =
      gpu::MemoryAllocation::CUTOFF_ALLOW_EVERYTHING;
}

void SetMaxVisibleBytes(cc::ManagedMemoryPolicy& actual)
{
  Platform* platform = Platform::Current();
  if (platform->GetDrawMode()) {
    actual.bytes_limit_when_visible =
        std::min(actual.bytes_limit_when_visible,
                 static_cast<size_t>(2000 * 1024 * 1024));
  }
}

void ConfigureOverlayScrollbarSettings(cc::LayerTreeSettings& settings)
{
  if (ui::IsOverlayScrollbarEnabled()) {
    settings.scrollbar_animator = cc::LayerTreeSettings::AURA_OVERLAY;
    settings.scrollbar_fade_delay = ui::kOverlayScrollbarFadeDelay;
    settings.scrollbar_fade_duration = ui::kOverlayScrollbarFadeDuration;
    settings.scrollbar_thinning_duration =
        ui::kOverlayScrollbarThinningDuration;
    settings.scrollbar_flash_after_any_scroll_update = true;
  }
}

void AdjustGraphicsSettings(const gfx::Size& screen_size,
                            cc::LayerTreeSettings& settings,
                            bool using_low_memory_policy)
{
  if (using_low_memory_policy) {
    // On low-end we want to be very careful about killing other
    // apps. So initially we use 50% more memory to avoid flickering
    // or raster-on-demand.
    settings.max_memory_for_prepaint_percentage = 67;
  } else {
    // On other devices we have increased memory excessively to avoid
    // raster-on-demand already, so now we reserve 50% _only_ to avoid
    // raster-on-demand, and use 50% of the memory otherwise.
    settings.max_memory_for_prepaint_percentage = 50;
  }

  if (ui::IsOverlayScrollbarEnabled()) {
    settings.scrollbar_animator = cc::LayerTreeSettings::AURA_OVERLAY;
    settings.scrollbar_fade_delay = ui::kOverlayScrollbarFadeDelay;
    settings.scrollbar_fade_duration = ui::kOverlayScrollbarFadeDuration;
    settings.scrollbar_thinning_duration =
        ui::kOverlayScrollbarThinningDuration;
    settings.scrollbar_flash_after_any_scroll_update = false;
  }

  // TODO(danakj): Only do this on low end devices.
  settings.create_low_res_tiling = true;
}

void SetEnableDeleteUnusedResourcesDelay(cc::LayerTreeSettings& settings)
{
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx) &&
      !base::SysInfo::IsLowEndDevice()) {
    bool excludable_devices =
        base::ohos::IsTabletDevice() || base::ohos::IsPcDevice();

    settings.enable_delete_unused_resources_delay = !excludable_devices;
  }
}
}  // namespace blink
