#include <QtTest/QtTest>

#include "OmronRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class TestOmronRepo : public QObject {
    Q_OBJECT
public:
    TestOmronRepo();
    ~TestOmronRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetOmronChannelNames();
    void TestGetOmronChannel();
    void TestGetChannels();
    void TestGetBarcoMessages();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    OmronRepo repo_;
};

}
