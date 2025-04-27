// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_NATIVE_THEME_OVERLAY_SCROLLBAR_CONSTANTS_AURA_H_
#define UI_NATIVE_THEME_OVERLAY_SCROLLBAR_CONSTANTS_AURA_H_

#include "base/time/time.h"
#include "ui/gfx/geometry/skia_conversions.h"

namespace ui {

#ifdef OHOS_SCROLLBAR
constexpr int kOverlayScrollbarStrokeWidth = 0;
constexpr int kOverlayScrollbarThumbWidthPressed = 8;
constexpr base::TimeDelta kOverlayScrollbarFadeDelay = base::Milliseconds(2000);
constexpr base::TimeDelta kOverlayScrollbarFadeDuration = base::Milliseconds(300);
#else
constexpr int kOverlayScrollbarStrokeWidth = 1;
constexpr int kOverlayScrollbarThumbWidthPressed = 10;
constexpr base::TimeDelta kOverlayScrollbarFadeDelay = base::Milliseconds(500);
constexpr base::TimeDelta kOverlayScrollbarFadeDuration = base::Milliseconds(200);
#endif // OHOS_SCROLLBAR

// TODO(bokan): This is still undetermined. crbug.com/652520.
constexpr base::TimeDelta kOverlayScrollbarThinningDuration =
    base::Milliseconds(200);

}  // namespace ui

#endif  // UI_NATIVE_THEME_OVERLAY_SCROLLBAR_CONSTANTS_AURA_H_
