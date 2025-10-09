/**
 * Module Constant
 * 常量模块
 */

const Constant = {
    LIST_MIN: 1, // list至少要有多少个元素

    ATTR_NUM: 5, // 样式表选择器关联属性个数

    WIDE_SCREEN_RATE: 0.8, // 宽屏手机定义

    ZOOM_SCALE: 0.48,
    FIXED_DIALOG_ZOOM: 0.5,
    MIN_COLUMN_HEIGHT: 10,
    ERROR_VALUE: 4,
    MAX_WIDTH_DIFF: 3,

    MIN_MASK_OPACITY: '0.3',
};

export default Constant;

export const LayoutValue = {
    DISABLE: 'disable', // 用于记录不重排元素，不做任何操作
    LIST_ITEM: 'listItem', // 用于记录多行列表的所有的list节点，后续不对此元素进行任何操作
    VERTICAL_GRID: 'vertical_grid', // 用于记录宫格列表节点
    CROSS_GRID: 'cross_grid', // 用于记录宫格列表节点
    ASIDE_BAR: 'asideBar', // 用于记录纵向侧边栏
    LARGE_IMG: 'largeImg', // 用于记录与大图节点相关的节点，后续不对此元素进行任何操作
    HANDLE_IMG: 'handleImg', // 用于记录大图(virImg)节点，后续不对此元素进行任何操作
    BACK_IMG: 'backImg', // 用于记录大图(virImg)节点，后续不对此元素进行任何操作
    ONE_LINE: 'oneLine', // 用于记录单行节点，后续不对此元素进行任何操作
    ZOOM_PARENT: 'zoomParent', // 用于标记缩放过的节点的所有祖先节点，此节点禁止缩放，但可对其子孙元素中未缩放过的元素进行处理
    IS_IMG: 'isImg', // 用来记录该元素是否为图片
    HAS_TEXT_CHILD: 'hasTextChild', // 用来记录该元素是否为图片
    BIG_FIXED_DIALOG: 'bigFixedDialog', // 用于标记大型对话框
    ZOOM_SMALL_IMG: 'smallImg', // 用于标记小图
    WATERFALL: 'waterfall', // 用于标记瀑布流
    WATERFALL_ITEM: 'waterfallItem', // 用于标记瀑布流子元素
    SWIPER: 'swiper', // 标记swiper
    SWIPER_BEATUY: 'swiperBeatuy', // 标记swiper 插入节点，后续不处理
    SPECIAL_TEXT_LIST: 'specialTextList',
    SCROLL_LIST: 'scrollList', // 标记纵向列表
    SMALL_FIXED: 'smallFixed', // 小型悬浮图标
    COMMENT: 'comment', // 用于标记评论
    HEADER: 'header', // 用于标记顶栏
    SPECIAL_HEADER: 'special_header',
    HEADER_COVERED: 'header_covered', // 被顶栏覆盖的元素
    NAV: 'nav', // 用于标记导航栏
    ACROSS_COLUMN: 'across_column', // 用于标记通栏
    NEXT_ROUND: 'next_round', // 用于在reset尚未生效阶段暂时阻塞其他布局的处理
    ZOOM_HTML_FONTSIZE: 'zoomHTMLFontSize',
    BOTTOM: 'bottom',
    PARENT: 'parent',
    ROOT: 'root',
    BUTTON: 'button',
    EMPTY: 'empty', // 标记通栏处理的空元素
};

export const LayoutKey = {
    LAYOUT_TAG: 'hwLayout', // 用来记录元素是否被重排过
    CONFIG_LAYOUT_TAG: 'thirdLayout', // 用来记录元素是否被重排过
    HW_BUTTON: 'hw_button', // 用于标记按钮
    DIFF_ELE: 'diffEle', // 用于记录本次重排需要处理的元素
    INSERT_RULE: 'insertRule', // insert rule 的元素
    INSERT_SWIPER: 'insertSwiper', // insert rule 的元素
    INNER_IMG_COUNT: 'innerImgCount', // 内部图片数量
    PADDING_PERCENT_HEIGHT: 'paddingPercentHeight', // 元素是通过设置padding为百分比来模拟的固定高度
};

export const FontTag = {
    ZOOM_FONT: 'zoomFont', // 用于标记缩放的字体节点
};
export const HwTag = {
    MIN_HEIGHT_TAG: 'min_hw_height', // 用于标记调整过的min height
    CROSS_TAG: 'isCross', // 用于记录当前的节点是否属于通栏，取值为true or false
    TEXT_TAG: 'isText', // 用于记录当前的节点是否属于通栏，取值为true or false
    DISPLAY_TAG: 'display', // 用于记录当前元素是否由于高度为0被设置为display: none
    MARGINS: 'margins',
    UID: 'uid',
    IS_FIXED: 'isFixed',
    MULTI_IMGS: 'multi_items',
    ELE_NUM: 'ele_num',
    ORIGIN_WIDTH: 'origin_width',
    ITEM_IS_PERCENT_WIDTH: 'percent',
    IS_CENTER_LAYOUT: 'is_center_layout',
    CLONE_DOM: 'clone_dom',
    HEIGHT_0: 'height0',
    FORCE_CSS: 'force_css',
    WEBKIT_BOX: 'webkit_box',
    LEFT_CHILD: 'left_child',
    RIGHT_CHILD: 'right_child',
    LEFT_GAP: 'left_gap',
    RIGHT_GAP: 'right_gap',
    GRID_MAX_CHILD: 'grid_max_child',
    WIDTH_UNDER_FLEX: 'column_flex',
    ORIGIN_HEIGHT: 'origin_height',
};
