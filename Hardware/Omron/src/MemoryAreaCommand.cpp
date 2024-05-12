#include "MemoryAreaCommand.h"

#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    // BASE CLASS

    MemoryAreaCommand::MemoryAreaCommand(FINSHeader header, FINS_MEMORY_AREA memory_area, char area_code, uint16_t address, uint8_t bit)
        : fins_header_(header), memory_area_(memory_area), area_code_(area_code), address_(address), bit_(bit) {}

    MemoryAreaCommand::MemoryAreaCommand(FINSHeader header, FINS_MEMORY_AREA memory_area, char area_code, uint16_t address, uint8_t bit, QByteArray content)
        : fins_header_(header), memory_area_(memory_area), area_code_(area_code), address_(address), bit_(bit), content_(content) {}

    QByteArray MemoryAreaCommand::Command() const {
        FINSFrame frame(fins_header_, ConstructFINSCommandData());
        return Construct_FINS_FRAME_SEND(frame);
    }

    bool MemoryAreaCommand::IsBitCommand() const {
        return (area_code_ == GetFINSBitAreaCode(memory_area_));
    }

    uint8_t MemoryAreaCommand::ParseMRC(QByteArray data) const {
        if (data.size() < 28) { throw std::runtime_error("Reply too short, cannot extract MRC"); }
        return static_cast<uint8_t>(data.at(26));
    }

    uint8_t MemoryAreaCommand::ParseSRC(QByteArray data) const {
        if (data.size() < 28) { throw std::runtime_error("Reply too short, cannot extract SRC"); }
        return static_cast<uint8_t>(data.at(27));
    }

    uint8_t MemoryAreaCommand::ParseMRES(QByteArray data) const {
        if (data.size() < 30) { throw std::runtime_error("Reply too short, cannot extract MRES"); }
        return static_cast<uint8_t>(data.at(28));
    }

    uint8_t MemoryAreaCommand::ParseSRES(QByteArray data) const {
        if (data.size() < 30) { throw std::runtime_error("Reply too short, cannot extract SRES"); }
        return static_cast<uint8_t>(data.at(29));
    }

    QByteArray MemoryAreaCommand::ParseContent(QByteArray data) const {
        if (data.size() == 31) {
            return data.right(1);
        } else if (data.size() == 32) {
            return data.right(2);
        } else {
            QByteArray no_content(0);
            return no_content;
        }
    }

    // READ

    MemoryAreaRead::MemoryAreaRead(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit)
        : MemoryAreaCommand(header, memory_area, GetFINSBitAreaCode(memory_area), address, bit) {
    }

    MemoryAreaRead::MemoryAreaRead(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address)
        : MemoryAreaCommand(header, memory_area, GetFINSWordAreaCode(memory_area), address, 0) {
    }

    FINSCommandData MemoryAreaRead::ConstructFINSCommandData() const {
        QByteArray full_address;
        full_address.append(area_code_);
        full_address.append(AsByteArray(address_, 2));
        full_address.append(bit_);
        if (full_address.size() != 4) {
            QString message = "Memory area read command has wrong size, should be 4, is " + QString::number(full_address.size());
            throw std::runtime_error(message.toStdString());
        }
        QByteArray n_items_to_read = AsByteArray(1, 2); // only implemented reading one bit or word
        return FINSCommandData(MRC_, SRC_, full_address + n_items_to_read);
    }

    void MemoryAreaRead::DeserializeReply(QByteArray reply) {
        if (!IsValidFINSReply(reply)) { throw std::runtime_error("Cannot deserialize content to memory area read reply"); }
        if (ParseFINSReplyType(reply) != BASE_TYPE) { throw std::runtime_error("This is not a fins frame send reply"); }
        if (ParseMRC(reply) != MRC_ || ParseSRC(reply) != SRC_) { throw std::runtime_error("This is not a memory area read command reply"); }
        frame_error_ = ParseFINSErrorCode(reply);
        mres_ = ParseMRES(reply);
        sres_ = ParseSRES(reply);
        content_ = ParseContent(reply);
    }

    // WRITE

    MemoryAreaWrite::MemoryAreaWrite(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content)
          :  MemoryAreaCommand(header, memory_area, GetFINSWordAreaCode(memory_area), address, 0, AsByteArray(content, 2)) {
    }

    MemoryAreaWrite::MemoryAreaWrite(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, uint16_t content)
          :  MemoryAreaCommand(header, memory_area, GetFINSBitAreaCode(memory_area), address, bit, AsByteArray(content, 1)) {
    }

    void MemoryAreaWrite::DeserializeReply(QByteArray reply) {
        if (!IsValidFINSReply(reply)) { throw std::runtime_error("Cannot deserialize content to memory area write reply"); }
        if (ParseFINSReplyType(reply) != BASE_TYPE) { throw std::runtime_error("This is not a fins frame send reply"); }
        if (ParseMRC(reply) != MRC_ || ParseSRC(reply) != SRC_) { throw std::runtime_error("This is not a memory area write command reply"); }
        frame_error_ = ParseFINSErrorCode(reply);
        mres_ = ParseMRES(reply);
        sres_ = ParseSRES(reply);
    }

    FINSCommandData MemoryAreaWrite::ConstructFINSCommandData() const {
        QByteArray full_address;
        full_address.append(QByteArray(1, area_code_));
        full_address.append(AsByteArray(address_, 2));
        full_address.append(bit_);
        if (full_address.size() != 4) {
            QString message = "Memory area write command has wrong size, should be 4, is " + QString::number(full_address.size());
            throw std::runtime_error(message.toStdString());
        }
        QByteArray n_items_to_write = AsByteArray(1, 2); // only implemented single word / bit writes
        return FINSCommandData(MRC_, SRC_, full_address + n_items_to_write + content_);
    }
}

