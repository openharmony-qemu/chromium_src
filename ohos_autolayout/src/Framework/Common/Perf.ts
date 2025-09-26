/**
 * 函数执行日志装饰器（支持同步/异步方法）
 * @param config 可选的配置参数，例如日志级别、启用状态等
 */
export enum Level{
    DEBUG = "DEBUG",
    INFO = "INFO",
    ERROR = "ERROR"
}

export function PerfExecution(config?: { level?: string; enabled?: boolean }) {
    return function (target: Object, propertyKey: string | symbol, descriptor: PropertyDescriptor) {
        if (config?.enabled === false) return descriptor;

        const originalMethod = descriptor.value;

        // 包装为同步或异步逻辑
        const wrappedMethod = function (this: any, ...args: any[]) {
            console.log(`[${config?.level || Level.INFO}] Enter ${propertyKey.toString()}`, args);

            try {
                const result = originalMethod.apply(this, args);

                // 处理异步方法
                if (result instanceof Promise) {
                    return result
                        .then((res) => {
                            console.log(`[${config?.level || Level.INFO}] Exit Success ${propertyKey.toString()}`, res);
                            return res;
                        })
                        .catch((err) => {
                            console.log(`[${config?.level || Level.ERROR}] Exit Error ${propertyKey.toString()}`, err);
                            throw err;
                        });
                }

                // 同步方法直接返回
                console.log(`[${config?.level || Level.INFO}] Exit Success ${propertyKey.toString()}`, result);
                return result;
            } catch (error) {
                console.log(`[${config?.level || Level.ERROR}] Exit Error ${propertyKey.toString()}`, error);
                throw error;
            }
        };

        descriptor.value = wrappedMethod;
        return descriptor;
    };
}
