// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_OHOS_TASK_SCHEDULER_TASK_TRAITS_OHOS_H_
#define BASE_OHOS_TASK_SCHEDULER_TASK_TRAITS_OHOS_H_

enum TaskTraits {
  UI_TRAITS_START = 0,
  UI_BEST_EFFORT = UI_TRAITS_START + 1,
  UI_USER_VISIBLE = UI_TRAITS_START + 2,
  UI_USER_BLOCKING = UI_TRAITS_START + 3,
  UI_DEFAULT = UI_TRAITS_START,
  UI_TRAITS_END = UI_USER_BLOCKING
};

#endif  // BASE_OHOS_TASK_SCHEDULER_TASK_TRAITS_OHOS_H_
