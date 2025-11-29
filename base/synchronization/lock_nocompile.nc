// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This is a "No Compile Test" suite.
// http://dev.chromium.org/developers/testing/no-compile-tests

#include "base/synchronization/lock.h"

namespace base {

class SynchronizedInt {
public:
  void ResetWithoutCheckingAutolock() {
    AutoTryLock maybe(lock);
    value = 0; // expected-error {{writing variable 'value' requires holding mutex 'lock' exclusively}}
  }

private:
  Lock lock;
  int value GUARDED_BY(lock) = 0;
};

struct StructWithLock {
  Lock lock;
};

void AutoLockAsTemporary(StructWithLock* s) {
#if BUILDFLAG(IS_OHOS)
  AutoLock(s->lock); // expected-error {{ignoring temporary created by a constructor declared with 'nodiscard' attribute}}
#else
  AutoLock(s->lock); // expected-error {{ignoring temporary of type 'BasicAutoLock<base::Lock>' declared with 'nodiscard' attribute}}
#endif
}

}  // namespace base
