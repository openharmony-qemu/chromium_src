/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/cpptoc/ark_web_drag_data_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

bool ARK_WEB_CALLBACK ark_web_drag_data_set_file_uri(
    struct _ark_web_drag_data_t *self, const ArkWebString *uri) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(uri, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetFileUri(*uri);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_drag_data_get_link_url(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetLinkURL();
}

bool ARK_WEB_CALLBACK ark_web_drag_data_set_link_url(
    struct _ark_web_drag_data_t *self, const ArkWebString *url) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(url, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetLinkURL(*url);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_drag_data_get_link_title(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetLinkTitle();
}

bool ARK_WEB_CALLBACK ark_web_drag_data_set_link_title(
    struct _ark_web_drag_data_t *self, const ArkWebString *title) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(title, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetLinkTitle(*title);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_drag_data_get_fragment_text(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetFragmentText();
}

bool ARK_WEB_CALLBACK ark_web_drag_data_set_fragment_text(
    struct _ark_web_drag_data_t *self, const ArkWebString *text) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(text, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetFragmentText(*text);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_drag_data_get_fragment_html(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetFragmentHtml();
}

bool ARK_WEB_CALLBACK ark_web_drag_data_set_fragment_html(
    struct _ark_web_drag_data_t *self, const ArkWebString *html) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(html, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetFragmentHtml(*html);
}

ArkWebString ARK_WEB_CALLBACK
ark_web_drag_data_get_image_file_name(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetImageFileName();
}

bool ARK_WEB_CALLBACK ark_web_drag_data_get_pixel_map_setting(
    struct _ark_web_drag_data_t *self, const void **data, size_t *len,
    int *width, int *height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(len, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(width, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(height, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->GetPixelMapSetting(data, *len, *width,
                                                             *height);
}

bool ARK_WEB_CALLBACK ark_web_drag_data_set_pixel_map_setting(
    struct _ark_web_drag_data_t *self, const void *data, size_t len, int width,
    int height) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  ARK_WEB_CPPTOC_CHECK_PARAM(data, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->SetPixelMapSetting(data, len, width,
                                                             height);
}

void ARK_WEB_CALLBACK
ark_web_drag_data_clear_image_file_names(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  // Execute
  ArkWebDragDataCppToC::Get(self)->ClearImageFileNames();
}

bool ARK_WEB_CALLBACK
ark_web_drag_data_is_single_image_content(struct _ark_web_drag_data_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

  // Execute
  return ArkWebDragDataCppToC::Get(self)->IsSingleImageContent();
}

void ARK_WEB_CALLBACK ark_web_drag_data_get_drag_start_position(
    struct _ark_web_drag_data_t *self, int *x, int *y) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(x, );

  ARK_WEB_CPPTOC_CHECK_PARAM(y, );

  // Execute
  ArkWebDragDataCppToC::Get(self)->GetDragStartPosition(*x, *y);
}

} // namespace

ArkWebDragDataCppToC::ArkWebDragDataCppToC() {
  GetStruct()->set_file_uri = ark_web_drag_data_set_file_uri;
  GetStruct()->get_link_url = ark_web_drag_data_get_link_url;
  GetStruct()->set_link_url = ark_web_drag_data_set_link_url;
  GetStruct()->get_link_title = ark_web_drag_data_get_link_title;
  GetStruct()->set_link_title = ark_web_drag_data_set_link_title;
  GetStruct()->get_fragment_text = ark_web_drag_data_get_fragment_text;
  GetStruct()->set_fragment_text = ark_web_drag_data_set_fragment_text;
  GetStruct()->get_fragment_html = ark_web_drag_data_get_fragment_html;
  GetStruct()->set_fragment_html = ark_web_drag_data_set_fragment_html;
  GetStruct()->get_image_file_name = ark_web_drag_data_get_image_file_name;
  GetStruct()->get_pixel_map_setting = ark_web_drag_data_get_pixel_map_setting;
  GetStruct()->set_pixel_map_setting = ark_web_drag_data_set_pixel_map_setting;
  GetStruct()->clear_image_file_names =
      ark_web_drag_data_clear_image_file_names;
  GetStruct()->is_single_image_content =
      ark_web_drag_data_is_single_image_content;
  GetStruct()->get_drag_start_position =
      ark_web_drag_data_get_drag_start_position;
}

ArkWebDragDataCppToC::~ArkWebDragDataCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebDragDataCppToC, ArkWebDragData,
                                        ark_web_drag_data_t>::kBridgeType =
    ARK_WEB_DRAG_DATA;

} // namespace OHOS::ArkWeb
