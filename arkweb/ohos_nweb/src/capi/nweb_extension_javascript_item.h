/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_WEB_EXTENSION_JAVASCRIPT_ITEM_H_
#define OHOS_NWEB_SRC_WEB_EXTENSION_JAVASCRIPT_ITEM_H_

#include <optional>
#include <cstdint>
#include <string>
#include <vector>

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/public/capi/nweb_basic_types.h"
#endif

struct FrameInfos {
  // GlobalRenderFrameHostId.child_id + "_" + GlobalRenderFrameHostId.frame_routing_id
  std::string id;
  std::string parentId;
 
	void clear() {
    id.clear();
    parentId.clear();
  }
};

struct IsolatedWorld {
  std::string name;
 
  void clear() {
    name.clear();
  }
};

struct GetFrameInfosParam {
  std::vector<FrameInfos> frameinfoList;
};

struct RunJavaScriptParam {
  std::string script;
  std::optional<FrameInfos> rootFrame = std::nullopt;
  std::optional<IsolatedWorld> world = std::nullopt;
  bool recursive;
  int32_t callbackId;
};

enum class JavaScriptDataType {
  NONE = 0,
  BOOL,
  INT,
  DOUBLE,
  STRING,
  BINARY,
  DICTIONARY,
  LIST,
};

struct JavaScriptItemDictionary {
  std::optional<std::string> errName = std::nullopt;
  std::optional<std::string> errMsg = std::nullopt;
};

struct JavaScriptItemList {
  JavaScriptDataType type;
  std::vector<std::string> stringArr;
  std::vector<int32_t> intArr;
  std::vector<double> doubleArr;
  std::vector<bool> boolArr;
};

struct JavaScriptValue {
  JavaScriptDataType type;
  std::optional<std::string> stringValue = std::nullopt;
  std::optional<int32_t> intValue = std::nullopt;
  std::optional<double> doubleValue = std::nullopt;
  std::optional<bool> boolValue = std::nullopt;
  std::vector<uint8_t> binaryData;
  JavaScriptItemDictionary dictionaryValue;
  JavaScriptItemList listValue;
};

#if BUILDFLAG(ARKWEB_NWEB_EX)
typedef void (*OnReceiveValueCallback)(int32_t nwebId, int32_t callbackId,
                                       const ArkWebPbBuffer* pb_result_buffer);
typedef void (*OnReceiveFrameInfosCallback)(int32_t nwebId,
                                            const ArkWebPbBuffer* pb_result_buffer);

typedef void (*OnLastJavaScriptProxyCallingFrameInfoCallback)(int32_t nwebId,
                                                              const ArkWebPbBuffer* pb_result_buffer);
#endif

#endif //OHOS_NWEB_SRC_WEB_EXTENSION_JAVASCRIPT_ITEM_H_