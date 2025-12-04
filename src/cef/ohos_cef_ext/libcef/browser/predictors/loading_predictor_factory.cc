// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor_factory.h"

#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "ohos_cef_ext/libcef/browser/predictors/loading_predictor.h"
#include "content/public/browser/browser_context.h"
#include <memory>

namespace ohos_predictors {

// static
LoadingPredictor* LoadingPredictorFactory::GetForBrowserContext(
    content::BrowserContext* context) {
  return static_cast<LoadingPredictor*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

// static
LoadingPredictorFactory* LoadingPredictorFactory::GetInstance() {
  return base::Singleton<LoadingPredictorFactory>::get();
}

LoadingPredictorFactory::LoadingPredictorFactory()
    : BrowserContextKeyedServiceFactory(
          "LoadingPredictor",
          BrowserContextDependencyManager::GetInstance()) {}

LoadingPredictorFactory::~LoadingPredictorFactory() {}

std::unique_ptr<KeyedService> LoadingPredictorFactory::BuildServiceInstanceForBrowserContext(
    content::BrowserContext* context) const {
  if (!IsLoadingPredictorEnabled(context)) {
    return nullptr;
  }

  return std::make_unique<LoadingPredictor>(LoadingPredictorConfig(), context);
}

}  // namespace ohos_predictors
