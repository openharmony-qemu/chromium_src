// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_LAYER_TREE_EXTRA_STATE_H_
#define CC_DEBUG_LAYER_TREE_EXTRA_STATE_H_

#include <string>
#include "cc/debug/debug_export.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace cc {

class CC_DEBUG_EXPORT LayerTreeExtraState final {
 public:
  LayerTreeExtraState();
  LayerTreeExtraState(const std::string& toast_message);
  LayerTreeExtraState(const LayerTreeExtraState& other);
  ~LayerTreeExtraState();

  static bool Equal(const LayerTreeExtraState& a, const LayerTreeExtraState& b);

  uint32_t ID() const;
  bool ShowToast() const;
  absl::optional<std::string> ToastMessage() const;

 private:
  uint32_t id_ = 0;
  absl::optional<std::string> toast_message_;
};

} // namespace
#endif // CC_DEBUG_LAYER_TREE_EXTRA_STATE_H_
