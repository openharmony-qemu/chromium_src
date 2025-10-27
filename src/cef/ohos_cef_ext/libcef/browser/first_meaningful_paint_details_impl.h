// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H_
#define CEF_LIBCEF_BROWSER_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H_
#pragma once

#include "include/cef_first_meaningful_paint_details.h"
#include "libcef/common/value_base.h"

// CefFirstMeaningfulPaintDetails implementation
class CefFirstMeaningfulPaintDetailsImpl
    : public CefFirstMeaningfulPaintDetails {
 public:
  explicit CefFirstMeaningfulPaintDetailsImpl(int64_t navigationStartTime,
                                              int64_t firstMeaningfulPaintTime);

  CefFirstMeaningfulPaintDetailsImpl(
      const CefFirstMeaningfulPaintDetailsImpl&) = delete;
  CefFirstMeaningfulPaintDetailsImpl& operator=(
      const CefFirstMeaningfulPaintDetailsImpl&) = delete;

  // CefFirstMeaningfulPaintDetailsImpl methods.
  int64_t GetNavigationStartTime() override;
  int64_t GetFirstMeaningfulPaintTime() override;

 private:
  int64_t navigationStartTime_;
  int64_t firstMeaningfulPaintTime_;

  IMPLEMENT_REFCOUNTING(CefFirstMeaningfulPaintDetailsImpl);
};

#endif  // CEF_LIBCEF_BROWSER_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H_
