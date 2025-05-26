import { Browser } from './browser'
import { WebTab } from './titleBar'
import { CustomDialogExample } from './menu'

const ButtonWidth: number = 22
const ButtonRadius: number = 4
const downColor: string = "#e4e4e4"
const upColor: string = "#00000000"

@Component
export struct PhoneLayout {
  @Link browser:Browser;
  @State isPhone:boolean = true;
  @State hasDown:boolean = false;
  @State pageCount: string = "1"
  @State arrayIndex: number = 0;
  addr:string = ""

  private toolPoneArray: Array<Object> = [
    {
      imageSrc: $r('app.media.ic_public_back'),
      id: 1
    },
    {
      imageSrc: $r('app.media.ic_public_advance'),
      id: 2
    },
    {
      imageSrc: $r('app.media.ic_public_home'),
      id: 5
    },
    {
      imageSrc: $r('app.media.ic_public_refresh'),
      id: 3
    },
    {
      imageSrc: $r('app.media.ic_public_more_filled'),
      id: 6
    },
  ]

  dialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogExample({ browser: $browser, isPhone: $isPhone, cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true,
    alignment:DialogAlignment.Bottom
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


  @Builder ToolBar() {
    Column() {
      Progress({ value: this.browser.progress, total: 100 })
        .color('#0000ff')
        .visibility(this.browser.hideProgress ? Visibility.None : Visibility.Visible)
      Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceAround }) {
        ForEach(this.toolPoneArray, item => {
          Column() {
            Divider().color('#e9eaec')
            Button({ type: ButtonType.Normal }) {
              Column() {
                if (item.id != 4) {
                  Image(item.imageSrc)
                } else {
                  Column() {
                    Text(this.pageCount)
                      .fontSize(16)
                  }
                  .border({ width: 2 })
                  .width(22)
                  .height(22)
                  .borderRadius(5)
                  .justifyContent(FlexAlign.Center)
                }
              }
              .width(ButtonWidth)
              .height(ButtonWidth)
              .justifyContent(FlexAlign.Center)
            }
            .height('100%')
            .width('100%')
            .backgroundColor(this.arrayIndex == item.id ? downColor : upColor)
            .borderRadius(ButtonRadius)
            .flexShrink(0)
            .onTouch((event: TouchEvent) => {
              if (event.type == TouchType.Down) {
                this.arrayIndex = item.id
              }

              if (event.type == TouchType.Up) {
                this.arrayIndex = 0
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
                case 5:
                  this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({
                    url: $rawfile('phone.html')
                  })
                  break;
                case 6:
                  this.dialogController.open()
                  break;
              }
            })
          }
          .width('20%')
        })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#fdfdfd')
    }.height('100%')
  }

  build() {
    Navigation() {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
        Flex({ direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
          TextInput({ placeholder: '输入网址...', text: this.browser.inputValue })
            .placeholderFont({ size: 16, weight: 500 })
            .fontSize(16)
            .margin(2)
            .height(30)
            .onChange((value: string) => {
              console.info("onChange")
              this.addr = value
            })
            .onSubmit((enterKey: EnterKeyType) => {
              console.info("onSubmit")
              this.browser.webControllerArray[this.browser.tabArrayIndex].controller.loadUrl({
                url: "https://" + this.addr
              })
              this.addr = ""
            })
          Button({ type: ButtonType.Normal }) {
            Image($r('app.media.submit'))
          }
          .margin(8)
          .width(ButtonWidth)
          .height(ButtonWidth)
          .backgroundColor(this.hasDown ? downColor : upColor)
          .borderRadius(ButtonRadius)
          .flexShrink(0)
          .onTouch((event: TouchEvent) => {
            if (event.type == TouchType.Down) {
              this.hasDown = true
            }

            if (event.type == TouchType.Up) {
              this.hasDown = false
            }
          })
          .onClick((event: ClickEvent) => {
            this.browser.loadUrl(this.addr)
            this.addr = ""
          })
        }
        WebTab({ browser: $browser, isPhone: $isPhone })
      }

    }
    .hideTitleBar(true)
    .toolBar(this.ToolBar) //待手机验证
    .hideBackButton(true)


  }
}