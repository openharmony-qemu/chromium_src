import mediaquery from '@ohos.mediaquery'
import deviceInfo from '@ohos.deviceInfo';
import { TabletTitle, BrowserTabs, WebTab } from './titleBar'
import { PhoneLayout } from './PhoneLayout'
import { Browser, LoadingStatus } from './browser'
import { UA } from './settingsPage/settings'

@Entry
@Component
export struct Index {
  @State isPhone: boolean = false;
  @State browser: Browser = new Browser()
  private isInit:Boolean = false
  listener = mediaquery.matchMediaSync('(orientation: landscape)')

  build() {
    Flex({ direction: FlexDirection.Column }) {
      if (!this.isPhone) {
        TabletTitle({ browser: $browser, isPhone: $isPhone })
        Progress({ value: this.browser.progress, total: 100 })
          .color('#0000ff')
          .visibility(this.browser.hideProgress ? Visibility.None : Visibility.Visible)
        BrowserTabs({ browser: $browser })
        Divider().color('#c6c6c6').width('100%').flexShrink(0)
        WebTab({ browser: $browser, isPhone: $isPhone})
      } else {
        PhoneLayout({ browser: $browser })
      }
    }
  }

  onPageShow() {
    this.setUserAgentByAppStorage()
  }

  setUserAgentByAppStorage() {
    if (!AppStorage.Has('userAgent')) {
      if(!this.isPhone) {
        AppStorage.SetOrCreate("userAgent", UA.PAD)
      } else {
        AppStorage.SetOrCreate("userAgent", UA.PHONE)
      }
    }

    if (!AppStorage.Has('userAgentType')) {
      AppStorage.SetOrCreate("userAgentType", "默认")
    }

    try {
      let appStorageUserAgent: string = AppStorage.Get<string>('userAgent');
      if (appStorageUserAgent != '') {
        this.browser.setUserAgentValue(appStorageUserAgent)
      }
    } catch(e) {
      console.log("unexpected error: " + e);
    }
  }

  aboutToAppear() {
    try {
      let deviceType = deviceInfo.deviceType;
      if (deviceType == 'phone') {
        this.isPhone = true
      }
    } catch(e) {
      console.log("getSync unexpected error: " + e);
    }
//    this.listener.on('change', this.orientationCallback.bind(this))
  }

  orientationCallback(result) {
    if (!this.isInit) {
      if (result.matches) {
        this.isPhone = false
      } else {
        this.isPhone = true
      }
      this.isInit = true
    }
    console.info('orientationCallback ' + this.isPhone)
  }

  onBackPress(): boolean {
    console.info('back press')
    if (this.browser.webControllerArray[this.browser.tabArrayIndex].controller.accessBackward() ||
      this.browser.loadingStatus == LoadingStatus.LOADING) {
      this.browser.webControllerArray[this.browser.tabArrayIndex].controller.backward()
      return true
    }
    return false
  }

  onDeviceChange() {
    this.browser = new Browser()
  }
}
