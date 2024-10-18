// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/layer_tree_extra_state.h"

namespace cc {

namespace {
uint32_t GenerateUID() {
  static uint32_t sUid = 0;
  return ++sUid;
}
}

LayerTreeExtraState::LayerTreeExtraState() = default;
LayerTreeExtraState::LayerTreeExtraState(const std::string& toast_message)
    : id_(GenerateUID())
    , toast_message_(toast_message) {}
LayerTreeExtraState::LayerTreeExtraState(const LayerTreeExtraState& other) = default;
LayerTreeExtraState::~LayerTreeExtraState() = default;


// static
bool LayerTreeExtraState::Equal(const LayerTreeExtraState& a,
                                const LayerTreeExtraState& b) {
  return a.id_ == b.id_ &&
         a.toast_message_ == b.toast_message_;
}

uint32_t LayerTreeExtraState::ID() const {
  return id_;
}

bool LayerTreeExtraState::ShowToast() const {
  return !!toast_message_;
}

absl::optional<std::string> LayerTreeExtraState::ToastMessage() const {
  return toast_message_;
}
} // namespace
