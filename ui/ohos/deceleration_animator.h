/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UI_OHOS_DECELERATION_ANIMATOR_H_
#define UI_OHOS_DECELERATION_ANIMATOR_H_

#include <cmath>
#include <memory>

#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"

namespace base {
class TimeDelta;
}

namespace {
const float kOne = 1.f;
const float kFactorMultiplier = 2;
const float kAnimateMilliseconds = 10;
}

namespace ui {
class DecelerateInterpolator {
 public:
  DecelerateInterpolator(float factor) : factor_(factor) {}
  float getInterpolation(float input) {
    return kOne - std::pow(kOne - input, kFactorMultiplier * factor_);
  }

 private:
  float factor_ = 1.f;
};

class DecelerationAnimatorListener {
 public:
  virtual void onAnimationEnd() = 0;
  virtual void onAnimationRepeat(float) = 0;
  virtual ~DecelerationAnimatorListener() {}
};

class DecelerationAnimator : public base::RefCountedThreadSafe<DecelerationAnimator> {
 public:
  DecelerationAnimator() : interpolator_(2.f), listener_(nullptr) {}
  DecelerationAnimator(float decelerate_factor,
                       std::unique_ptr<DecelerationAnimatorListener> listener)
      : interpolator_(decelerate_factor), listener_(std::move(listener)) {}
  ~DecelerationAnimator() = default;

  void startAnimate(float distance, base::TimeDelta duration);
  void resetAnimate();

  void setRefreshListener(
      std::unique_ptr<DecelerationAnimatorListener> listener);

 private:
  void animate(float distance, base::TimeDelta duration);

  DecelerateInterpolator interpolator_;
  std::unique_ptr<DecelerationAnimatorListener> listener_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_ =
      base::ThreadPool::CreateSequencedTaskRunner(
          {base::MayBlock(), base::TaskPriority::BEST_EFFORT,
           base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN});
};
}  // namespace ui

#endif  // UI_OHOS_DECELERATION_ANIMATOR_H_
