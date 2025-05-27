import router from '@system.router'
import { UserAgentCustomize, UserAgentChoose } from './userAgentCustomize'
import { Browser } from './../browser'

@Entry
@Component
struct UserAgent {
  @State isPhone: boolean = <boolean> router.getParams().isPhone;
  @State browser: Browser = <Browser> router.getParams().browser;
  @StorageLink('userAgent')  currentUserAgent: string = AppStorage.Get<string>("userAgent")
  @StorageLink('userAgentType') userAgentType: string = AppStorage.Get<string>("userAgentType")

  userAgentCustomizeController: CustomDialogController = new CustomDialogController({
    builder: UserAgentCustomize({ browser: $browser, isPhone: $isPhone, cancel: this.onCancel, confirm: this.onAccept }),
    cancel: this.existApp,
    autoCancel: true,
    alignment:DialogAlignment.Center
  })

  userAgentController: CustomDialogController = new CustomDialogController({
    builder: UserAgentChoose({browser: $browser, isPhone: $isPhone }),
    cancel: this.existApp,
    autoCancel: true,
    alignment:DialogAlignment.Center
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
      Flex({ direction: FlexDirection.Column }) {
        Column() {
          Row() {
            Image($r('app.media.ic_public_back')).width(24).height(24)
            Text('设置').fontSize(22).width('95%').height(75).margin({left:10})
          }
          .onClick((event: ClickEvent) => {
            router.back({
              uri: "pages/settings",
            })
          })     
          Column() {
            Text('User Agent').width('90%').fontSize(20).margin({ top: 20, bottom: 5 })
            Text(this.userAgentType).width('90%').fontSize(15).fontColor("#77787b")
          }.width('100%').height(85)
          .onClick((event: ClickEvent) => {
             this.userAgentController.open()
          })

          Divider().vertical(false).strokeWidth(1).color("#d3d7d4").lineCap(LineCapStyle.Round)
          Column() {
            Text('User Agent Customize').width('90%').fontSize(20).margin({ top: 20, bottom: 5 })
            Text(this.currentUserAgent).width('90%').fontSize(15).fontColor("#77787b")
          }.width('100%').height(85)
           .onClick((event: ClickEvent) => {
             this.userAgentCustomizeController.open()
          })

          Divider().vertical(false).strokeWidth(1).color("#d3d7d4").lineCap(LineCapStyle.Round)
        }.width("100%")
        .height("100%")
    }
  }
}
