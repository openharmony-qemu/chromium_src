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

#include "ui/ohos/deceleration_animator.h"

#include "base/threading/platform_thread.h"
#include "base/time/time.h"

namespace ui {
void DecelerationAnimator::startAnimate(float distance,
                                        base::TimeDelta duration) {
  if (task_runner_) {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&DecelerationAnimator::animate,
                                  base::Unretained(this), distance, duration));
  }
}

void DecelerationAnimator::animate(float distance, base::TimeDelta duration) {
  auto startTime = base::Time::Now();
  float lastY = 0;
  auto endTime = startTime + duration;

  while (base::Time::Now() < endTime) {
    float timeProgress = (base::Time::Now() - startTime).InMilliseconds() /
                         duration.InMillisecondsF();

    float relativeDistance = interpolator_.getInterpolation(timeProgress);
    float currY = relativeDistance * distance;
    float delta = currY - lastY;
    if (listener_) {
      listener_->onAnimationRepeat(delta);
    }

    lastY = currY;

    base::PlatformThread::Sleep(base::Milliseconds(kAnimateMilliseconds));
  }
  if (listener_) {
    listener_->onAnimationEnd();
  }
}

void DecelerationAnimator::resetAnimate() {
  listener_.reset();
  task_runner_.reset();
}

void DecelerationAnimator::setRefreshListener(
    std::unique_ptr<DecelerationAnimatorListener> listener) {
  listener_ = std::move(listener);
}

}  // namespace ui
