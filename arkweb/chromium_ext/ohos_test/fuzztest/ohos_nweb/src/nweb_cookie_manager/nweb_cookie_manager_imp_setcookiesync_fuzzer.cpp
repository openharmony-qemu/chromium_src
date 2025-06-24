/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include <fuzzer/FuzzedDataProvider.h>

#include <string>

#include "nweb_cookie_manager_imp_mocks.h"
#include "nweb_engine_impl.h"
#include "ohos_nweb/src/nweb_cookie_manager_impl.h"


using ::testing::NiceMock;

namespace OHOS::NWeb {

std::shared_ptr<NWebCookieManager> cookieMgr = nullptr;

bool LLVMFuzzerTestSetup() {
  std::shared_ptr<NWebDefaultEngineInitArgsImpl> init_args =
      std::make_shared<NWebDefaultEngineInitArgsImpl>();
  NWebEngineImpl::GetInstance()->InitializeWebEngine(init_args);
  cookieMgr = NWebEngineImpl::GetInstance()->GetCookieManager();
  return true;
}

bool NWebCookieManagerImplSetCookieSync(const uint8_t* data, size_t size) {
  FuzzedDataProvider dataProvider(data, size);
  std::string url = dataProvider.ConsumeRandomLengthString();
  std::string value = dataProvider.ConsumeRandomLengthString();
  bool incognito_mode = dataProvider.ConsumeBool();
  bool includeHttpOnly = dataProvider.ConsumeBool();
  cookieMgr->SetCookieSync(url, value, incognito_mode, includeHttpOnly);
  return true;
}
}  // namespace OHOS::NWeb

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  /* Run your code on data */
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::NWebCookieManagerImplSetCookieSync(data, size);
  }
  return 0;
}
