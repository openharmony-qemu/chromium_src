// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { inject, injectable } from 'inversify';
import dragController from "@ohos.arkui.dragController"
import image from '@ohos.multimedia.image';

import { AbilityManager } from '../common/AbilityManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';
import { OhosDragParamToJs, OhosDropData } from '../interface/CommonInterface';
import { DragParamManager } from '../common/DragParamManager';
import { MODULE_TYPE } from '../common/ModuleType';
import { window } from '@kit.ArkUI';

const TAG: string = 'OhosDrag';

@injectable()
export default class DragDropAdapter extends BaseAdapter {
  private pixmap: image.PixelMap | undefined = undefined

  private abilityManager: AbilityManager;
  private dragSourceWindowId: string = "";
  private dragParamManager: DragParamManager;

  constructor(
    @inject(DragParamManager) manager: DragParamManager,
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext,
    @inject(AbilityManager) abilityManager: AbilityManager,
  ) {
    super(ctx);
    this.dragParamManager = manager;
    this.abilityManager = abilityManager;
  }

  @LogMethod
  async startDrag(dragParams: OhosDragParamToJs) {
    this.dragSourceWindowId = dragParams.windowId;

    let touchX: Dimension = ((dragParams.touchX / dragParams.width) * 100 + '%') as Dimension;
    let touchY: Dimension = ((dragParams.touchY / dragParams.height) * 100 + '%') as Dimension;

    let dragInfo: dragController.DragInfo = {
      pointerId: 0,
      data: this.dragParamManager.handleDragRecords(dragParams),
      extraParams: '',
      previewOptions: {numberBadge: false},
      touchPoint: {
        x: touchX,
        y: touchY
      }
    }

    let opts: image.InitializationOptions = { editable: false, pixelFormat: image.PixelMapFormat.BGRA_8888,
                                              size: { height: dragParams.height, width: dragParams.width } }
    try {
      this.pixmap = await image.createPixelMap(dragParams.pixelBuffer, opts);
    } catch (error) {
      LogUtil.error(TAG, `pixelmap create fail :${JSON.stringify(error)}`);
      return;
    }
    let dragItemInfo: DragItemInfo = {
      pixelMap: this.pixmap,
      builder: ()=>{},
      extraInfo: "ohosDrag"
    }
    this.executeDragAction(dragItemInfo, dragInfo);
  }

  private executeDragAction(dragItemInfo: DragItemInfo, dragInfo: dragController.DragInfo) {
    let uiContext: UIContext | undefined = this.dragParamManager.getUiContext(this.dragSourceWindowId);
    if (!uiContext) {
      this.clearDragParam();
      LogUtil.error(TAG, `executeDrag fail : uiContext not exist`);
      return;
    }
    try {
      let customBuilders: Array<CustomBuilder | DragItemInfo> = new Array<CustomBuilder | DragItemInfo>();
      customBuilders.push(dragItemInfo);
      let dragAction: dragController.DragAction = uiContext.getDragController().createDragAction(customBuilders, dragInfo);
      if (!dragAction) {
        this.clearDragParam();
        LogUtil.error(TAG, `dragAction create fail`);
        return;
      }
      dragAction.on('statusChange', this.dragActionStatusListener(dragAction));
      dragAction.startDrag().catch((err: Error) => {
        this.clearDragParam();
        LogUtil.error(TAG, `dragAction startDrag fail :${JSON.stringify(err)}`);
      })
    } catch(err) {
      this.clearDragParam();
      LogUtil.error(TAG, `dragAction fail :${JSON.stringify(err)}`);
    }
  }

  dragEnterData(xComponentId: string, summary: Summary) {
    let dropData: OhosDropData = this.dragParamManager.handleDropDataForEnter(summary);
    this.nativeContext.OnDragEnterCB(xComponentId, dropData, dropData.filePaths);
  }

  dragMove(xComponentId: string, windowX: number, windowY: number): void {
    let windowClass: window.Window | undefined = this.abilityManager.getMainWindow(xComponentId);
    try {
      const mainWindowProp: window.WindowProperties | undefined = windowClass?.getWindowProperties();
      let xComBorder: number = mainWindowProp?.drawableRect.left || 0;
      let xComTop: number = mainWindowProp?.drawableRect.top || 0;
      windowX -= xComBorder;
      windowY -= xComTop;
      this.nativeContext.OnDragMoveCB(xComponentId, windowX, windowY);
    } catch (exception) {
      LogUtil.error(TAG, `Failed to obtain the window properties, Cause code: ${exception.code}, message: ${exception.message}`);
    }
  }

  dragLeave(xComponentId: string) {
    this.nativeContext.OnDragLeaveCB(xComponentId);
  }

  dropData(xComponentId: string, dragData: UnifiedData) {
    let dropData: OhosDropData = this.dragParamManager.handleDropDataForDrop(dragData);
    this.nativeContext.OnDropCB(xComponentId, dropData, dropData.filePaths);
  }

  private dragActionStatusListener(dragAction: dragController.DragAction) {
    return (dragAndDropInfo: dragController.DragAndDropInfo) => {
      if (dragAndDropInfo.status === dragController.DragStatus.ENDED) {
        if (dragAction) {
          dragAction.off('statusChange');
        }
        this.nativeContext.OnDragEndCB(this.dragSourceWindowId);
        this.clearDragParam();
      }
    };
  }

  private clearDragParam() {
    if (this.pixmap) {
      this.pixmap.release().catch((err: Error) => {
        LogUtil.error(TAG, `pixmap release failed:${JSON.stringify(err)}`);
      })
    }
    this.dragSourceWindowId = "";
    this.dragParamManager.clearParam();
  }
}
