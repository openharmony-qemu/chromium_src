import {WebSettingsExtension, WebExtension} from 'libbrowser_service.so'
import { UA } from './settingsPage/settings'

export class WebObject {
  controller:WebController;
  isRegistered:boolean;

  constructor(controller:WebController, isRegistered:boolean) {
    this.controller = controller
    this.isRegistered = isRegistered
  }
}

@Observed
class WebKey {
  key:number;
  timestamp:number;

  constructor(key:number, timestamp:number) {
    this.key = key
    this.timestamp = timestamp
  }
}

export enum LoadingStatus {
  LOADING,
  END
}

export class Browser {
  userAgentValue: string = UA.PAD
  inputValue:string = ""
  tabArrayIndex:number = 0
  progress:number = 0;
  hideProgress:boolean = true;
  loadingStatus:LoadingStatus = LoadingStatus.END
  webArray: Array<WebKey> = [new WebKey(0, new Date().getTime())]
  tabsController: TabsController = new TabsController()
  webControllerArray: Array<WebObject> = [new WebObject(new WebController(), false)]

  deleteTab(index:number) {
    for (let i = 0; i < this.webArray.length; ++i) {
      console.info('deleteTab key ' + this.webArray[i].key + ' time ' + this.webArray[i].timestamp)
    }
    console.info('delete before tab index ' + index + ' controller length ' + this.webControllerArray.length + ' tabArrayIndex ' + this.tabArrayIndex)
    this.webArray.splice(index, 1);
    this.webControllerArray.splice(index, 1)
    if (this.tabArrayIndex > index || this.tabArrayIndex == this.webArray.length) {
      this.tabArrayIndex -= 1;
    }

    for (let i = index; i < this.webArray.length; ++i) {
      this.webArray[i].key -= 1
    }
    for (let i = 0; i < this.webArray.length; ++i) {
      console.info('key ' + this.webArray[i].key + ' time ' + this.webArray[i].timestamp)
    }
    console.info('delete after tab index ' + index + ' controller length ' + this.webControllerArray.length + ' tabArrayIndex ' + this.tabArrayIndex)
    this.tabsController.changeIndex(this.tabArrayIndex)
  }

  getWebArray() {
    return this.webArray;
  }

  addTab() {
    let webController: WebController = new WebController();
    let object = new WebObject(webController, false)
    this.webControllerArray.push(object)
    this.webArray.push(new WebKey(this.webArray.length, new Date().getTime()))
    this.tabArrayIndex = this.webArray.length - 1
    console.info('add tab 123 index ' + this.tabArrayIndex)
    this.tabsController.changeIndex(this.tabArrayIndex)
  }

  setTabArrayIndex(tabArrayIndex:number) {
    this.tabArrayIndex = tabArrayIndex
  }

  getTabArrayIndex():number {
    return this.tabArrayIndex
  }

  setInputVal(inputValue:string) {
    this.inputValue = inputValue;
  }

  getInputVal() {
    return this.inputValue
  }

  setUserAgentValue(userAgentValue:string) {
    if (globalThis.browserServiceInitialized) {
      for (let i = 0; i < this.webControllerArray.length; i++) {
        let webSettingsExt = new WebSettingsExtension(this.webControllerArray[i].controller)
        webSettingsExt.setBrowserUserAgentString(userAgentValue)
      }
    }
    this.userAgentValue = userAgentValue;
  }

  getUserAgentValue() {
    return this.userAgentValue
  }


  loadUrl(addr:string) {
    addr = "https://" + addr;
    this.webControllerArray[this.tabArrayIndex].controller.loadUrl({url:addr})
  }

  Back() {
    if (this.webControllerArray[this.tabArrayIndex].controller.accessBackward()) {
      this.webControllerArray[this.tabArrayIndex].controller.backward()
    }
  }

  Forward() {
    if (this.webControllerArray[this.tabArrayIndex].controller.accessForward()) {
      this.webControllerArray[this.tabArrayIndex].controller.forward()
    }
  }

  Refresh() {
    // this.webControllerArray[this.tabArrayIndex].controller.refresh()
    let controller = this.webControllerArray[this.tabArrayIndex].controller
    if (globalThis.browserServiceInitialized) {
      let webExt = new WebExtension(controller)
      webExt.reloadOriginalUrl()
    } else {
      controller.refresh()
    }
  }

}