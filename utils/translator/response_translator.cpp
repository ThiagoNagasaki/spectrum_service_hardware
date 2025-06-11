// utils/response_translator.cpp
#include "response_translator.h"
#include <algorithm>
#include "../../utils/enum_/mcb_port_addresses.h"
#include <cctype>

namespace utils {

// remove espaços e '\0' das extremidades
static std::string trim(const std::string& s) {
    auto first = s.find_first_not_of(" \r\n\t\0");
    if (first == std::string::npos) return "";
    auto last = s.find_last_not_of(" \r\n\t\0");
    return s.substr(first, last - first + 1);
}

std::string ResponseTranslator::asciiToString(const std::vector<uint8_t>& data) {
    std::string out;
    out.reserve(data.size());
    for (auto b : data) {
        if (b == 0) break;
        out.push_back(static_cast<char>(b));
    }
    return trim(out);
}

std::string ResponseTranslator::asciiToString(const std::array<uint8_t,15>& data) {
    std::string out;
    out.reserve(data.size());
    for (auto b : data) {
        if (b == 0) break;
        out.push_back(static_cast<char>(b));
    }
    return trim(out);
}


std::string ResponseTranslator::keyCodeToString(utils::enum_::KeyCode code) {
    using K = utils::enum_::KeyCode;
    switch (code) {
        case K::None:           return "None";
        case K::S1:             return "S1";
        case K::S2:             return "S2";
        case K::S3:             return "S3";
        case K::S4:             return "S4";
        case K::S5:             return "S5";
        case K::S6:             return "S6";
        case K::S7:             return "S7";
        case K::S8:             return "S8";
        case K::S9:             return "S9";
        case K::S10:            return "S10";
        case K::S11:            return "S11";
        case K::S12:            return "S12";
        case K::S13:            return "S13";
        case K::S14:            return "S14";
        case K::S15:            return "S15";
        case K::S16:            return "S16";
        case K::S17:            return "S17";
        case K::S18:            return "S18";
        case K::S19:            return "S19";
        case K::S20:            return "S20";
        case K::S21:            return "S21";
        case K::S22:            return "S22";
        case K::S23:            return "S23";
        case K::S24:            return "S24";
        case K::S25:            return "S25";
        case K::S26:            return "S26";
        case K::S27:            return "S27";
        case K::S28:            return "S28";
        case K::Shift:          return "Shift";
        case K::Ctrl:           return "Ctrl";
        case K::S31:            return "S31";
        case K::S32:            return "S32";
        case K::S33:            return "S33";
        case K::S34:            return "S34";
        case K::S35:            return "S35";
        case K::S36:            return "S36";
        case K::S37:            return "S37";
        case K::S38:            return "S38";
        case K::S39:            return "S39";
        default:                return "UnknownKey";
    }
}

} // namespace utils
