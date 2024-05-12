#include "TestQueue.h"
#include "ThreadSafeQueue.h"

namespace medicyc::cyclotroncontrolsystem::global {

void TestQueue::initTestCase() {}

void TestQueue::cleanupTestCase() {}

void TestQueue::init() {}

void TestQueue::cleanup() {}

void TestQueue::TestEmptyQueue() {
    ThreadSafeQueue<int> q;
    QVERIFY(q.empty());

    try {
        q.clear();
    } catch(std::exception& exc) {
        QFAIL("Clear on empty queue should not throw");
    }

    try {
        q.pop();
        QFAIL("pop on empty queue should throw");
    } catch(std::exception& exc) {
        (void)exc;
    }

    try {
        q.front();
        QFAIL("front on empty queue should throw");
    } catch(std::exception& exc) {
        (void)exc;
    }
}

void TestQueue::TestSingleEntry() {
    ThreadSafeQueue<int> q1;
    q1.push(1);
    QVERIFY(!q1.empty());

    QVERIFY(q1.front() == 1);
    QVERIFY(!q1.empty());

    QVERIFY(q1.pop() == 1);
    QVERIFY(q1.empty());

    ThreadSafeQueue<int> q2;
    q2.push(1);
    q2.clear();
    QVERIFY(q2.empty());
}

void TestQueue::TestMultipleEntry() {
    ThreadSafeQueue<int> q1;
    q1.push(1);
    q1.push(2);

    QVERIFY(q1.front() == 1);
    QVERIFY(q1.front() == 1);
    QVERIFY(q1.pop() == 1);
    QVERIFY(q1.front() == 2);
    QVERIFY(q1.pop() == 2);
    QVERIFY(q1.empty());
}

void TestQueue::TestAddQueue() {
    ThreadSafeQueue<int> q1;
    q1.push(1);
    q1.push(2);

    std::queue<int> q2;
    q2.push(3);
    q2.push(4);
    q2.push(5);

    q1.push(q2);
    QVERIFY(q1.pop() == 1);
    QVERIFY(q1.pop() == 2);
    QVERIFY(q1.pop() == 3);
    QVERIFY(q1.pop() == 4);
    QVERIFY(q1.pop() == 5);
    QVERIFY(q1.empty());
}

}
