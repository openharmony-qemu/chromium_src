import { Browser, LoadingStatus, WebObject } from './browser'
import { CustomDialogExample } from './menu'
import { BrowserServiceInitializer, WebAppClientExtension } from 'libbrowser_service.so'

const ButtonWidth: number = 22
const ButtonRadius: number = 4
const downColor: string = "#e4e4e4"
const upColor: string = "#00000000"

@Component
export struct TabletTitle {
  @Link browser: Browser;
  @Link isPhone: boolean;
  @State inputValue: string = "";
  @State arrayIndex: number = 0;
  @State array1Index: number = 0;
  addr: string = ""
  private buttonArray1: Array<Object> = [
    {
      imageSrc: $r('app.media.ic_public_back'),
      id: 1
    },
    {
      imageSrc: $r('app.media.ic_public_advance'),
      id: 2
    },
    {
      imageSrc: $r('app.media.ic_public_refresh'),
      id: 3
    },
    {
      imageSrc: $r('app.media.ic_public_home'),
      id: 4
    },
  ]
  private buttonArray2: Array<Object> = [
    {
      imageSrc: $r('app.media.submit'),
      id: 5
    },
    {
      imageSrc: $r('app.media.ic_public_add'),
      id: 6
    },
    {
      imageSrc: $r('app.media.ic_public_more_filled'),
      id: 7
    },
  ]
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({
      browser: $browser,
      isPhone: $isPhone,
      cancel: this.onCancel,
      confirm: this.onAccept
    }),
    cancel: this.existApp,
    autoCancel: true,
    alignment: DialogAlignment.Bottom
  })

  onCancel() {
    console.info('Callback when the first button is clicked')
  }

  onAccept() {
    console.info('Callback when the second button is clicked')
  }

  existApp() {
    console.info('Click the callback in the blank area')
  }

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
      ForEach(this.buttonArray1, item => {
        Button({ type: ButtonType.Normal }) {
          Image(item.imageSrc)
        }
        .margin(8)
        .width(ButtonWidth)
        .height(ButtonWidth)
        .backgroundColor(this.array1Index == item.id ? downColor : upColor)
        .borderRadius(ButtonRadius)
        .flexShrink(0)
        .onTouch((event: TouchEvent) => {
          if (event.type == TouchType.Down) {
            this.array1Index = item.id
          }

          if (event.type == TouchType.Up) {
            this.array1Index = 0
          }
        })
        .onClick((event: ClickEvent) => {
          switch (item.id) {
            case 1:
              this.browser.Back()
              break;
            case 2:
              this.browser.Forward()
              break;
            case 3:
              this.browser.Refresh()
              break;
            case 4:
              this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({
                url: $rawfile('pad.html')
              })
              break;
          }
        })
      }, item => item.id)

      TextInput({ placeholder: '输入网址...', text: this.browser.inputValue })
        .placeholderFont({ size: 18, weight: 500 })
        .fontSize(18)
        .margin(2)
        .onChange((value: string) => {
          this.addr = value
        })
        .onSubmit((enterKey: EnterKeyType) => {
          console.info("onSubmit")
          if (this.addr.length != 0) {
            this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({
              url: "https://" + this.addr
            })
            this.addr = ""
          }
        })

      ForEach(this.buttonArray2, item => {
        Button({ type: ButtonType.Normal }) {
          Image(item.imageSrc)
        }
        .margin(8)
        .width(ButtonWidth)
        .height(ButtonWidth)
        .backgroundColor(this.array1Index == item.id ? downColor : upColor)
        .borderRadius(ButtonRadius)
        .flexShrink(0)
        .onTouch((event: TouchEvent) => {
          if (event.type == TouchType.Down) {
            this.array1Index = item.id
          }

          if (event.type == TouchType.Up) {
            this.array1Index = 0
          }
        })
        .onClick((event: ClickEvent) => {
          if (this.array1Index == 5) {
            this.browser.loadUrl(this.addr)
            this.addr = ""
          }

          if (this.array1Index == 6) {
            console.info('add tab')
            this.browser.addTab()
          }

          if (this.array1Index == 7) {
            console.info('dialogController.open')
            this.dialogController.open()
          }
        })
      }, item => item.id)
    }
    .width('100%')
    .padding({ right: 5 })
  }
}


@Component
export struct BrowserTabs {
  @Link browser: Browser;
  @State tabButtonIndex: number = -1;

  build() {
    Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Start }) {
      ForEach(this.browser.webArray, item => {
        Row() {
          Row() {
            Text('主页')
          }.width('100%').flexShrink(1).justifyContent(FlexAlign.Center).height('100%')

          Button({ type: ButtonType.Normal }) {
            Image($r('app.media.ic_public_cancel_filled')).width(12).height(12)
          }
          .backgroundColor(this.tabButtonIndex == item.key ? downColor : '#00000000')
          .margin({ right: 2 })
          .width(33)
          .height(33)
          .borderRadius(5)
          .onTouch((event: TouchEvent) => {
            if (event.type == TouchType.Down) {
              this.tabButtonIndex = item.key
            }
            console.info('jyj button index ' + this.tabButtonIndex + ' key ' + item.key)

            if (event.type == TouchType.Up) {
              this.tabButtonIndex = -1
            }
          })
          .onClick((event: ClickEvent) => {
            if (this.browser.webArray.length > 1) {
              this.browser.deleteTab(this.tabButtonIndex)
            }
          })
        }.width(180).height('100%').backgroundColor(this.browser.tabArrayIndex == item.key ? '#ffffff' : '#ececec')
        .onClick((event: ClickEvent) => {
          console.info('onClick ' + item.key)
          this.browser.setTabArrayIndex(item.key)
          this.browser.tabsController.changeIndex(item.key)
        })

        Divider().vertical(true).color('#c6c6c6')
      }, item => item.timestamp.toString())
    }.height(33).flexShrink(0).backgroundColor('#ececec')
  }
}


@Component
export struct WebTab {
  @Link browser: Browser;
  @Link isPhone: boolean;
  isRegistered: boolean = false
  testObj = {
    test: (addr) => {
      console.info('tabArrayIndex ' + this.browser.tabArrayIndex)
      this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({ url: "https://" + addr })
    },
    searchWord: (word: string) => {
      console.info('search word ' + word)
      let code = globalThis.encodeURI(word)
      this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({
        url: "https://www.bing.com/search?q=" + code
      })
    }
  }

  build() {
    Tabs({ barPosition: BarPosition.Start, controller: this.browser.tabsController }) {
      ForEach(this.browser.webArray, item => {
        TabContent() {
          Web({
            src: this.isPhone ? $rawfile('phone.html') : $rawfile('pad.html'),
            controller: this.browser.webControllerArray[item.key].controller
          })
            .javaScriptAccess(true)
            .fileAccess(true)
            .userAgent(this.browser.getUserAgentValue())
            .onPageBegin((event) => {
              if (!globalThis.browserServiceInitialized) {
                BrowserServiceInitializer.initBrowserService(result => {
                  globalThis.browserServiceInitialized = result
                  console.info("[BrowserShell] Initialize BrowserService done, result: " + globalThis.browserServiceInitialized)
                })
              }
              if (globalThis.browserServiceInitialized) {
                let webAppClientExt = new WebAppClientExtension(this.browser.webControllerArray[item.key].controller)
                webAppClientExt.onLoadStarted(toDiffDoc => {
                  console.info('[BrowserShell] onLoadStarted toDifferentDocument: ' + toDiffDoc)
                })
              }

              console.log("WebTab browser:" + this.browser + ", isPhone:" + this.isPhone + ", userAgentString:" + this.browser.getUserAgentValue())
              console.info('onPageBegin ' + JSON.stringify(event))
              this.browser.loadingStatus = LoadingStatus.LOADING
            })
            .onPageEnd((event) => {
              console.info('onPageEnd ' + JSON.stringify(event))
              this.browser.webControllerArray[item.key].controller.runJavaScript({ script: "adapterDevice()" })
              this.browser.loadingStatus = LoadingStatus.END
              this.registerFunc(this.browser.webControllerArray[item.key])
              if (event.url.startsWith("http")) {
                this.browser.inputValue = event.url
              } else {
                this.browser.inputValue = ""
              }
            })
            .onProgressChange((event) => {
              console.info('onProgressChange')
              this.browser.progress = event.newProgress
              if (this.browser.progress == 100) {
                this.browser.hideProgress = true
              } else {
                this.browser.hideProgress = false
              }
            })
        }
      }, item => item.timestamp.toString())
    }
    .barHeight(0)
    .scrollable(false)
  }

  registerFunc(webObject: WebObject) {
    if (!webObject.isRegistered) {
      console.info('registerFunc')
      webObject.controller.registerJavaScriptProxy({
        object: this.testObj, name: "etsObj", methodList: ["test", "searchWord"]
      })
      webObject.isRegistered = true
      webObject.controller.refresh()
    }
  }
}
