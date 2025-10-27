import { Browser } from './../browser'
import {UA} from './settings'

@CustomDialog
@Component
export struct UserAgentCustomize {
  @Link browser: Browser;
  @Link isPhone: boolean;
  private userAgentString: string = "";
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      Text('User Agent Customize').width('80%').fontSize(20).margin({ top: 20, bottom: 20 })
      TextInput({ placeholder: 'Please customize user agent' })
        .placeholderColor("rgb(0,0,225)")
        .placeholderFont({ size: 16, weight: 100, family: 'cursive', style: FontStyle.Italic })
        .height(50)
        .width('90%')
        .align(Alignment.Center)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontFamily("sans-serif")
        .fontStyle(FontStyle.Normal)
        .onChange((value: string) => {
          this.userAgentString = value
        })

      Flex({ justifyContent: FlexAlign.SpaceAround }) {
        Button('cancel').fontSize(20)
          .onClick(() => {
            this.controller.close()
            this.cancel()
          }).backgroundColor(0xffffff).fontColor(Color.Black)
        Button('confirm').fontSize(20)
          .onClick(() => {
            this.controller.close()
            this.confirm()
            if (this.userAgentString != "") {
              this.browser.userAgentValue = this.userAgentString
              AppStorage.SetOrCreate("userAgent",this.userAgentString);
              console.log("UserAgentCustomize userAgentValue:" + this.browser.userAgentValue )
            }
          }).backgroundColor(0xffffff).fontColor(Color.Red)
      }.margin({ bottom: 10, top: 10 })
    }
  }
}

@CustomDialog
@Component
export struct UserAgentChoose{
  @Link browser: Browser;
  @Link isPhone: boolean;
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  private arr : Array<Object> = [
    {id: 1, title: "手机", ua: UA.PHONE },
    {id: 2, title: "平板", ua: UA.PAD},
    {id: 3, title: "iphone", ua: UA.IPHONE},
    {id: 4, title: "ipad", ua: UA.IPAD}
  ]

  private arr1: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

  build() {
    Column() {
      List({ space: 20, initialIndex: 0 }) {
        ForEach(this.arr, (item) => {
          ListItem() {
            Text(item.title)
              .width('100%')
              .height(40)
              .fontSize(16)
              .textAlign(TextAlign.Center)
              .borderRadius(10)
            //              .backgroundColor(0xFFFFFF)
          }.editable(false)
          .stateStyles({
            pressed: {
              .backgroundColor(0x317aff) // 设置点击时的样式
              .margin({ top: 20 })
            }
          })
          .onClick((event: ClickEvent) => {
            this.browser.userAgentValue = item.ua
            AppStorage.SetOrCreate("userAgentType", item.title)
            AppStorage.SetOrCreate("userAgent",item.ua )
            console.log("UserAgentChoose.etc userAgentValue:" + this.browser.userAgentValue)
            this.controller.close()
          })
        },)
      }.divider({ strokeWidth: 1, color: "#d3d7d4" }) // 每行之间的分界线
    }
  }

}