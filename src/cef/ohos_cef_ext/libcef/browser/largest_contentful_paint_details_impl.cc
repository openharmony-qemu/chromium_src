// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef/browser/largest_contentful_paint_details_impl.h"

CefLargestContentfulPaintDetailsImpl::CefLargestContentfulPaintDetailsImpl(
    int64_t navigationStartTime,
    int64_t largestImagePaintTime,
    int64_t largestTextPaintTime,
    int64_t largestImageLoadStartTime,
    int64_t largestImageLoadEndTime,
    double_t imageBPP)
    : navigationStartTime_(navigationStartTime),
      largestImagePaintTime_(largestImagePaintTime),
      largestTextPaintTime_(largestTextPaintTime),
      largestImageLoadStartTime_(largestImageLoadStartTime),
      largestImageLoadEndTime_(largestImageLoadEndTime),
      imageBPP_(imageBPP) {}

int64_t CefLargestContentfulPaintDetailsImpl::GetNavigationStartTime() {
  return navigationStartTime_;
}

int64_t CefLargestContentfulPaintDetailsImpl::GetLargestImagePaintTime() {
  return largestImagePaintTime_;
}

int64_t CefLargestContentfulPaintDetailsImpl::GetLargestTextPaintTime() {
  return largestTextPaintTime_;
}

int64_t CefLargestContentfulPaintDetailsImpl::GetLargestImageLoadStartTime() {
  return largestImageLoadStartTime_;
}

int64_t CefLargestContentfulPaintDetailsImpl::GetLargestImageLoadEndTime() {
  return largestImageLoadEndTime_;
}

double_t CefLargestContentfulPaintDetailsImpl::GetImageBPP() {
  return imageBPP_;
}
 