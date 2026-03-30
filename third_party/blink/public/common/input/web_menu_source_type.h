// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_PUBLIC_COMMON_INPUT_WEB_MENU_SOURCE_TYPE_H_
#define THIRD_PARTY_BLINK_PUBLIC_COMMON_INPUT_WEB_MENU_SOURCE_TYPE_H_

#include "arkweb/build/features/features.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace blink {

enum WebMenuSourceType {
  kMenuSourceNone,
  kMenuSourceMouse,
  kMenuSourceKeyboard,
  kMenuSourceTouch,
  kMenuSourceTouchEditMenu,
  kMenuSourceLongPress,
  kMenuSourceLongTap,
  kMenuSourceTouchHandle,
  kMenuSourceStylus,
  kMenuSourceAdjustSelection,
  kMenuSourceAdjustSelectionReset,
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  kMenuSourceShowFreeCopyMenu,
  kMenuSourceTypeLast = kMenuSourceShowFreeCopyMenu
#else
  kMenuSourceTypeLast = kMenuSourceAdjustSelectionReset
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_PUBLIC_COMMON_INPUT_WEB_MENU_SOURCE_TYPE_H_
