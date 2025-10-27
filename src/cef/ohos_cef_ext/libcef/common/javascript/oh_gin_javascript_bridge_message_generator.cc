// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define IPC_MESSAGE_IMPL
#include "libcef/common/javascript/oh_gin_javascript_bridge_message_generator.h"

// Generate constructors.
#include "ipc/struct_constructor_macros.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_message_generator.h"

// Generate param traits write methods.
#include "ipc/param_traits_write_macros.h"
namespace IPC {
#include "libcef/common/javascript/oh_gin_javascript_bridge_message_generator.h"
}  // namespace IPC

// Generate param traits read methods.
#include "ipc/param_traits_read_macros.h"
namespace IPC {
#include "libcef/common/javascript/oh_gin_javascript_bridge_message_generator.h"
}  // namespace IPC

// Generate param traits log methods.
#include "ipc/param_traits_log_macros.h"
namespace IPC {
#include "libcef/common/javascript/oh_gin_javascript_bridge_message_generator.h"
}  // namespace IPC
