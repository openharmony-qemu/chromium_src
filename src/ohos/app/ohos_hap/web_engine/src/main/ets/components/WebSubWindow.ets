// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { IParams } from '../interface/CommonInterface';
import { PanAction } from '../common/Constants';
import DragDropAdapter from '../adapter/DragDropAdapter';
import Inject from '../common/InjectModule';
import LogUtil from '../utils/LogUtil';
import MultiInputAdapter from '../adapter/MultiInputAdapter';

@Component
export struct WebSubWindow {
  private static TAG: string = 'WebSubWindow';
  @LocalStorageLink('subWindowParams') params: IParams = {
    id: '',
    size: [0, 0], // [width, height]
    callback: (ready: boolean, id: string) => {
    },
    initColorRgb: '#ffffff',
  };
  private xComponentId: string = this.params.id;
  private LIBRARY_NAME: string = 'adapter';
  private callback: (ready: boolean, id: string) => void = this.params.callback;
  private XCOMPONENT_NAME: string = 'subXcomponent';
  private dragDropAdapter: DragDropAdapter = Inject.get(DragDropAdapter);
  private multiInputAdapter: MultiInputAdapter = Inject.get(MultiInputAdapter);


  build() {
    XComponent({
      id: this.xComponentId,
      type: XComponentType.SURFACE,
      libraryname: this.LIBRARY_NAME
    })
      .backgroundColor(this.params.initColorRgb)
      .focusable(true)
      .defaultFocus(true)
      .gesture(
        PanGesture({ direction: PanDirection.Vertical | PanDirection.Horizontal })
          .onActionStart((event: GestureEvent) => {
            this.multiInputAdapter.OnPanEvent(PanAction.kStart, this.xComponentId, event);
          })
          .onActionUpdate((event: GestureEvent) => {
            this.multiInputAdapter.OnPanEvent(PanAction.kUpdate, this.xComponentId, event);
          })
          .onActionEnd((event: GestureEvent) => {
            this.multiInputAdapter.OnPanEvent(PanAction.kEnd, this.xComponentId, event);
          })
          .onActionCancel(() => {
            this.multiInputAdapter.OnPanEvent(PanAction.kCancel, this.xComponentId, undefined);
          })
      )
      .onDragEnter((event?: DragEvent) => {
        this.dragDropAdapter.dragEnterData(this.xComponentId, (event as DragEvent).getSummary());
      })
      .onDragMove((event?: DragEvent) => {
        let windowX = event?.getWindowX();
        let windowY = event?.getWindowY();
        windowX = vp2px(windowX);
        windowY = vp2px(windowY);
        this.dragDropAdapter.dragMove(this.xComponentId, windowX, windowY);
      })
      .onDragLeave(() => {
        this.dragDropAdapter.dragLeave(this.xComponentId);
      })
      .onDrop((event?: DragEvent) => {
        if (!event) {
          LogUtil.error(WebSubWindow.TAG, 'OhosDrag WebSubWindow DragDrop onDrop event is null');
          return;
        }
        let dragData: UnifiedData = (event as DragEvent).getData() as UnifiedData;
        this.dragDropAdapter.dropData(this.xComponentId, dragData);
      })
      .onLoad(() => {
        LogUtil.info(WebSubWindow.TAG, 'onLoad xComponentId:' + this.xComponentId);
        this.callback(true, this.xComponentId);
      })
      .id(this.XCOMPONENT_NAME)
  }
}
