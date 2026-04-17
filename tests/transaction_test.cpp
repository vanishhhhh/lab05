#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>

#include "Account.h"
#include "Transaction.h"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}

    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int diff), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

TEST(Transaction, FeeCanBeReadAndChanged) {
    Transaction tr;

    EXPECT_EQ(tr.fee(), 1);

    tr.set_fee(25);
    EXPECT_EQ(tr.fee(), 25);
}

TEST(Transaction, ThrowsForInvalidInput) {
    Transaction tr;
    Account a1(1, 500);
    Account a2(2, 500);

    EXPECT_THROW(tr.Make(a1, a1, 200), std::logic_error);
    EXPECT_THROW(tr.Make(a1, a2, -100), std::invalid_argument);
    EXPECT_THROW(tr.Make(a1, a2, 99), std::logic_error);
}

TEST(Transaction, ReturnsFalseIfFeeIsTooLarge) {
    Transaction tr;
    Account from(1, 500);
    Account to(2, 500);

    tr.set_fee(60);

    EXPECT_FALSE(tr.Make(from, to, 100));
}

TEST(Transaction, RealTransactionCallsRealSaveToDataBase) {
    Transaction tr;
    Account from(1, 500);
    Account to(2, 500);

    tr.set_fee(1);

    EXPECT_TRUE(tr.Make(from, to, 200));

    // Проверяем текущее поведение кода из репозитория
    EXPECT_EQ(from.GetBalance(), 500);
    EXPECT_EQ(to.GetBalance(), 499);
}

TEST(Transaction, SuccessPathWithMocks) {
    StrictMock<MockTransaction> tr;
    StrictMock<MockAccount> from(1, 500);
    StrictMock<MockAccount> to(2, 500);

    tr.set_fee(1);

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);

    EXPECT_CALL(to, ChangeBalance(200)).Times(1);
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(500));
    EXPECT_CALL(to, ChangeBalance(-201)).Times(1);

    EXPECT_CALL(tr, SaveToDataBase(_, _, 200)).Times(1);

    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_TRUE(tr.Make(from, to, 200));
}

TEST(Transaction, RollbackPathWithMocks) {
    StrictMock<MockTransaction> tr;
    StrictMock<MockAccount> from(1, 500);
    StrictMock<MockAccount> to(2, 100);

    tr.set_fee(100);

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);

    EXPECT_CALL(to, ChangeBalance(200)).Times(1);
    EXPECT_CALL(to, GetBalance()).WillOnce(Return(250));
    EXPECT_CALL(to, ChangeBalance(-200)).Times(1);

    EXPECT_CALL(tr, SaveToDataBase(_, _, 200)).Times(1);

    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    EXPECT_FALSE(tr.Make(from, to, 200));
}