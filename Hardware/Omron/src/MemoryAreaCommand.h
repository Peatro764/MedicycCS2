#ifndef COMMEQUIPMENT_MEMORYAREACOMMAND_H
#define COMMEQUIPMENT_MEMORYAREACOMMAND_H

#include "Omron_global.h"

#include <QObject>
#include <QByteArray>

#include "FINS.h"
#include "FrameSendCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    // BASECLASS
    class MemoryAreaCommand : public FrameSendCommand {
    public:
        explicit MemoryAreaCommand(FINSHeader header, FINS_MEMORY_AREA memory_area, char area_code, uint16_t address, uint8_t bit);
        explicit MemoryAreaCommand(FINSHeader header, FINS_MEMORY_AREA memory_area, char area_code, uint16_t address, uint8_t bit, QByteArray content);
        virtual ~MemoryAreaCommand() {}
        QByteArray Command() const;
        virtual void DeserializeReply(QByteArray reply) = 0;
        bool IsBitCommand() const;

        //uint16_t Content() const { return content_; }
        uint16_t Content() const { return AsInt16(content_); }
        uint16_t Address() const { return address_; }
        uint8_t Bit() const { return bit_; }
        uint32_t FrameError() const { return frame_error_; }
        uint8_t MainResponseCode() const { return mres_; }
        uint8_t SubResponseCode() const { return sres_; }
        uint8_t Node() const { return fins_header_.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1); }
        FINS_MEMORY_AREA MemoryArea() const { return memory_area_; }
        char AreaCode() const { return area_code_; }
        virtual FINSCommandData ConstructFINSCommandData() const = 0;
        uint8_t mres() const { return mres_; }
        uint8_t sres() const { return sres_; }
    protected:
        uint8_t ParseMRC(QByteArray data) const;
        uint8_t ParseSRC(QByteArray data) const;
        uint8_t ParseMRES(QByteArray data) const;
        uint8_t ParseSRES(QByteArray data) const;
        QByteArray ParseContent(QByteArray data) const;
        FINSHeader fins_header_;
        FINS_MEMORY_AREA memory_area_ = FINS_MEMORY_AREA::UNKNOWN;
        char area_code_ = 0x0;
        uint16_t address_ = 0;
        uint8_t bit_ = 0;
        QByteArray content_ = 0;
        uint32_t frame_error_ = 0;
        uint8_t mres_ = 0; // main response code
        uint8_t sres_ = 0; // sub response code
    };

    // READ SUPERCLASS
    class MemoryAreaRead : public MemoryAreaCommand {
  //      Q_OBJECT
    public:
        explicit MemoryAreaRead(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address);
        explicit MemoryAreaRead(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit);
        ~MemoryAreaRead() {}
        void DeserializeReply(QByteArray reply);
    private:
        FINSCommandData ConstructFINSCommandData() const;
        uint8_t MRC_ = 1; // main request code
        uint8_t SRC_ = 1; // sub request code
    };

    // WRITE SUPERCLASS
    class MemoryAreaWrite : public MemoryAreaCommand {
//        Q_OBJECT

    public:
        explicit MemoryAreaWrite(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
        explicit MemoryAreaWrite(FINSHeader header, FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, uint16_t content);
        ~MemoryAreaWrite() {}
        void DeserializeReply(QByteArray reply);
    private:
        FINSCommandData ConstructFINSCommandData() const;
        uint8_t MRC_ = 1; // main request code
        uint8_t SRC_ = 2; // sub request code
    };


}
#endif
