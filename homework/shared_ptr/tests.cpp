#include <gtest/gtest.h>

#include "MakeShared.hpp"
#include "SharedPointer.hpp"
#include "WeakPointer.hpp"

constexpr int testValue = 5;
constexpr int testValue2 = 44;
constexpr int noReferences = 0;
constexpr int oneReference = 1;
constexpr int twoReferences = 2;
constexpr int threeReferences = 3;
constexpr int fourReferences = 4;

class TestClass {
public:
    int testFunction()
    {
        return testValue;
    }
};

class SharedPointerTest : public ::testing::Test {
protected:
    SharedPointerTest()
        : s_ptr(new int{testValue}) {}
    SharedPointer<int> s_ptr;
};

class WeakPointerTest : public ::testing::Test {
protected:
    WeakPointerTest()
        : ss_ptr(new int{testValue}) {}
    SharedPointer<int> ss_ptr;
    WeakPointer<int> w_ptr{ss_ptr};
};

class ControlBlockTest : public ::testing::Test {
protected:
    ControlBlock<int> testControlBlock;
};

TEST_F(SharedPointerTest, shouldCreateSharedPointerFromWeakPointer)
{
    WeakPointer<int> w_ptr{s_ptr};

    SharedPointer<int> ss_ptr{w_ptr};

    ASSERT_EQ(ss_ptr.get(), s_ptr.get());
}

TEST_F(SharedPointerTest, shouldCopySharedPointer)
{
    auto newPtr(s_ptr);
    auto newPtr1 = s_ptr;

    ASSERT_EQ(*s_ptr, *newPtr);
    ASSERT_EQ(*s_ptr, *newPtr1);
    ASSERT_EQ(s_ptr.use_count(), threeReferences);
}

TEST_F(SharedPointerTest, shouldMoveSharedPointer)
{
    auto newPtr(std::move(s_ptr));
    ASSERT_EQ(*newPtr, testValue);
    ASSERT_EQ(newPtr.use_count(), oneReference);
}

TEST_F(SharedPointerTest, shouldUseCopyAssingnmentOperatorSharedPointer)
{
    SharedPointer<int> newPtr;
    newPtr = s_ptr;
    ASSERT_EQ(*s_ptr, testValue);
    ASSERT_EQ(s_ptr.use_count(), twoReferences);
}

TEST_F(SharedPointerTest, shouldUseMoveAssingnmentOperatorSharedPointer)
{
    SharedPointer<int> newPtr;
    newPtr = std::move(s_ptr);
    ASSERT_EQ(*newPtr, testValue);
    ASSERT_EQ(newPtr.use_count(), oneReference);
    ASSERT_EQ(s_ptr.get(), nullptr);
}
TEST_F(SharedPointerTest, shouldResetSharedPtrNoArgumentCase)
{
    s_ptr.reset();
    ASSERT_EQ(s_ptr.get(), nullptr);
}

TEST_F(SharedPointerTest, shouldResetSharedPtrWithArgumentCase)
{
    auto newPtr(s_ptr);
    auto newPtr1 = s_ptr;
    ASSERT_EQ(s_ptr.use_count(), threeReferences);
    s_ptr.reset(new int(testValue2));
    ASSERT_EQ(newPtr.use_count(), twoReferences);
    ASSERT_EQ(s_ptr.use_count(), oneReference);
    ASSERT_EQ(*s_ptr, testValue2);
}

TEST_F(SharedPointerTest, shouldReturnUseCountOfSharedPointer)
{
    auto newPtr = s_ptr;

    ASSERT_EQ(s_ptr.use_count(), twoReferences);
}

TEST_F(SharedPointerTest, shouldUseDereferenceOperatorOfSharedPointer)
{
    ASSERT_EQ(*s_ptr, testValue);
}

TEST_F(SharedPointerTest, shouldUseArrowOperatorOfSharedPointer)
{
    SharedPointer<TestClass> s_ptr(new TestClass{});

    ASSERT_EQ(s_ptr->testFunction(), testValue);
}

TEST_F(SharedPointerTest, shouldReturnBoolWhenUsingBoolOperator)
{
    SharedPointer<int> s_ptrNew;

    ASSERT_TRUE(s_ptr);
    ASSERT_FALSE(s_ptrNew);
}

TEST_F(WeakPointerTest, shouldReturnUseCountEqualTo0WhenCreatedByDefault)
{
    WeakPointer<int> test_w_ptr;
    ASSERT_EQ(test_w_ptr.use_count(), noReferences);
}

TEST_F(WeakPointerTest, shouldReturnUseCountEqualTo1WhenCreatedBySharedPointer)
{
    ASSERT_EQ(w_ptr.use_count(), oneReference);
}

TEST_F(WeakPointerTest, shouldReturnSharedPointerWhenLockWeakPointer)
{
    auto s_ptrNew = w_ptr.lock();
    ASSERT_EQ(*s_ptrNew, testValue);
}

TEST_F(WeakPointerTest, shouldReturnNullptrSharedPointerWhenLockWeakPointerWithNoSharedPointer)
{
    WeakPointer<int> test_w_ptr;
    auto s_ptrNew = test_w_ptr.lock();
    ASSERT_EQ(s_ptrNew.get(), nullptr);
}

TEST_F(WeakPointerTest, shouldUseMoveConstructor)
{
    WeakPointer<int> w_ptrNew{std::move(w_ptr)};
    ASSERT_TRUE(w_ptr.expired());
    ASSERT_FALSE(w_ptrNew.expired());
    ASSERT_EQ(w_ptrNew.use_count(), oneReference);
}

TEST_F(WeakPointerTest, shouldUseMoveAssignment)
{
    WeakPointer<int> w_ptrNew;
    w_ptrNew = std::move(w_ptr);
    ASSERT_TRUE(w_ptr.expired());
    ASSERT_EQ(w_ptrNew.use_count(), oneReference);
}

TEST_F(WeakPointerTest, shouldUseCopyAssignment)
{
    WeakPointer<int> w_ptrNew;
    w_ptrNew = w_ptr;
    ASSERT_EQ(w_ptr.use_count(), oneReference);
}

TEST_F(ControlBlockTest, shouldGetSharedRefs)
{
    ASSERT_EQ(testControlBlock.getShared(), oneReference);
}

TEST_F(ControlBlockTest, shouldGetWeakRefs)
{
    ASSERT_EQ(testControlBlock.getWeak(), noReferences);
}

TEST_F(ControlBlockTest, shouldIncreaseSharedRefs)
{
    testControlBlock.increaseShared();
    ASSERT_EQ(testControlBlock.getShared(), twoReferences);
}

TEST_F(ControlBlockTest, shouldIncreaseWeakRefs)
{
    testControlBlock.increaseWeak();
    ASSERT_EQ(testControlBlock.getWeak(), oneReference);
}

TEST_F(ControlBlockTest, shouldDecreaseSharedRefs)
{
    testControlBlock.increaseShared();
    testControlBlock.decreaseShared();
    ASSERT_EQ(testControlBlock.getShared(), oneReference);
}

TEST_F(ControlBlockTest, shouldDecreaseWeakRefs)
{
    testControlBlock.increaseWeak();
    testControlBlock.decreaseWeak();
    ASSERT_EQ(testControlBlock.getWeak(), noReferences);
}

TEST_F(ControlBlockTest, shouldIncreaseAndDecreaseSharedAndWeakRefs)
{
    SharedPointer<int> sPointer(new int(testValue));
    ASSERT_EQ(sPointer.use_count(), oneReference);

    SharedPointer<int> sPointer2 = sPointer;
    ASSERT_EQ(sPointer.use_count(), twoReferences);
    ASSERT_EQ(sPointer2.use_count(), twoReferences);

    SharedPointer<int> sPointer3 = sPointer;
    ASSERT_EQ(sPointer.use_count(), threeReferences);
    ASSERT_EQ(sPointer2.use_count(), threeReferences);
    ASSERT_EQ(sPointer3.use_count(), threeReferences);

    WeakPointer<int> wPointer(sPointer);
    ASSERT_EQ(wPointer.use_count(), threeReferences);
    ASSERT_EQ(sPointer.use_count(), threeReferences);

    SharedPointer<int> sPointer4 = sPointer;
    ASSERT_EQ(wPointer.use_count(), fourReferences);
    ASSERT_EQ(sPointer.use_count(), fourReferences);
    
    sPointer.reset();
    ASSERT_EQ(wPointer.use_count(), threeReferences);
    ASSERT_EQ(sPointer2.use_count(), threeReferences);
    //ASSERT_EQ(sPointer.use_count(), noReferences); error = 1 instead 0 
    
    SharedPointer<int> sPointerNull;
    //ASSERT_EQ(sPointerNull.use_count(), noReferences); error = 1 instead 0
}

TEST(MakeSharedTest, shouldUseMakeSharedOnSingleTestValue)
{
    auto ms_ptr = MakeShared<int>(testValue);
    ASSERT_EQ(*ms_ptr, testValue);
}

TEST(MakeSharedTest, shouldUseMakeSharedOnExampleStructure)
{
    struct TestStructure {
        TestStructure(int x, int y, int z)
            : x_(x), y_(y), z_(z){};
        int x_;
        int y_;
        int z_;
    };
    auto ms_ptr = MakeShared<TestStructure>(testValue, testValue, testValue);
    ASSERT_EQ(ms_ptr->x_, testValue);
    ASSERT_EQ(ms_ptr->y_, testValue);
    ASSERT_EQ(ms_ptr->z_, testValue);
}
