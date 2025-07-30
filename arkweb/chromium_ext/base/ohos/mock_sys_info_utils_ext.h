#include <gmock/gmock.h>
#include <gtest/gtest.h>

class SystemPropertiesMock {
public:
    static SystemPropertiesMock& getInstance() {
        static SystemPropertiesMock instance;
        return instance;
    };
    MOCK_METHOD(bool, IsPcDeviceMock, (), ());
};

#ifdef __cplusplus
extern "C" {
#endif
bool __wrap_IsPcDevice() {
    return SystemPropertiesMock::getInstance().IsPcDeviceMock();
}
#ifdef __cplusplus
}
#endif