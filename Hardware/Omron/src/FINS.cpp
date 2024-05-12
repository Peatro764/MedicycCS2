#include "FINS.h"

#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    // FINSHEADER
    FINSHeader::FINSHeader(QByteArray data) {
        if (data.size() != SIZE) {
            QString message = "FINS header size is " + QString::number(data.size()) + ", should be " + QString::number(SIZE);
            throw std::runtime_error(message.toStdString());
        }
        data_ = data;
    }

    FINSHeader::FINSHeader(uint8_t ICF,
                           uint8_t RSV,
                           uint8_t GCT,
                           uint8_t DNA,
                           uint8_t DA1,
                           uint8_t DA2,
                           uint8_t SNA,
                           uint8_t SA1,
                           uint8_t SA2,
                           uint8_t SID) {
        data_.resize(10);
        data_[static_cast<int>(FINS_HEADER_FIELD::ICF)] = static_cast<char>(ICF);
        data_[static_cast<int>(FINS_HEADER_FIELD::RSV)] = static_cast<char>(RSV);
        data_[static_cast<int>(FINS_HEADER_FIELD::GCT)] = static_cast<char>(GCT);
        data_[static_cast<int>(FINS_HEADER_FIELD::DNA)] = static_cast<char>(DNA);
        data_[static_cast<int>(FINS_HEADER_FIELD::DA1)] = static_cast<char>(DA1);
        data_[static_cast<int>(FINS_HEADER_FIELD::DA2)] = static_cast<char>(DA2);
        data_[static_cast<int>(FINS_HEADER_FIELD::SNA)] = static_cast<char>(SNA);
        data_[static_cast<int>(FINS_HEADER_FIELD::SA1)] = static_cast<char>(SA1);
        data_[static_cast<int>(FINS_HEADER_FIELD::SA2)] = static_cast<char>(SA2);
        data_[static_cast<int>(FINS_HEADER_FIELD::SID)] = static_cast<char>(SID);
    }

    FINSHeader::FINSHeader(uint8_t DA1, uint8_t SA1) {
        data_ = FINSHeader(0x80, 0, 0x02, 0, DA1, 0, 0, SA1, 0, 0).toByteArray();
    }

    QByteArray FINSHeader::toByteArray() const {
        return data_;
    }

    FINSHeader FINSHeader::WithNewDA1(uint8_t DA1) const {
        QByteArray updated(data_);
        updated[static_cast<int>(FINS_HEADER_FIELD::DA1)] = static_cast<char>(DA1);
        return updated;
    }


    uint8_t FINSHeader::GetFrameField(FINS_HEADER_FIELD field) const {
        return static_cast<uint8_t>(data_.at(static_cast<int>(field)));
    }

    void FINSHeader::SetFrameField(FINS_HEADER_FIELD field, uint8_t value) {
        data_[static_cast<int>(field)] = static_cast<char>(value);
    }

    // FINSCOMMANDDATA
    FINSCommandData::FINSCommandData(QByteArray data) {
        if (data.size() < MIN_SIZE) {
            QString message = "FINS command data size is " + QString::number(data.size()) + ", should be >= " + QString::number(MIN_SIZE);
            throw std::runtime_error(message.toStdString());
        }
        data_ = data;
    }

    FINSCommandData::FINSCommandData(uint8_t MRC, uint8_t SRC, QByteArray body) {
        data_.append(static_cast<char>(MRC));
        data_.append(static_cast<char>(SRC));
        data_.append(body);
    }

    uint8_t FINSCommandData::MRC() const {
        return static_cast<uint8_t>(data_.at(0));
    }

    uint8_t FINSCommandData::SRC() const {
        return static_cast<uint8_t>(data_.at(1));
    }

    QByteArray FINSCommandData::body() const {
        return data_.right(data_.size() - 2);
    }

    QByteArray FINSCommandData::toByteArray() const {
        return data_;
    }

    //  FINSFRAME
    FINSFrame::FINSFrame(FINSHeader header,
                         FINSCommandData data)
        : header_(header), commandData_(data) {}

     QByteArray FINSFrame::toByteArray() const {
         QByteArray sum = header_.toByteArray();
         return sum.append(commandData_.toByteArray());
     }

    // UTILITY FUNCTIONS
     FINS_MEMORY_AREA ToFinsMemoryArea(QString name) {
         if (name == QString("CIO")) {
             return FINS_MEMORY_AREA::CIO;
         } else if (name == QString("DM")) {
             return FINS_MEMORY_AREA::DM;
         } else if (name == QString("HR")) {
             return FINS_MEMORY_AREA::HR;
         } else {
             return FINS_MEMORY_AREA::UNKNOWN;
         }
     }

     QString FromFinsMemoryArea(FINS_MEMORY_AREA memory_area) {
         QString str;
         switch (memory_area) {
         case FINS_MEMORY_AREA::DM:
             str = QString("DM");
             break;
         case FINS_MEMORY_AREA::CIO:
             str = QString("CIO");
             break;
         case FINS_MEMORY_AREA::HR:
             str = QString("HR");
             break;
         default:
             str = QString("Unknown");
             break;
         }
         return str;
     }


     char GetFINSBitAreaCode(FINS_MEMORY_AREA memory_area) {
         char code(0);
         switch (memory_area) {
         case FINS_MEMORY_AREA::CIO:
             code = 0x30;
             break;
         case FINS_MEMORY_AREA::DM:
             code = 0x02;
             break;
         case FINS_MEMORY_AREA::HR:
             code = 0x32;
             break;
         default:
             code = 0;
             break;
         }
         return code;
     }

     char GetFINSWordAreaCode(FINS_MEMORY_AREA memory_area) {
         char code(0);
         switch (memory_area) {
         case FINS_MEMORY_AREA::CIO:
             code = 0xB0;
             break;
         case FINS_MEMORY_AREA::DM:
             code = 0x82;
             break;
         case FINS_MEMORY_AREA::HR:
             code = 0xB2;
             break;
         default:
             code = 0;
             break;
         }
         return code;
     }

     QString HashedAddress(uint8_t node, FINS_MEMORY_AREA area, uint16_t address, uint8_t bit) {
         QString hash = QString("%1%2%3%4").arg(node).arg(FromFinsMemoryArea(area)).arg(address).arg(bit);
         return hash;
     }


    QByteArray EmptyErrorBlock() {
        return AsByteArray(0, 4);
    }

    QByteArray AsByteArray(uint32_t value, int n_bytes) {
        if (value > (std::pow(2, 8 * n_bytes) - 1)) {
            QString message = QString("The value ") + QString::number(value) + " cannot be represented in " +QString::number(n_bytes) + " bytes";
            throw std::runtime_error(message.toStdString().c_str());
        }
        QByteArray ba;
        ba.resize(n_bytes);
        for (int byte = 0; byte < n_bytes; ++byte) {
            ba[n_bytes - byte - 1] = static_cast<char>((value >> 8*byte) & 0xFF);
        }
        return ba;
    }

    uint32_t AsInt32(QByteArray data) {
        const int size = data.size();
        if (size > 4) { throw std::runtime_error("AsInt32 only converts bytearrays with a size up to 4 bytes"); }
        uint32_t value(0);
        for (int byte = 0; byte < size; ++byte) {
            value = value + static_cast<uint16_t>((static_cast<uint8_t>(data[size - byte - 1]) << 8*byte));
        }
        return value;
    }

    uint16_t AsInt16(QByteArray data) {
        const int size = data.size();
        if (size > 2) { throw std::runtime_error("AsInt16 only converts bytearrays with a size up to 2 bytes"); }
        uint16_t value(0);
        for (int byte = 0; byte < size; ++byte) {
            value = value + static_cast<uint16_t>((static_cast<uint8_t>(data[size - byte - 1]) << 8*byte));
        }
        return value;
    }

    uint8_t AsInt8(QByteArray data) {
        if (data.size() != 1) { throw std::runtime_error("AsInt8 only converts bytearrays with a size of 1 byte"); }
        return static_cast<uint8_t>(data[0]);
    }

    bool IsValidFINSReply(QByteArray data) {
        if (data.length() < 16) { return false; }
        const QByteArray header = data.mid(0, 4);
        if (header != FINS_TCP_HEADER) { return false; }
        return true;
    }

    uint32_t ParseFINSErrorCode(QByteArray data) {
        if (data.size() < 16) { throw std::runtime_error("Cannot parse error code, message too small"); }
        return AsInt32(data.mid(12, 4));
    }

    FINS_COMMAND_TYPE ParseFINSReplyType(QByteArray data) {
        if (data.size() < 12) { throw std::runtime_error("Cannot parse FINS reply type, message too small"); }
        return static_cast<FINS_COMMAND_TYPE>(AsInt32(data.mid(8, 4)));
    }

    FINS_FRAME_SEND_COMMAND_TYPE ParseFINSFrameSendCommandType(QByteArray data) {
        if (data.size() < 28) { throw std::runtime_error("Cannot parse FINS frame send command reply type, message too small"); }
        return static_cast<FINS_FRAME_SEND_COMMAND_TYPE>(AsInt32(data.mid(26, 2)));
    }
}

