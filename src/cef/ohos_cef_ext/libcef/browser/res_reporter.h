// Copyright 2024 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RES_REPORTER_H_
#define CEF_LIBCEF_RES_REPORTER_H_

#include <vector>

#include "base/memory/raw_ptr.h"

using InitFunc = void (*)();
using ResAddRtg = void (*)(std::vector<int>);
using ResFetchBegin = void (*)();
using ResFetchEnd = void (*)();

class ResReporter {
 public:
  static ResReporter& GetInstance();
  void Init();
  void AddRtg(std::vector<int> tids);
  void FetchBegin();
  void FetchEnd();

 private:
  ResReporter();
  ~ResReporter();
  bool LoadLibrary();
  void CloseLibrary();
  void* LoadSymbol(const char* symName);
  raw_ptr<void> resSchedHandle_ = nullptr;
  bool resSchedSoLoaded_ = false;

  InitFunc initFunc_ = nullptr;
  ResAddRtg resAddRtgFunc_ = nullptr;
  ResFetchBegin resFetchBeginFunc_ = nullptr;
  ResFetchEnd resFetchEndFunc_ = nullptr;
};

#endif  // CEF_LIBCEF_RES_REPORTER_H_
