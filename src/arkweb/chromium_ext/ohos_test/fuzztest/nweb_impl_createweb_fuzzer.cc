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

#include <fuzzer/FuzzedDataProvider.h>

#include <string>

#include "nweb_impl.h"
using namespace OHOS::NWeb;

namespace {

class NWebOutputFrameCallbackFuzzer : public NWebOutputFrameCallback {
 public:
  NWebOutputFrameCallbackFuzzer() = default;
  ~NWebOutputFrameCallbackFuzzer() = default;
  bool Handle(const char* buffer, uint32_t width, uint32_t height) override {
    return true;
  }
};

class NWebEngineInitArgsFuzzer : public NWebEngineInitArgs {
 public:
  std::string GetDumpPath() override { return "/DumpPath"; }

  bool GetIsFrameInfoDump() override { return true; }

  std::list<std::string> GetArgsToAdd() override { return {"ArgsToAdd"}; }

  std::list<std::string> GetArgsToDelete() override { return {"ArgsToDelete"}; }

  bool GetIsMultiRendererProcess() override { return true; }

  bool GetIsEnhanceSurface() override { return false; }

  bool GetIsPopup() override { return false; }

  std::string GetSharedRenderProcessToken() override { return "token"; }
};

class NWebCreateInfoFuzzer : public NWebCreateInfo {
 public:
  NWebCreateInfoFuzzer() = default;
  ~NWebCreateInfoFuzzer() = default;
  uint32_t GetWidth() override { return 1; }

  uint32_t GetHeight() override { return 1; }

  std::shared_ptr<NWebOutputFrameCallback> GetOutputFrameCallback() override {
    return std::make_shared<NWebOutputFrameCallbackFuzzer>();
  }

  std::shared_ptr<NWebEngineInitArgs> GetEngineInitArgs() override {
    return std::make_shared<NWebEngineInitArgsFuzzer>();
  }

  void* GetProducerSurface() override { return nullptr; }

  void* GetEnhanceSurfaceInfo() override { return nullptr; }

  bool GetIsIncognitoMode() override { return false; }
};

void FuzzTest(const uint8_t* data, size_t size) {
  FuzzedDataProvider data_provider(data, size);
  uint32_t id = data_provider.ConsumeIntegral<uint32_t>();
  NWebImpl nwebImpl(id);
  // test oncontinue and onpause when the pointer is null
  nwebImpl.OnContinue();
  nwebImpl.OnPause();
  // test process mode set and get
  RenderProcessMode mode =
      static_cast<RenderProcessMode>(data_provider.ConsumeIntegral<int>());
  nwebImpl.SetRenderProcessMode(mode);
  RenderProcessMode get_mode = nwebImpl.GetRenderProcessMode();
  nwebImpl.SetRenderProcessMode(get_mode);
  std::shared_ptr<NWebCreateInfo> create_info =
      std::make_shared<NWebCreateInfoFuzzer>();
  std::shared_ptr<NWeb> created_nweb = nwebImpl.CreateNWeb(create_info);
  int32_t nweb_id = data_provider.ConsumeIntegral<int32_t>();
  std::shared_ptr<NWeb> get_nweb = nwebImpl.GetNWeb(nweb_id);
  // try to make the pointer non null and call oncontinue and onpause
  nwebImpl.Init(create_info);
  // test other lifecycle functions
  nwebImpl.OnContinue();
  nwebImpl.OnPause();
  nwebImpl.OnWebviewShow();
  nwebImpl.OnWebviewHide();
  nwebImpl.OnDestroy();
}
}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FuzzTest(data, size);
  return 0;
}
 