#ifndef COMMEQUIPMENT_FINS_H
#define COMMEQUIPMENT_FINS_H

#include "Omron_global.h"

#include <QObject>
#include <QByteArray>
#include <map>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    const QByteArray FINS_TCP_HEADER("FINS");

    enum class FINS_MEMORY_AREA { UNKNOWN = 0, CIO = 1, DM = 2, HR = 3 };
    enum class FINS_COMMAND_TYPE { NODE_ADDRESS_SEND = 0, NODE_ADDRESS_RECV = 1, FRAME_SEND = 2, FRAME_ERROR = 3, NODE_BUSY = 6 };
    enum class FINS_FRAME_SEND_COMMAND_TYPE { MEMORY_AREA_READ = 257, MEMORY_AREA_WRITE = 258, UNKNOWN = 0 };

    class OMRON_EXPORT FINSHeader {
    public:
        enum class FINS_HEADER_FIELD { ICF = 0, RSV = 1, GCT = 2, DNA = 3, DA1 = 4, DA2 = 5, SNA = 6, SA1 = 7, SA2 = 8, SID = 9 };
        FINSHeader(QByteArray data);
        FINSHeader(uint8_t ICF, // Information Control Field (80 = command)
                   uint8_t RSV, // Reserved by system (0)
                   uint8_t GCT, // Permissible number of gateways (2)
                   uint8_t DNA, // Destination Network Address (0 = Local)
                   uint8_t DA1, // Destination Node Address (0 = Local unit)
                   uint8_t DA2, // Destination Unit Address (0 = PLC (CPU UNIT))
                   uint8_t SNA, // Source Network Address
                   uint8_t SA1, // Source Node Address
                   uint8_t SA2, // Source Unit Address
                   uint8_t SID); // Service ID: Used to identify the process. Returns use the same number for SID as what they received
        FINSHeader(uint8_t DA1, uint8_t SA1); // default values for all parameters except source and destination node addresses
        QByteArray toByteArray() const;
        FINSHeader WithNewDA1(uint8_t DA1) const;
        uint8_t GetFrameField(FINS_HEADER_FIELD field) const;
        void SetFrameField(FINS_HEADER_FIELD field, uint8_t value);

    private:
        QByteArray data_;
        static const int SIZE = 10;
    };

    class FINSCommandData {
    public:
        FINSCommandData(QByteArray data);
        FINSCommandData(uint8_t MRC, uint8_t SRC, QByteArray body);
        QByteArray toByteArray() const;
        uint8_t MRC() const; //  main request/response code
        uint8_t SRC() const; // sub request/response code
        QByteArray body() const; // command body (ex: start adress + number of fields to read)
    private:
        QByteArray data_;
        const int MIN_SIZE = 4;
    };

    class FINSFrame {
    public:
        FINSFrame(FINSHeader header,
                  FINSCommandData data);
        QByteArray toByteArray() const;
        FINSHeader header() const { return header_; }
        FINSCommandData data() const { return commandData_; }
    private:
        FINSHeader header_;
        FINSCommandData commandData_;
    };

    FINS_MEMORY_AREA OMRON_EXPORT ToFinsMemoryArea(QString name);
    QString OMRON_EXPORT FromFinsMemoryArea(FINS_MEMORY_AREA memory_area);
    char OMRON_EXPORT GetFINSBitAreaCode(FINS_MEMORY_AREA memory_area);
    char OMRON_EXPORT GetFINSWordAreaCode(FINS_MEMORY_AREA memory_area);
    QString OMRON_EXPORT HashedAddress(uint8_t node, FINS_MEMORY_AREA area, uint16_t address, uint8_t bit);

    QByteArray AsByteArray(uint32_t value, int n_bytes);

    uint32_t AsInt32(QByteArray data);
    uint16_t AsInt16(QByteArray data);
    uint8_t AsInt8(QByteArray data);

    QByteArray EmptyErrorBlock();

    bool IsValidFINSReply(QByteArray data);
    uint32_t ParseFINSErrorCode(QByteArray data);
    FINS_COMMAND_TYPE ParseFINSReplyType(QByteArray data);
    FINS_FRAME_SEND_COMMAND_TYPE ParseFINSFrameSendCommandType(QByteArray data);
}
#endif
