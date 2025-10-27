// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_LARGEST_CONTENTFUL_PAINT_DETAILS_IMPL_H_
#define CEF_LIBCEF_BROWSER_LARGEST_CONTENTFUL_PAINT_DETAILS_IMPL_H_
#pragma once

#include "include/cef_largest_contentful_paint_details.h"
#include "libcef/common/value_base.h"

// CefLargestContentfulPaintDetails implementation
class CefLargestContentfulPaintDetailsImpl
    : public CefLargestContentfulPaintDetails {
 public:
  explicit CefLargestContentfulPaintDetailsImpl(
      int64_t navigationStartTime,
      int64_t largestImagePaintTime,
      int64_t largestTextPaintTime,
      int64_t largestImageLoadStartTime,
      int64_t largestImageLoadEndTime,
      double_t imageBPP);

  CefLargestContentfulPaintDetailsImpl(
      const CefLargestContentfulPaintDetailsImpl&) = delete;
  CefLargestContentfulPaintDetailsImpl& operator=(
      const CefLargestContentfulPaintDetailsImpl&) = delete;

  // CefLargestContentfulPaintDetailsImpl methods.
  int64_t GetNavigationStartTime() override;
  int64_t GetLargestImagePaintTime() override;
  int64_t GetLargestTextPaintTime() override;
  int64_t GetLargestImageLoadStartTime() override;
  int64_t GetLargestImageLoadEndTime() override;
  double_t GetImageBPP() override;

 private:
  int64_t navigationStartTime_;
  int64_t largestImagePaintTime_;
  int64_t largestTextPaintTime_;
  int64_t largestImageLoadStartTime_;
  int64_t largestImageLoadEndTime_;
  double_t imageBPP_;

  IMPLEMENT_REFCOUNTING(CefLargestContentfulPaintDetailsImpl);
};

#endif  // CEF_LIBCEF_BROWSER_LARGEST_CONTENTFUL_PAINT_DETAILS_IMPL_H_
