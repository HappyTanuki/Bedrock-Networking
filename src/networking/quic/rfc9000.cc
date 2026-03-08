#include "networking/quic/rfc9000.h"

#include <span>

#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif
#include <cstring>

namespace bedrock::network {

bool QuicLongPacketHeaderV1::FixedBit() const { return 0b1 & (data[0] >> 6); }
std::uint8_t QuicLongPacketHeaderV1::LongPacketType() const {
  return 0b11 & (data[0] >> 4);
}
std::uint8_t QuicLongPacketHeaderV1::TypeSpecificBits() const {
  return 0b1111 & data[0];
}
std::span<const std::uint8_t> QuicLongPacketHeaderV1::TypeSpecificPayload()
    const {
  return VersionspecificData();
}

std::uint8_t QuicVersionNegotiationPacketV1::Unused() const {
  return VersionSpecificBits();
}

namespace QuicVariableIntegerV1Util {
static std::uint8_t Encode(std::uint8_t value);
static std::uint16_t Encode(std::uint16_t value);
static std::uint32_t Encode(std::uint32_t value);
static std::uint64_t Encode(std::uint64_t value);

static std::uint8_t Decode6(std::span<std::uint8_t> value);
static std::uint16_t Decode14(std::span<std::uint8_t> value);
static std::uint32_t Decode30(std::span<std::uint8_t> value);
static std::uint64_t Decode62(std::span<std::uint8_t> value);
};  // namespace QuicVariableIntegerV1Util

QuicVariableIntegerV1& QuicVariableIntegerV1::operator=(
    std::uint8_t rhs) noexcept {
  data[0] = rhs;
  data[0] &= 0b00111111;
  data[0] |= static_cast<std::uint8_t>(QuicIntegerTypeV1::kU6B) << 6;
  return *this;
}
QuicVariableIntegerV1& QuicVariableIntegerV1::operator=(
    std::uint16_t rhs) noexcept {
  rhs = htons(rhs);
  ::memcpy(data.data(), &rhs, 2);

  data[0] &= 0b00111111;
  data[0] |= static_cast<std::uint8_t>(QuicIntegerTypeV1::kU14B) << 6;
  return *this;
}
QuicVariableIntegerV1& QuicVariableIntegerV1::operator=(
    std::uint32_t rhs) noexcept {
  rhs = htonl(rhs);
  ::memcpy(data.data(), &rhs, 4);

  data[0] &= 0b00111111;
  data[0] |= static_cast<std::uint8_t>(QuicIntegerTypeV1::kU30B) << 6;
  return *this;
}

inline std::uint64_t bswap64(std::uint64_t x) {
#if defined(_MSC_VER)
  return _byteswap_uint64(x);
#else
  return __builtin_bswap64(x);
#endif
}
std::uint64_t htonll(std::uint64_t x) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return bswap64(x);
#else
  return x;
#endif
}
std::uint64_t ntohll(std::uint64_t x) { return htonll(x); }

QuicVariableIntegerV1& QuicVariableIntegerV1::operator=(
    std::uint64_t rhs) noexcept {
  rhs = htonll(rhs);
  ::memcpy(data.data(), &rhs, 8);

  data[0] &= 0b00111111;
  data[0] |= static_cast<std::uint8_t>(QuicIntegerTypeV1::kU62B) << 6;
  return *this;
}

QuicVariableIntegerV1::operator std::uint8_t() const noexcept {
  std::uint8_t temp = data[0] & 0x3F;
  return temp;
}
QuicVariableIntegerV1::operator std::uint16_t() const noexcept {
  std::uint16_t temp = 0;
  ::memcpy(&temp, data.data(), 2);
  temp = ntohs(temp);
  temp &= 0x3FFF;
  return temp;
}
QuicVariableIntegerV1::operator std::uint32_t() const noexcept {
  std::uint32_t temp = 0;
  ::memcpy(&temp, data.data(), 4);
  temp = ntohl(temp);
  temp &= 0x3FFFFFFF;
  return temp;
}
QuicVariableIntegerV1::operator std::uint64_t() const noexcept {
  std::uint64_t temp = 0;
  ::memcpy(&temp, data.data(), 8);
  temp = ntohll(temp);
  temp &= 0x3FFFFFFFFFFFFFFF;
  return temp;
}

std::span<const std::uint8_t> QuicVariableIntegerV1::GetValue() const noexcept {
  std::span<const std::uint8_t> span = data;

  switch (GetType()) {
    case QuicIntegerTypeV1::kU6B:
      span = span.subspan(0, 1);
      break;
    case QuicIntegerTypeV1::kU14B:
      span = span.subspan(0, 2);
      break;
    case QuicIntegerTypeV1::kU30B:
      span = span.subspan(0, 4);
      break;
    case QuicIntegerTypeV1::kU62B:
      span = span.subspan(0, 8);
      break;
  }

  return span;
}
void QuicVariableIntegerV1::SetValue(std::span<std::uint8_t> value,
                                     std::size_t& size) noexcept {
  if (value.size() < 1) {
    size = 1;
    return;
  } else if (value.size() >= 1 && size == 0) {
    switch (static_cast<QuicIntegerTypeV1>(value[0] >> 6)) {
      case QuicIntegerTypeV1::kU6B:
        size = 1;
        return;
      case QuicIntegerTypeV1::kU14B:
        size = 2;
        return;
      case QuicIntegerTypeV1::kU30B:
        size = 4;
        return;
      case QuicIntegerTypeV1::kU62B:
        size = 8;
        return;
    }
  }

  ::memcpy(data.data(), value.data(), 1 << (value[0] >> 6));
  return;
}

}  // namespace bedrock::network
