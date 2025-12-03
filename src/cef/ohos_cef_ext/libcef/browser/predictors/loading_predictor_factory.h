// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_FACTORY_H_
#define CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_FACTORY_H_

#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include <memory>

namespace ohos_predictors {

class LoadingPredictor;

class LoadingPredictorFactory : public BrowserContextKeyedServiceFactory {
 public:
  static LoadingPredictor* GetForBrowserContext(
      content::BrowserContext* context);
  static LoadingPredictorFactory* GetInstance();

  LoadingPredictorFactory(const LoadingPredictorFactory&) = delete;
  LoadingPredictorFactory& operator=(const LoadingPredictorFactory&) = delete;

 private:
  friend struct base::DefaultSingletonTraits<LoadingPredictorFactory>;

  LoadingPredictorFactory();
  ~LoadingPredictorFactory() override;

  // BrowserContextKeyedServiceFactory implementation
  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;

};

}  // namespace ohos_predictors

#endif  // CEF_LIBCEF_BROWSER_PREDICTORS_LOADING_PREDICTOR_FACTORY_H_
