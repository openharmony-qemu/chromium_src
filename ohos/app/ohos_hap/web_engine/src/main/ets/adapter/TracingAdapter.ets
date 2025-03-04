// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import Base, { BusinessError } from '@ohos.base';
import CommonEventManager from '@ohos.commonEventManager';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

@injectable()
export default class TracingAdapter extends BaseAdapter {
  private static TAG: string = 'TracingAdapter';
  private static EVENT_TRACE_START: string = 'chromium_tracing_start';
  private static EVENT_TRACE_STOP: string = 'chromium_tracing_stop';
  private static FLAG_TRACE_BEGIN: number = 0;
  private static FLAG_TRACE_END: number = 1;

  private subscriber: CommonEventManager.CommonEventSubscriber | undefined = undefined;
  private subscribeInfo: CommonEventManager.CommonEventSubscribeInfo = {
    events: [
      TracingAdapter.EVENT_TRACE_START,
      TracingAdapter.EVENT_TRACE_STOP
    ]
  };

  @LogMethod
  registerTracingEventCallback(callback: Function): void {
    try {
      CommonEventManager.createSubscriber(this.subscribeInfo,
        (err: BusinessError, scriber: CommonEventManager.CommonEventSubscriber) => {
          if (err) {
            LogUtil.error(TracingAdapter.TAG, 'create subscriber failed, error: ' + JSON.stringify(err));
            return;
          }
          this.subscriber = scriber;
          CommonEventManager.subscribe(this.subscriber, (err: Base.BusinessError, data: CommonEventManager.CommonEventData) => {
            if (err) {
              LogUtil.error(TracingAdapter.TAG, 'subscribe callback failed, error: ' + JSON.stringify(err));
              return;
            }
            if (!callback) {
              LogUtil.error(TracingAdapter.TAG, 'subscribe callback null');
              return;
            }

            let attachedData: string = data.data?? "";
            if (data.event === TracingAdapter.EVENT_TRACE_START) {
              callback(TracingAdapter.FLAG_TRACE_BEGIN, attachedData);
            } else if (data.event === TracingAdapter.EVENT_TRACE_STOP) {
              callback(TracingAdapter.FLAG_TRACE_END, attachedData);
            }
          });
        });
    } catch (err) {
      LogUtil.error(TracingAdapter.TAG, 'createSubscriber failed, code is %{public}d, error: ' + JSON.stringify(err));
    }
  }

  @LogMethod
  unregisterTracingEventCallback(): void {
    try {
      CommonEventManager.unsubscribe(this.subscriber, (err:Base.BusinessError) => {
        if (err) {
          LogUtil.error(TracingAdapter.TAG, 'unsubscribe failed, error: ' + JSON.stringify(err));
        }
      });
    } catch (err) {
      LogUtil.error(TracingAdapter.TAG, 'unsubscribe failed, error: ' + JSON.stringify(err));
    }
  }
};
