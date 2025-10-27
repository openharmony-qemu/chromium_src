/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cef/ohos_cef_ext/libcef/browser/adblock/adblock_config_bridge.h"

#include <stdio.h>

#include <fstream>

#include "base/files/file_util.h"
#include "base/functional/bind.h"
#include "base/task/thread_pool.h"
#include "base/values.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/registry_controlled_domains/registry_controlled_domain.h"
#include "third_party/blink/public/mojom/loader/resource_load_info.mojom-shared.h"
#include "third_party/jsoncpp/source/include/json/json.h"
#include "third_party/jsoncpp/source/include/json/reader.h"
#include "third_party/jsoncpp/source/include/json/value.h"

namespace ohos_adblock {

namespace {

base::LazyInstance<AdblockConfigBridge>::Leaky g_lazy_instance;

}  // namespace

AdblockConfigBridge::AdblockConfigBridge() = default;

AdblockConfigBridge::~AdblockConfigBridge() = default;

AdblockConfigBridge* AdblockConfigBridge::GetInstance() {
  return g_lazy_instance.Pointer();
}

}  // namespace ohos_adblock
