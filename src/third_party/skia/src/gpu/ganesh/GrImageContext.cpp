/*
 * Copyright 2019 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "include/private/gpu/ganesh/GrImageContext.h"

#include "arkweb/build/features/features.h"
#include "include/core/SkRefCnt.h"
#include "include/gpu/ganesh/GrContextThreadSafeProxy.h"
#include "src/gpu/ganesh/GrContextThreadSafeProxyPriv.h"
#if BUILDFLAG(ARKWEB_NPTR_PROTECTION)
#include "src/gpu/graphite/Log.h"
#endif
#include <utility>

GrImageContext::GrImageContext(sk_sp<GrContextThreadSafeProxy> proxy)
            : GrContext_Base(std::move(proxy)) {
}

GrImageContext::~GrImageContext() {}

void GrImageContext::abandonContext() {
    fThreadSafeProxy->priv().abandonContext();
}

bool GrImageContext::abandoned() {
#if BUILDFLAG(ARKWEB_NPTR_PROTECTION)
    if (fThreadSafeProxy == nullptr) {
        SKGPU_LOG_E("fGrImageContext ThreadSafeProxy is null");
        return true;
    }
#endif
    return fThreadSafeProxy->priv().abandoned();
}

sk_sp<GrImageContext> GrImageContext::MakeForPromiseImage(sk_sp<GrContextThreadSafeProxy> tsp) {
    return sk_sp<GrImageContext>(new GrImageContext(std::move(tsp)));
}
