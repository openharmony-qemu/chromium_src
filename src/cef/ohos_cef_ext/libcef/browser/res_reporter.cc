// Copyright 2024 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "res_reporter.h"

#include <dlfcn.h>

#include "base/logging.h"

#if (defined(__aarch64__) || defined(__x86_64__))
const std::string FRAME_AWARE_SO_PATH = "/system/lib64/libframe_ui_intf.z.so";
#else
const std::string FRAME_AWARE_SO_PATH = "/system/lib/libframe_ui_intf.z.so";
#endif

ResReporter& ResReporter::GetInstance() {
  static ResReporter instance;
  return instance;
}

ResReporter::ResReporter() {
  Init();
}

ResReporter::~ResReporter() {
  CloseLibrary();
}

NO_SANITIZE("cfi-icall") void ResReporter::Init() {
  int ret = LoadLibrary();
  if (!ret) {
    LOG(DEBUG) << "ResReporter:[Init] dlopen libframe_ui_intf.so failed!";
    return;
  }
  LOG(INFO) << "ResReporter::[Init] dlopen libframe_ui_intf.so success!";
  initFunc_ = (InitFunc)LoadSymbol("Init");
  if (initFunc_ != nullptr) {
    initFunc_();
  }
}

bool ResReporter::LoadLibrary() {
  if (!resSchedSoLoaded_) {
    resSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
    if (resSchedHandle_ == nullptr) {
      LOG(DEBUG)
          << "ResReporter:[LoadLibrary]dlopen libframe_ui_intf.so failed!";
      return false;
    }
    resSchedSoLoaded_ = true;
  }
  LOG(INFO) << "ResReporter:[LoadLibrary] load library success!";
  return true;
}

void ResReporter::CloseLibrary() {
  if (dlclose(resSchedHandle_) != 0) {
    LOG(DEBUG)
        << "ResReporter:[CloseLibrary]dlclose libframe_ui_intf.so failed!";
    return;
  }
  resSchedHandle_ = nullptr;
  resSchedSoLoaded_ = false;
  LOG(INFO) << "ResReporter:[CloseLibrary]libframe_ui_intf.so close success";
}

void* ResReporter::LoadSymbol(const char* symName) {
  if (!resSchedSoLoaded_) {
    LOG(DEBUG) << "ResReporter:[LoadSymbol]libframe_ui_intf.so not loaded.";
    return nullptr;
  }

  void* funcSym = dlsym(resSchedHandle_, symName);
  if (funcSym == nullptr) {
    LOG(DEBUG) << "ResReporter:[LoadSymbol]Get symbol failed: " << symName;
    return nullptr;
  }
  return funcSym;
}

void ResReporter::AddRtg(std::vector<int> tids) {
  if (!resSchedSoLoaded_) {
    return;
  }
  if (resAddRtgFunc_ == nullptr) {
    resAddRtgFunc_ = (ResAddRtg)LoadSymbol("WebviewAddRtg");
  }
  if (resAddRtgFunc_ != nullptr) {
    return resAddRtgFunc_(tids);
  } else {
    LOG(DEBUG) << "ResReporter:[AddRtg]load WebviewAddRtg function failed!";
    return;
  }
}

void ResReporter::FetchBegin() {
  if (!resSchedSoLoaded_) {
    return;
  }
  if (resFetchBeginFunc_ == nullptr) {
    resFetchBeginFunc_ = (ResFetchBegin)LoadSymbol("WebResFetchBegin");
  }
  if (resFetchBeginFunc_ != nullptr) {
    return resFetchBeginFunc_();
  } else {
    LOG(DEBUG)
        << "ResReporter:[FetchBegin]load WebResFetchBegin function failed!";
    return;
  }
}

void ResReporter::FetchEnd() {
  if (!resSchedSoLoaded_) {
    return;
  }
  if (resFetchEndFunc_ == nullptr) {
    resFetchEndFunc_ = (ResFetchEnd)LoadSymbol("WebResFetchEnd");
  }
  if (resFetchEndFunc_ != nullptr) {
    return resFetchEndFunc_();
  } else {
    LOG(DEBUG) << "ResReporter:[FetchEnd]load WebResFetchEnd function failed!";
    return;
  }
}
 