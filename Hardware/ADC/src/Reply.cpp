#include "Reply.h"
#include "ADCUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

Reply::Reply(QByteArray data) :
    data_(data) {}

bool Reply::IsValid() const {
    if (!CorrectSize()) {
        return false;
    }
    bool header_ok(data_.at(0) == '$' && data_.at(2) == '=');
    bool crc_ok(util::CheckSum(Header() + Body()) == CheckSum());
    return (header_ok && crc_ok);
}

QByteArray Reply::Header() const {
    if (CorrectSize()) {
        return data_.mid(0, 3);
    } else {
        return QByteArray("");
    }
}

ReplyType Reply::Type() const {
    if (CorrectSize()) {
        switch (data_.at(1)) {
        case 'A':
            return ReplyType::A;
            break;
        case 'E':
            return ReplyType::E;
            break;
        case 'F':
            return ReplyType::F;
            break;
        case 'G':
            return ReplyType::G;
            break;
        case 'H':
            return ReplyType::H;
            break;
        case 'I':
            return ReplyType::I;
            break;
        case 'J':
            return ReplyType::J;
            break;
        case 'K':
            return ReplyType::K;
            break;
        case 'L':
            return ReplyType::L;
            break;
        case 'U':
            return ReplyType::U;
            break;
        default:
            return ReplyType::UNKNOWN;
            break;
        }
    } else {
        return ReplyType::UNKNOWN;
    }
}

QByteArray Reply::Body() const {
    if (CorrectSize()) {
        return data_.mid(3, data_.size() - Header().size() - CheckSum().size() - Footer().size());
    } else {
        return QByteArray("");
    }
}

QByteArray Reply::Footer() const {
    if (CorrectSize()) {
        return data_.right(3);
    } else {
        return QByteArray("");
    }
}

QByteArray Reply::CheckSum() const {
    if (CorrectSize()) {
        const int checksum_size(2);
        return data_.mid(data_.length() - Footer().size() - checksum_size, checksum_size);
    } else {
        return QByteArray("");
    }
}

bool Reply::CorrectSize() const {
    return (data_.size() == 13);
}

}
