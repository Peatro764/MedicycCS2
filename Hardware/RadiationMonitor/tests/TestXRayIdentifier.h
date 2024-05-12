#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class TestXRayIdentifier : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void SingleEntryInInterval();
    void MultipleEntryInInterval();
    void PeakValue();

private:
};

}

