import router from '@system.router'
import { Browser } from './browser'

@Entry
@Component
struct Settings {
  @State isPhone: boolean = <boolean> router.getParams().isPhone;
  @State browser: Browser = <Browser> router.getParams().browser;
  
  build() {
      Flex({ direction: FlexDirection.Column }) {
        Column() {
          Row() {
            Image($r('app.media.ic_public_back')).width(24).height(24)
            Text('首页').fontSize(22).width('95%').height(75).margin({left:10})
          }
          .onClick(() => {
            router.back({
              uri: "pages/frontPage",
            })
          })
          Row() {
            Image($r('app.media.ic_public_settings')).width(24).height(24).margin({ left: 0 })
            Text('User Agent').width('90%').fontSize(20).margin({ top: 20, bottom: 20, left: 10 })
          }
          .onClick((event: ClickEvent) => {
            router.push({
              uri: "pages/settingsPage/userAgent",
              params: {
                browser: this.browser,
                isPhone: this.isPhone,
              }
            })
          })
        }.width("100%")
        .height("100%")
      }
  }
}