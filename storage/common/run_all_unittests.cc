// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/functional/bind.h"
#include "base/test/launcher/unit_test_launcher.h"
#include "base/test/test_suite.h"
#include "build/build_config.h"
#include "mojo/core/embedder/embedder.h"

#if BUILDFLAG(IS_OHOS)
#include "base/test/test_support_ohos.h"
#endif
 
namespace {
class StorageUnittestSuite : public base::TestSuite {
 public:
  StorageUnittestSuite(int argc, char** argv) : TestSuite(argc, argv) {}
 
 protected:
  void Initialize() override {
    TestSuite::Initialize();
#if BUILDFLAG(IS_OHOS)
    base::RegisterPathProviderForOhosTest();
#endif
  }
};
 
}  // namespace

int main(int argc, char** argv) {
  int result = 0;
  {
    StorageUnittestSuite test_suite(argc, argv);
    mojo::core::Init();
    result = base::LaunchUnitTests(
        argc, argv,
        base::BindOnce(&base::TestSuite::Run, base::Unretained(&test_suite)));
  }
  return result;
}
