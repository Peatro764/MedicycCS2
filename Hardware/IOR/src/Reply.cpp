#include "Reply.h"
#include "IORUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

Reply::Reply(QString data) :
    data_(data) {}

bool Reply::IsValid() const {
    if (!CorrectSize()) {
        return false;
    }
    bool header_ok(data_.at(0) == '$' && data_.at(2) == '=');
    bool crc_ok(ior_util::CheckSum(Header() + Body()) == CheckSum());
    return (header_ok && crc_ok);
}

QString Reply::Header() const {
    if (CorrectSize()) {
        return data_.mid(0, 3);
    } else {
        return QString("");
    }
}

ReplyType Reply::Type() const {
    if (CorrectSize()) {
        switch (data_.at(1).toLatin1()) {
        case 'A':
            return ReplyType::A;
            break;
        case 'B':
            return ReplyType::B;
            break;
        case 'C':
            return ReplyType::C;
            break;
        case 'D':
            return ReplyType::D;
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
        case 'M':
            return ReplyType::M;
            break;
        case 'N':
            return ReplyType::N;
            break;
        default:
            return ReplyType::UNKNOWN;
            break;
        }
    } else {
        return ReplyType::UNKNOWN;
    }
}

QString Reply::Body() const {
    if (CorrectSize()) {
        return data_.mid(3, data_.size() - Header().size() - CheckSum().size() - Footer().size());
    } else {
        return QString("");
    }
}

QString Reply::Footer() const {
    if (CorrectSize()) {
        return data_.right(3);
    } else {
        return QString("");
    }
}

QString Reply::CheckSum() const {
    if (CorrectSize()) {
        const int checksum_size(2);
        return data_.mid(data_.length() - Footer().size() - checksum_size, checksum_size);
    } else {
        return QString("");
    }
}

bool Reply::CorrectSize() const {
    return (data_.size() == 12);
}

}
