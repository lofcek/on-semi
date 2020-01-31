#include "LockGuard.h"
#include "gtest/gtest.h"
#include <mutex>

// Tests c-tor with std::mutex.
TEST(LockGuardTest, CtorWithStdMutex) {
    std::mutex mtx;
    {
        challenge::lock_guard<std::mutex> g(mtx);
        ASSERT_FALSE(mtx.try_lock());
    }
    // mtx should be released after destructing guard
    ASSERT_TRUE(mtx.try_lock());
}

// Tests c-tor with adopt_lock_t.
TEST(LockGuardTest, CtorWithStdAdoptLock) {
    std::mutex mtx;
    {
        challenge::lock_guard<std::mutex> g(mtx, challenge::adopt_lock_t());
        ASSERT_TRUE(mtx.try_lock());
    }
    // mtx should be released after destructing guard
    ASSERT_TRUE(mtx.try_lock());
}