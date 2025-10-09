export interface AdjacentImageInfo {
    adjacentWrapper: HTMLElement; // 拼接图包裹层
    adjacentImages: HTMLElement[]; // 发生拼接的两张图片
    adjacentLargeImages?: HTMLElement[]; // 与拼接图拼接的大图
}