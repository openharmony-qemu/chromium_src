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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "chrome/browser/content_settings/host_content_settings_map_factory.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "content/public/common/content_switches.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/sys_info_utils_ext.h"
#include "content/public/browser/render_widget_host.h"
#include "libcef/browser/osr/render_widget_host_view_osr.h"
#endif

namespace renderer_prefs {

#if BUILDFLAG(IS_ARKWEB)
void SetDefaultPrefsExt(blink::web_pref::WebPreferences& web) {
  // These OHOS default prefs defined in web_preferences.cc is only used for
  // none PC device, PC device has different prefs as below.
  if (base::ohos::IsPcDevice() || base::ohos::IsPcMode()) {
    web.viewport_meta_enabled = false;
    web.auto_zoom_focused_editable_to_legible_scale = false;
    web.shrinks_viewport_contents_to_fit = false;
    web.viewport_style = blink::mojom::ViewportStyle::kDefault;
    web.always_show_context_menu_on_touch = true;
    web.smooth_scroll_for_find_enabled = false;
    web.main_frame_resizes_are_orientation_changes = false;

    web.double_tap_to_zoom_enabled = false;

    web.text_autosizing_enabled = false;

    web.default_minimum_page_scale_factor = 1.f;
    web.default_maximum_page_scale_factor = 4.f;
  }
}
#endif

}  // namespace renderer_prefs
