import router from '@system.router'
import { Browser } from './browser'

@CustomDialog
@Component
export struct CustomDialogExample {
  @Link browser: Browser;
  @Link isPhone: boolean;

  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  private toolArray : Array<Object> = [
    { title: '添加至', img: $r('app.media.ic_public_add'), id: 1 },
    { title: "书签/历史", img: $r('app.media.ic_public_icon_history'), id: 2 },
    { title: "下载", img: $r('app.media.ic_downloads'), id: 3 },
    { title: "分享", img: $r('app.media.ic_public_home'), id: 4 },
    { title: "刷新", img: $r('app.media.ic_public_refresh'), id: 5 },
    { title: "工具箱", img: $r('app.media.icon_extensions'), id: 6},
    { title: "无痕浏览", img: $r('app.media.ic_public_incognito_icon'), id: 7},
    { title: "夜间模式", img: $r('app.media.ic_public_home'), id: 8},
    { title: "设置", img: $r('app.media.ic_public_settings'), id: 9},
    { title: "退出", img: $r('app.media.icon_clear'), id: 10}
  ]

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_public_account_circle')).width(48).height(48).margin({left:10})
        Text('登录').width('80%').fontSize(20).margin({ top: 20, bottom: 20 ,left: 10})
      }
      Grid() {
        ForEach(this.toolArray, item => {
            GridItem() {
              Column() {
                Image(item.img).objectFit(ImageFit.Contain).width(24).height(24)
                Text(item.title)
                  .fontSize(12)
                  .margin({top:3})
                  .textAlign(TextAlign.Center)
              }.alignItems(HorizontalAlign.Center)
            }.width(60)
             .height(50)
             .onClick((event: ClickEvent) =>{
               console.log("menu.etc userAgentString:" + JSON.stringify(this.browser))
               if (item.id == 9) {
                 router.push({
                   uri: "pages/settings",
                   params: {
                     browser: this.browser,
                     isPhone: this.isPhone,
                   }
                 })
                 this.controller.close()
               }
             })
          },)
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .width('90%')
      .height(130)
    }.width('100%').margin({ top: 5 })
  }
}


