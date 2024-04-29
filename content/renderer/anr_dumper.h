// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "base/synchronization/lock.h"
#include "content/common/child_process.mojom.h"

namespace content {

class AnrDumper

{
 public:
  static AnrDumper* GetInstance();
  class InterruptData;
  AnrDumper() = default;
  void DumpCurrentJavaScriptStack(
      mojom::ChildProcess::dumpCurrentJavaScriptStackInMainThreadCallback
          callback);
};
}  // namespace content
