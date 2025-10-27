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

bool NWebCookieManagerImplOthers(const uint8_t* data, size_t size) {
  FuzzedDataProvider dataProvider(data, size);
  std::shared_ptr<NiceMock<MockNWebBoolValueCallback>> nweb_value_callback =
      std::make_shared<NiceMock<MockNWebBoolValueCallback>>();
  bool boolFuzz = dataProvider.ConsumeBool();
  cookieMgr->IsAcceptCookieAllowed();
  cookieMgr->PutAcceptCookieEnabled(boolFuzz);
  cookieMgr->IsThirdPartyCookieAllowed();
  cookieMgr->PutAcceptThirdPartyCookieEnabled(boolFuzz);
  cookieMgr->IsFileURLSchemeCookiesAllowed();
  cookieMgr->PutAcceptFileURLSchemeCookiesEnabled(boolFuzz);
  cookieMgr->ExistCookies(nweb_value_callback);
  cookieMgr->Store();
  cookieMgr->Store(nweb_value_callback);

  return true;
}
}  // namespace OHOS::NWeb

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  /* Run your code on data */
  static bool init = OHOS::NWeb::LLVMFuzzerTestSetup();
  if (init) {
    OHOS::NWeb::NWebCookieManagerImplOthers(data, size);
  }
  return 0;
}
