/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_DRAG_DATA_IMPL_H
#define NWEB_DRAG_DATA_IMPL_H

#include <vector>

#include "cef/include/cef_drag_data.h"
#include "ohos_nweb/include/nweb_drag_data.h"

#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkPath.h"

namespace OHOS::NWeb {
class NWebDragDataImpl : public NWebDragData {
 public:
  NWebDragDataImpl() = delete;
  explicit NWebDragDataImpl(CefRefPtr<CefDragData> drag_data);
  NWebDragDataImpl(CefRefPtr<CefDragData> drag_data, CefPoint& drag_touch_point,
    std::vector<CefPoint>& start_edge, std::vector<CefPoint>& end_edge,
    float device_pixel_ratio, bool is_useful_selection,
    bool dark_mode_enable);
  ~NWebDragDataImpl() = default;

  // get the link URL that is being dragged.
  std::string GetLinkURL() override;

  // get the plain text that is being dragged.
  std::string GetFragmentText() override;

  // get the text/html fragment that is being dragged.
  std::string GetFragmentHtml()  override;

  // get the image representation of drag data.
  bool GetPixelMapSetting(const void** data, size_t& len, int& width, int& height)  override;

  // set the link URL that is being dragged.
  bool SetLinkURL(const std::string& url) override;

  // set the plain text that is being dragged.
  bool SetFragmentText(const std::string& Text) override;

  // set the text/html fragment that is being dragged.
  bool SetFragmentHtml(const std::string& html) override;

  // set the image representation of drag data.
  bool SetPixelMapSetting(const void* data, size_t len, int width, int height) override;

  // get the title associated with the link that is being dragged.
  std::string GetLinkTitle() override;

  // set the title associated with the link that is being dragged.
  bool SetLinkTitle(const std::string& title) override;

  void GetDragStartPosition(int& x, int& y) override;

  bool IsSingleImageContent() override;

  bool SetFileUri(const std::string& uri) override;

  std::string GetImageFileName() override;

  void ClearImageFileNames() override;

  CefRefPtr<CefDragData> GetDragData() {
    return drag_data_;
  }

  void FreePixlMapData();

 private:
  void* image_buffer_ = nullptr;
  void GenerateOhosDragBitmapFromOrigin(const SkBitmap& in_bitmap, SkBitmap& out_bitmap, int& width, int& height);
  void GenerateOhosDragBitmapFromOriginForRichtext(const SkBitmap& in_bitmap, SkBitmap& out_bitmap,
                                                   int& width, int& height);
  void GenerateOhosDragBitmapFromOriginForImage(const SkBitmap& in_bitmap, SkBitmap& out_bitmap,
                                                int& width, int& height);
  SkPath GetClippedPath(bool is_start_line_compelete, bool is_end_line_compelete);
  SkPath ClipLeftTopCorner(SkPath& origin_path, int pos_x, int pos_y, int radius);
  SkPath ClipRightBottomCorner(SkPath& origin_path, int pos_x, int pos_y, int radius);
  void AddRightBottomCorner(SkPath& origin_path, int pos_x, int pos_y, int radius);
  void AddLeftTopCorner(SkPath& origin_path, int pos_x, int pos_y, int radius);
  SkPath GetShadowPath(int width, int height);
  float ToOhCoordinate(int origin);
  CefRefPtr<CefDragData> drag_data_ = nullptr;
  CefPoint drag_image_origin_point_;
  CefPoint start_edge_top_;
  CefPoint start_edge_bottom_;
  CefPoint end_edge_top_;
  CefPoint end_edge_bottom_;
  int drag_clip_width_ = 0;
  int drag_clip_height_ = 0;
  float device_pixel_ratio_ = 0;
  bool is_useful_selection_ = true;
  bool dark_mode_enable_ = false;
};
}  // namespace OHOS::NWeb
#endif