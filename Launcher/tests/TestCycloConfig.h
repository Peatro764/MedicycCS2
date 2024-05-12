#include <QtTest/QtTest>

namespace cyclo_config {

class TestCycloConfig : public QObject {
    Q_OBJECT
public:
    TestCycloConfig();
    ~TestCycloConfig();
private slots:
    bool VerifyCount(QSignalSpy& spy, int expected_count, int timeout);
    bool LoadSqlResourceFile(QString file);
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestLastConfig();
    void TestGetConfig_SingleQuery();
    void TestGetConfig_MultiQuery();
    void TestGetBPLevels();

private:
    QString db_name_ = "testdatabase";
};

}
