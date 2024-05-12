#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QObject>
#include <QString>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

enum class ModeCommand : int { MODEFUNCTIONNEMENT = 0,
                                PREAMPLITYPE = 1,
                                PREAMPLIGAMME = 2,
                                EMITINSTANTENOUSMEASUREMENT = 4,
                                EMITINTEGRATED1MEASUREMENT = 5,
                                EMITINTEGRATED2MEASUREMENT = 6,
                                INCLUDEDATEINREPLY = 7,
                                INCLUDERAWMEASUREMENTINREPLY = 8 };
enum class ModeFunctionnement : int { MEASUREMENT = 0, TESTCURRENT = 1, ZEROGAMMEBASSE = 2, ZEROGAMMEHAUT = 3, TESTSIGNALS = 4 };
enum class PreAmpliType : int { PI148A = 0, PI148B = 1 };
enum class PreAmpliGamme : int { G14T8 = 0, G13T7 = 1, G12T6 = 2, G11T5 = 3, G10T4 = 4 };

enum class ParameterCommand : int { SETINSTANTENOUSMEASUREMENTTIME = 15,
                                    SETINSTANTENOUSMEASUREMENTUNITS = 16,
                                    NMBMEASUREMENTSBELOWTHRESHOLDBEFOREALARMRESET = 17, // NEW
                                    SETINTEGRATIONTIME1 = 18,
                                    SETINTEGRATIONTIME2 = 19,
                                    NMBMEASUREMENTSINTEGRATION2 = 20, // NEW
                                    SETINSTANTENOUSCONVERSIONCOEFF = 21,
                                    SETINTEGRATEDCONVERSIONCOEFF = 22,
                                    SETINTEGRATIONTHRESHOLD = 23,
                                    WARNING1THRESHOLD = 24, // NEW
                                    WARNING2THRESHOLD = 25, // NEW
                                    WARNING3THRESHOLD = 26 }; // NEW
enum class ThresholdStatus : int { MEASUREMENTOFFORMEASUREMENTOK = 0,
                                   MEASUREMENTABOVETHRESHOLD1 = 1,
                                   MEASUREMENTABOVETHRESHOLD2 = 3,
                                   MEASUREMENTABOVETHRESHOLD3 = 7 };

}

#endif
