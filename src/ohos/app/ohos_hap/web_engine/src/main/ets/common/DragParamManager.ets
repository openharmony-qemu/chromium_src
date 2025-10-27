// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import { OhosDragParamToJs, OhosDropData } from '../interface/CommonInterface';
import UDC from '@ohos.data.unifiedDataChannel';
import UTD from '@ohos.data.uniformTypeDescriptor';
import LogUtil from '../utils/LogUtil';
import unifiedDataChannel from '@ohos.data.unifiedDataChannel';
import { fileUri } from '@kit.CoreFileKit';
import fs from '@ohos.file.fs';
import StringUtil from '../utils/StringUtil';

interface IDragParams {
  id: string,
  uiContext: UIContext | undefined
};

AppStorage.setOrCreate('dragParamList', new Map<string,IDragParams>());

const BOOKMARK_DEFINED_TYPE: string = "ApplicationDefinedType-bookmark";
const WEB_CUSTOM_DEFINED_TYPE: string = "ApplicationDefinedType-webCustom";
const APPLICATION_DEFINE_TYPE: string = "ApplicationDefinedType";
const FILE_DRAG_DEFAULT_NAME = "/storage/User/currentUser/file.png";
const HYPERLINK_DRAG_DEFAULT_URL = "https://www.exampleholder.com";
const HYPERLINK_DRAG_DEFAULT_TITLE = "ExampleHolder";
const TAG: string = 'OhosDrag';

@injectable()
export class DragParamManager {
  constructor() {
    let appStorageLink: SubscribedAbstractProperty<Map<string,IDragParams>> = AppStorage.link('dragParamList');
    this.appStorage = appStorageLink.get();
  }
  private appStorage: Map<string,IDragParams>;
  private dragData: UDC.UnifiedData | undefined = undefined

  private paramNormalization(windowId: string | number): string {
    if (typeof windowId === 'number') {
      return 'browser' + windowId;
    }
    return windowId;
  }

  public addDragContextData(data: IDragParams) {
    if (data && data.id) {
      this.appStorage.set(data.id, data);
    }
  }

  public removeDragContextData(windowId: string | number) {
    let id = this.paramNormalization(windowId);
    this.appStorage.delete(id);
  }

  public setUiContext(windowId: string | number, uiContext: UIContext): void {
    let id = this.paramNormalization(windowId);
    let current = this.appStorage.get(id);
    if (current) {
      current.uiContext = uiContext;
      this.appStorage.set(id, current);
    }
  }

  public getUiContext(windowId: string | number): UIContext | undefined {
    let id = this.paramNormalization(windowId);
    let current = this.appStorage.get(id);
    if (current) {
      return current.uiContext;
    }
    return undefined;
  }

  public handleDragRecords(dragParams: OhosDragParamToJs): UDC.UnifiedData | undefined {
    this.dragData = undefined;
    let dragContents: Array<UDC.UnifiedRecord> = [];
    let dragDataHtml: UDC.HTML|undefined = undefined;
    let dragDataText: UDC.PlainText|undefined = undefined;
    let dragDataHyperlink: UDC.Hyperlink|undefined = undefined;
    if (dragParams.basicData.html) {
      dragDataHtml = new UDC.HTML();
      dragDataHtml.htmlContent = dragParams.basicData.html;
      dragContents.push(dragDataHtml);
    }
    if (dragParams.basicData.text) {
      dragDataText = new UDC.PlainText();
      dragDataText.textContent = dragParams.basicData.text;
      dragContents.push(dragDataText);
      if (dragDataHtml !== undefined) {
        dragDataHtml.plainContent = dragParams.basicData.text;
      } else {
        dragDataHtml = new UDC.HTML();
        dragDataHtml.plainContent = dragParams.basicData.text;
        dragContents.push(dragDataHtml);
      }
    }
    if (dragParams.basicData.urlTitle) {
      dragDataHyperlink = new UDC.Hyperlink();
      dragDataHyperlink.description = dragParams.basicData.urlTitle;
      dragContents.push(dragDataHyperlink);
    }
    if (dragParams.basicData.url) {
      if (dragDataHyperlink !== undefined) {
        dragDataHyperlink.url = dragParams.basicData.url;
      } else {
        dragDataHyperlink = new UDC.Hyperlink();
        dragDataHyperlink.url = dragParams.basicData.url;
        dragContents.push(dragDataHyperlink);
      }
    }
    if (dragParams.imageTempUri) {
      let image: UDC.Image | undefined = this.handleImageDragUri(dragParams.imageTempUri);
      if (image) {
        dragContents.push(image);
      }
    }
    if (dragParams.bookmarkBuffer.byteLength > 0) {
      let record = new unifiedDataChannel.ApplicationDefinedRecord();
      record.applicationDefinedType = BOOKMARK_DEFINED_TYPE;
      record.rawData = new Uint8Array(dragParams.bookmarkBuffer);
      dragContents.push(record);
    }
    if (dragParams.webCustomBuffer.byteLength > 0) {
      let record = new unifiedDataChannel.ApplicationDefinedRecord();
      record.applicationDefinedType = WEB_CUSTOM_DEFINED_TYPE;
      record.rawData = new Uint8Array(dragParams.webCustomBuffer);
      dragContents.push(record);
    }
    if (dragContents.length === 0) {
      return this.dragData;
    }
    this.dragData = new UDC.UnifiedData(dragContents[0]);
    for (let i = 1; i < dragContents.length; i++) {
      this.dragData.addRecord(dragContents[i]);
    }
    return this.dragData;
  }

  private handleImageDragUri(imageUri: string) : UDC.Image | undefined {
    if (!imageUri) {
      LogUtil.error(TAG, `get drag image info fail, imageUri is empty`);
      return;
    }
    try {
      let res: boolean = fs.accessSync(imageUri);
      if (!res) {
        LogUtil.error(TAG, `get drag image info failed, image not exists`);
        return;
      }
      let uri: string = fileUri.getUriFromPath(imageUri);
      let image: UDC.Image = new UDC.Image();
      image.imageUri = uri;
      return image;
    } catch(err) {
      LogUtil.error(TAG, `get drag image info failed with error message: ${err.message},
                    error code: ${err.code}`);
    }
    return;
  }

  public handleDropDataForEnter(summary: Summary) : OhosDropData {
    let dropData: OhosDropData = {
      basicData: {
        text: "",
        url: "",
        urlTitle: "",
        html: "",
        htmlBaseUrl: ""
      },
      filePaths: new Array,
      bookmarkBuffer: new ArrayBuffer(0),
      webCustomBuffer: new ArrayBuffer(0)
    };
    if (this.dragData?.getRecords() && this.dragData?.getRecords().length > 0) {
      this.handleDropDataFromRecords(this.dragData?.getRecords(), dropData);
      return dropData;
    }
    let records: Array<string> = JSON.parse(JSON.stringify(summary.summary));
    if (!records || records.length === 0) {
      LogUtil.warn(TAG, `On handleDropDataForEnter summary records is null`);
      return dropData;
    }
    for(let i = 0; i < records.length; i++) {
      switch (records[i][0]) {
        case UTD.UniformDataType.HYPERLINK:
          // drag from other apps,there is no actual data. use temporary data
          dropData.basicData.url = HYPERLINK_DRAG_DEFAULT_URL;
          dropData.basicData.urlTitle = HYPERLINK_DRAG_DEFAULT_TITLE;
          break;
        case UTD.UniformDataType.FILE:
        case UTD.UniformDataType.IMAGE:
        case UTD.UniformDataType.VIDEO:
        case UTD.UniformDataType.AUDIO:
        case UTD.UniformDataType.FOLDER:
          // on drag enter event,there is no actual data. use temporary data
          dropData.filePaths.push(FILE_DRAG_DEFAULT_NAME);
          break;
        default:
          LogUtil.warn(TAG, `On handleDropDataForEnter dragData, type: ${records[i][0]} not support`);
          break;
      }
    }
    return dropData;
  }

  public handleDropDataForDrop(dragData: UnifiedData) : OhosDropData {
    let dropData: OhosDropData = {
      basicData: {
        text: "",
        url: "",
        urlTitle: "",
        html: "",
        htmlBaseUrl: ""
      },
      filePaths: new Array,
      bookmarkBuffer: new ArrayBuffer(0),
      webCustomBuffer: new ArrayBuffer(0)
    };
    if (!dragData) {
      LogUtil.warn(TAG, `On handleDropDataForDrop dragData is undefined`);
      return dropData;
    }
    let records: Array<UDC.UnifiedRecord> = dragData.getRecords();
    if (!records) {
      LogUtil.warn(TAG, `On handleDropDataForDrop dragData records is undefined`);
      return dropData;
    }
    this.handleDropDataFromRecords(records, dropData);
    return dropData;
  }

  private handleDropDataFromRecords(records: Array<UDC.UnifiedRecord>, dropData: OhosDropData) {
    records.forEach((item) => {
      switch (item.getType()) {
        case UTD.UniformDataType.PLAIN_TEXT:
          let plainText = item as unifiedDataChannel.PlainText;
          dropData.basicData.text = plainText.textContent;
          break;
        case UTD.UniformDataType.HYPERLINK:
          let hyperlink = item as unifiedDataChannel.Hyperlink;
          dropData.basicData.url = hyperlink.url;
          dropData.basicData.urlTitle = hyperlink.description ? hyperlink.description : "";
          break;
        case UTD.UniformDataType.HTML:
          let html = item as unifiedDataChannel.HTML;
          dropData.basicData.html = html.htmlContent;
          dropData.basicData.htmlBaseUrl = html.plainContent ? html.plainContent : "";
          break;
        case APPLICATION_DEFINE_TYPE:
          let applicationDefinedItem = this.getRecord(APPLICATION_DEFINE_TYPE) as UDC.ApplicationDefinedRecord;
          if (!applicationDefinedItem) {
            LogUtil.error(TAG, 'handleDropDataFromRecords,APPLICATION_DEFINE_TYPE:item undefined');
            break;
          }
          if (applicationDefinedItem.rawData.byteLength > 0) {
            if (applicationDefinedItem.applicationDefinedType === BOOKMARK_DEFINED_TYPE) {
              dropData.bookmarkBuffer = applicationDefinedItem.rawData;
            } else if (applicationDefinedItem.applicationDefinedType === WEB_CUSTOM_DEFINED_TYPE) {
              dropData.webCustomBuffer = applicationDefinedItem.rawData;
            } else {
              LogUtil.warn(TAG, `On handleDropDataFromRecords dragData,APPLICATION_DEFINE_TYPE: ${item.getType()} not support`);
            }
          }
          break;
        case UTD.UniformDataType.FILE:
        case UTD.UniformDataType.IMAGE:
        case UTD.UniformDataType.VIDEO:
        case UTD.UniformDataType.AUDIO:
        case UTD.UniformDataType.FOLDER:
          // web page images do not require push filePath
          let webImage = this.getRecord(UTD.UniformDataType.IMAGE) as UDC.Image;
          if (!webImage) {
            let file = item as unifiedDataChannel.File;
            dropData.filePaths.push(StringUtil.filterFileDocs(file.uri));
          }
          break;
        default:
          LogUtil.warn(TAG, `On handleDropDataFromRecords dragData, type: ${item.getType()} not support`);
          break;
      }
    })
  }

  private getRecord(recordType: string) : UDC.UnifiedRecord | undefined {
    if (!this.dragData) {
      return;
    }
    let records = this.dragData.getRecords();
    for (let i = 0; i < records.length; i++) {
      if (records[i].getType() === recordType) {
        return records[i];
      }
    }
    return;
  }

  public clearParam() : void {
    this.dragData = undefined;
  }
};
