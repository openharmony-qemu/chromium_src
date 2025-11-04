export default class Constant {
    static readonly minMargin = 5;
    static readonly ATTR_NUM = 5; // 样式表选择器关联属性个数
    
    // Layout
    static readonly display = 'display';
    static readonly width = 'width';
    static readonly minHeight = 'min-height';
    static readonly transform = 'transform';
    static readonly transition = 'transition';
    static readonly transform_origin = 'transform-origin';
    static readonly translate = 'translate';
    static readonly perspective = 'perspective';
    static readonly flex = 'flex';
    static readonly position = 'position';
    static readonly top = 'top';
    static readonly left = 'left';
    static readonly bottom = 'bottom';
    static readonly float = 'float';
    static readonly vertical_align = 'vertical-align';
    static readonly justify_content = 'justify-content';
    static readonly margin_top = 'margin-top';
    static readonly margin_left = 'margin-left';
    static readonly margin_right = 'margin-right';
    static readonly flex_direction = 'flex-direction';
    static readonly align_items = 'align-items';
    static readonly margin_bottom = 'margin-bottom';
    static readonly padding_top = 'padding-top';
    static readonly padding_bottom = 'padding-bottom';
    static readonly padding_right = 'padding-right';
    static readonly padding_left = 'padding-left';
    static readonly right = 'right';
    static readonly overflow = 'overflow';
    static readonly overflow_x = 'overflow-x';
    static readonly overflow_y = 'overflow-y';
    static readonly zoom = 'zoom';
    static readonly z_index = 'z-index';
    static readonly line_height = 'line-height';
    static readonly computedWidth = 'computedWidth';
    static readonly box_sizing = 'box-sizing';
    static readonly background_size = 'background-size';
    static readonly border_bottom_width = 'border-bottom-width';
    static readonly border_top_width = 'border-top-width';
    static readonly border_left_width = 'border-left-width';
    static readonly border_right_width = 'border-right-width';
    static readonly border_left = 'border-left';
    static readonly border_right = 'border-right';
    static readonly border_top = 'border-top';
    static readonly white_space = 'white-space';
    static readonly text_align = 'text-align';
    static readonly max_height = 'max-height';
    static readonly flex_grow = 'flex-grow';
    static readonly row = 'row';
    static readonly flex_shrink = 'flex-shrink';
    static readonly filter = 'filter';
    static readonly flex_wrap = 'flex-wrap';
    static readonly content = 'content';
    static readonly name_ = 'name';
    static readonly meta = 'meta';
    static readonly viewport = 'viewport';
    static readonly resize = 'resize';
    static readonly load = 'load';
    static readonly childList = 'childList';
    static readonly border_radius = 'border-radius';
    static readonly border = 'border';
    static readonly border_width = 'border-width';
    static readonly max_width = 'max-width';
    static readonly transition_duration = 'transition-duration';
    static readonly visibility = 'visibility';
    static readonly opacity = 'opacity';
    static readonly rel = 'rel';
    static readonly href = 'href';
    static readonly _webkit_box_flex = '-webkit-box-flex';
    static readonly scale = 'scale';
    static readonly flex_basis = 'flex-basis';
    static readonly box_shadow = 'box-shadow';
    static readonly pointer_events = 'pointer-events';
    static readonly px = 'px';
    static readonly style = 'style';
    static readonly media_ = 'media';
    static readonly WIDE_SCREEN_MEDIA_EN_DEFAULT = 'only screen and (max-height : 500px), screen and (min-width : 500px)';
    static readonly WIDE_SCREEN_MEDIA_DIS = 'only screen and (max-height : -10px)';
    static readonly none = 'none';
    static readonly stylesheet = 'stylesheet';
    static readonly link = 'link';
    static readonly delete = 'delete';

    static readonly fixed = 'fixed';
    static readonly sticky = 'sticky';
    static readonly absolute = 'absolute';
    static readonly static = 'static';

    static readonly wrap = 'wrap';
    static readonly column = 'column';
    static readonly space_between = 'space-between';
    static readonly space_evenly = 'space-evenly';
    static readonly space_around = 'space-around';

    static readonly true = 'true';
    static readonly false = 'false';

    static readonly grid_template_columns = 'grid-template-columns';
    static readonly gap = 'gap';
    static readonly grid_auto_flow = 'grid-auto-flow';
    static readonly dense = 'dense';
    static readonly start = 'start';
    static readonly fit_content = 'fit-content';
    static readonly break_inside = 'break-inside';
    static readonly avoid = 'avoid';
    static readonly grid = 'grid';
    static readonly border_box = 'border-box';

    static readonly tag_truncate = 'truncate-tag';

    static readonly discrepancy = 1;
    static readonly maxCloseButtonSizeRatio = 0.15;
    static readonly bottomCloseButtonRatio = 0.6;

    static readonly flexDelHeightThreshold = 2;
    
    static readonly ERR_CODE_GAPS = 100;
    static readonly ERR_CODE_OVERFLOW = 50;
    static readonly ERR_CODE_OFFSET = 10;
    static readonly ERR_CODE_CLOSE_BTN_OVERLAP = 2;
    static readonly ERR_CODE_INTERACTIVE = 1;

    static readonly paddingTop = 'padding-top';
    static readonly paddingBottom = 'padding-bottom';

    // event name
    static readonly transitionend = 'transitionend';

    // localName
    static readonly script = 'script';
    static readonly noscript = 'noscript';
    static readonly back = 'back';
    static readonly swiperBox = 'swiper-box';

    // value
    static readonly num0px = '0px';
    static readonly hidden = 'hidden';
    static readonly num100percent = '100%';
    static readonly important = '!important';

    static readonly hw_ = 'hw_';
    static readonly a_ = 'a';
    static readonly hwA = Constant.hw_ + Constant.a_;
}

export const LayoutValue = {
    DISABLE: 'disable', // 用于记录不重排元素，不做任何操作
    VERTICAL_GRID: 'vertical_grid', // 用于记录宫格列表节点
    ASIDE_BAR: 'asideBar', // 用于记录纵向侧边栏
    BACK_IMG: 'backImg', // 用于记录大图(virImg)节点，后续不对此元素进行任何操作
    ONE_LINE: 'oneLine', // 用于记录单行节点，后续不对此元素进行任何操作
    ZOOM_PARENT: 'zoomParent', // 用于标记缩放过的节点的所有祖先节点，此节点禁止缩放，但可对其子孙元素中未缩放过的元素进行处理
    IS_IMG: 'isImg', // 用来记录该元素是否为图片
    HAS_TEXT_CHILD: 'hasTextChild', // 用来记录该元素是否包含文本子元素
    WATERFALL: 'waterfall', // 用于标记瀑布流
    SWIPER: 'swiper', // 标记swiper
    SCROLL_LIST: 'scrollList', // 标记纵向列表
    COMMENT: 'comment', // 用于标记评论
    HEADER: 'header', // 用于标记顶栏
    ACROSS_COLUMN: 'across_column', // 用于标记通栏
    NEXT_ROUND: 'next_round', // 用于在reset尚未生效阶段暂时阻塞其他布局的处理
    BOTTOM: 'bottom',
    EMPTY: 'empty', // 标记通栏处理的空元素
};

export const LayoutKey = {
    LAYOUT_TAG: 'hwLayout', // 用来记录元素是否被重排过
    CONFIG_LAYOUT_TAG: 'thirdLayout', // 用来记录元素是否被重排过
    DIFF_ELE: 'diffEle', // 用于记录本次重排需要处理的元素
    INSERT_RULE: 'insertRule', // insert rule 的元素
    INSERT_SWIPER: 'insertSwiper', // insert rule 的元素
};

export const HwTag = {
    MULTI_IMGS: 'multi_items',
    ELE_NUM: 'ele_num',
    ORIGIN_WIDTH: 'origin_width',
    HEIGHT_0: 'height0',
    FORCE_CSS: 'force_css',
    ORIGIN_HEIGHT: 'origin_height',
};
