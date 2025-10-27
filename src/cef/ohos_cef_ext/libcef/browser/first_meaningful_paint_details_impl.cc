// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef/browser/first_meaningful_paint_details_impl.h"

CefFirstMeaningfulPaintDetailsImpl::CefFirstMeaningfulPaintDetailsImpl(
    int64_t navigationStartTime,
    int64_t firstMeaningfulPaintTime)
    : navigationStartTime_(navigationStartTime),
      firstMeaningfulPaintTime_(firstMeaningfulPaintTime) {}

int64_t CefFirstMeaningfulPaintDetailsImpl::GetNavigationStartTime() {
  return navigationStartTime_;
}

int64_t CefFirstMeaningfulPaintDetailsImpl::GetFirstMeaningfulPaintTime() {
  return firstMeaningfulPaintTime_;
}
 