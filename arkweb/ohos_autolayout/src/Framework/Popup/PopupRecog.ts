import { PopupInfo } from './PopupInfo';
import { PredictionResult } from './PredictionResult';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';

// 定义模型参数的类型
interface ModelParameters {
    readonly features: string[];
    readonly coefficients: number[];
    readonly intercept: number;
    readonly scalerMean: number[];
    readonly scalerScale: number[];
}

// 定义预处理后的特征类型，将特征名称映射到数字
type ProcessedFeatures = { [key: string]: number };


// 模型参数保持不变
const modelParameters: ModelParameters = {
    features: [
        'root_position',            // 根节点的position
        // 'root_zindex',              // 根节点的层级
        'has_mask',                 // 是否存在遮罩
        'root_screen_area_ratio',   // 根节点的屏幕占比
        'root_is_visiable',         // 根节点是否可见
        'has_close_button',         // 是否存在关闭按钮
        'mask_area_ratio',          // 遮罩的屏幕占比
        'mask_position',            // 遮罩的position
        // 'mask_zindex'               // 遮罩的层级
    ],
    // 模型参数精度限制成3位小数，如果发现识别错误，请先启用全精度模型测试，确定是否因为模型精度导致的错误
    // 3位精度浮点
    coefficients: [-0.008, 0.565, 1.012, -0.214, 0.377, 2.006, 0.868] ,
    intercept: 3.593 ,
    scalerMean: [3.546, 0.948, 47.741, 0.588, 0.856, 80.64, 4.165] ,
    scalerScale: [0.557, 0.221, 48.176, 0.492, 0.351, 38.879, 0.621]
};

// 预编码的 position 值及其定义类型
const positionMapping: { [key: string]: number } = {
    'sticky': 1,
    'relative': 2,
    'static': 3,
    'fixed': 4,
    'absolute': 5
};

export class PopupRecog {
    /**
     * 将原始特征数据预处理为模型所需的数值格式。
     * @param rawFeatures - 原始特征对象。
     * @returns 一个将特征名称映射到其处理后数值的对象。
     */
    static preprocessFeatures(rawFeatures: PopupInfo): ProcessedFeatures {
        const features: ProcessedFeatures = {};
        
        // 1. 处理位置特征
        const rootPosKey = String(rawFeatures.root_position).toLowerCase();
        features.root_position = positionMapping[rootPosKey] ?? NaN; // 如果未找到，使用 ?? 简洁地分配 NaN

        const maskPosKey = String(rawFeatures.mask_position).toLowerCase();
        features.mask_position = positionMapping[maskPosKey] ?? NaN;

        // 2. 处理布尔值特征
        features.has_mask = rawFeatures.has_mask ? 1 : 0;
        features.root_is_visiable = rawFeatures.root_is_visiable ? 1 : 0;
        features.has_close_button = rawFeatures.has_close_button ? 1 : 0;

        // 3. 处理数值特征
        features.root_screen_area_ratio = rawFeatures.root_screen_area_ratio;
        features.mask_area_ratio = rawFeatures.mask_area_ratio;

        return features;
    }

    /**
     * 预测给定的特征是否代表一个弹窗。
     * @param rawFeatures - 从 DOM 收集的原始特征对象。
     * @returns 一个包含预测结果（1 代表弹窗，0 代表非弹窗）和概率的对象。
     */
    static predictIsPopup(rawFeatures: PopupInfo): PredictionResult {
        // 1. 预处理原始输入特征
        const features = PopupRecog.preprocessFeatures(rawFeatures);

        let linearCombination = modelParameters.intercept;

        // 2. 标准化特征并计算线性组合
        for (let i = 0; i < modelParameters.features.length; i++) {
            const featureName = modelParameters.features[i];
            const featureValue = features[featureName];

            // 确保特征在预处理后存在并且是有效的数字
            if (featureValue !== undefined && !isNaN(featureValue)) {
                // 标准化
                const scaledValue = (featureValue - modelParameters.scalerMean[i]) / modelParameters.scalerScale[i];
                // 添加到线性组合中
                linearCombination += modelParameters.coefficients[i] * scaledValue;
            } else {
                Log.w(`预处理后的特征 '${featureName}' 是 undefined 或 NaN，返回非弹窗预测`, Tag.popupRecog);
                // 如果关键特征缺失，我们可以断定它不是弹窗。
                return { prediction: 0, probability: 0 };
            }
        }
        
        // Sigmoid 函数用于计算概率
        const sigmoid = (z: number): number => {
            return 1 / (1 + Math.exp(-z));
        }

        // 3. 计算最终概率
        const probability = sigmoid(linearCombination);

        // 4. 基于 0.5 的阈值进行分类
        const prediction = probability >= 0.5 ? 1 : 0;

        return {
            prediction,
            probability
        };
    }
}