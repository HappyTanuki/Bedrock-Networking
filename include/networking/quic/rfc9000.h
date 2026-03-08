#ifndef BEDROCK_NETWORKING_NETWORKING_QUIC_RFC9000_H_
#define BEDROCK_NETWORKING_NETWORKING_QUIC_RFC9000_H_

#include <array>

#include "quic_packet.h"

namespace bedrock::network {

// Long Header Packet {
//   Header Form (1) = 1,
//   Fixed Bit (1) = 1,
//   Long Packet Type (2),
//   Type-Specific Bits (4),
//   Version (32),
//   Destination Connection ID Length (8),
//   Destination Connection ID (0..160),
//   Source Connection ID Length (8),
//   Source Connection ID (0..160),
//   Type-Specific Payload (..),
// }
struct QuicLongPacketHeaderV1 : public QuicLongPacketHeader {
 public:
  // bool HeaderForm() const;

  // If this value is not true, then this packet is invalid and MUST be
  // discarded.
  bool FixedBit() const;
  std::uint8_t LongPacketType() const;
  std::uint8_t TypeSpecificBits() const;
  // std::uint32_t Version() const;
  // std::uint8_t DestinationConnectionIDLength() const;
  // std::span<const std::uint8_t> DestinationConnectionID() const;
  // std::uint8_t SourceConnectionIDLength() const;
  // std::span<const std::uint8_t> SourceConnectionID() const;

  std::span<const std::uint8_t> TypeSpecificPayload() const;
};

// Version Negotiation Packet {
//   Header Form (1) = 1,
//   Unused (7),
//   Version (32) = 0,
//   Destination Connection ID Length (8),
//   Destination Connection ID (0..2040),
//   Source Connection ID Length (8),
//   Source Connection ID (0..2040),
//   Supported Version (32) ...,
// }
// No difference but VersionSpecificBits() is unused
struct QuicVersionNegotiationPacketV1 : public QuicVersionNegotiationPacket {
 public:
  // bool HeaderForm() const;

  std::uint8_t Unused() const;
  // std::uint32_t Version() const;
  // std::uint8_t DestinationConnectionIDLength() const;
  // std::span<const std::uint8_t> DestinationConnectionID() const;
  // std::uint8_t SourceConnectionIDLength() const;
  // std::span<const std::uint8_t> SourceConnectionID() const;
  // std::span<const std::uint32_t> SupportedVersion() const;
};

// Initial Packet {
//   Header Form (1) = 1,
//   Fixed Bit (1) = 1,
//   Long Packet Type (2) = 0,
//   Reserved Bits (2),
//   Packet Number Length (2),
//   Version (32),
//   Destination Connection ID Length (8),
//   Destination Connection ID (0..160),
//   Source Connection ID Length (8),
//   Source Connection ID (0..160),
//   Token Length (i),
//   Token (..),
//   Length (i),
//   Packet Number (8..32),
//   Packet Payload (8..),
// }
struct QuicInitialPacketV1 : public QuicLongPacketHeaderV1 {
 public:
  // bool HeaderForm() const;
  // bool FixedBit() const;
  // std::uint8_t LongPacketType() const;

  std::uint8_t ReservedBits() const;
  std::uint8_t PacketNumberLength() const;
  // std::uint32_t Version() const;
  // std::uint8_t DestinationConnectionIDLength() const;
  // std::span<const std::uint8_t> DestinationConnectionID() const;
  // std::uint8_t SourceConnectionIDLength() const;
  // std::span<const std::uint8_t> SourceConnectionID() const;

  // std::span<const std::uint8_t> TypeSpecificPayload() const;
};

// This presents in LSB
enum class QuicStreamTypeV1 {
  kClientBiDirectional = 0x00,
  kServerBiDirectional = 0x01,
  kClientUnDirectional = 0x02,
  kServerUnDirectional = 0x03
};

enum class QuicVersionV1 {
  kVersionNegotiation = 0x00,
  kTLS = 0x01,
};

// This presents in MSB
enum class QuicIntegerTypeV1 {
  kU6B = 0b00,
  kU14B = 0b01,
  kU30B = 0b10,
  kU62B = 0b11
};

struct QuicVariableIntegerV1 {
 public:
  constexpr QuicIntegerTypeV1 GetType() const noexcept {
    return static_cast<QuicIntegerTypeV1>(data[0] >> 6);
  }

  QuicVariableIntegerV1& operator=(std::uint8_t rhs) noexcept;
  QuicVariableIntegerV1& operator=(std::uint16_t rhs) noexcept;
  QuicVariableIntegerV1& operator=(std::uint32_t rhs) noexcept;
  QuicVariableIntegerV1& operator=(std::uint64_t rhs) noexcept;

  operator std::uint8_t() const noexcept;
  operator std::uint16_t() const noexcept;
  operator std::uint32_t() const noexcept;
  operator std::uint64_t() const noexcept;

  std::span<const std::uint8_t> GetValue() const noexcept;
  // if size <=1, then this function would set size to required size given
  // value[0]'s type indicator
  void SetValue(std::span<std::uint8_t> value, std::size_t& size) noexcept;

 private:
  std::array<std::uint8_t, 8> data;
};

enum class QuicLongHeaderPacketTypeV1 {
  kInitial = 0x00,
  k0RTT = 0x01,
  kHandshake = 0x02,
  kRetry = 0x03
};

class QuicStreamStateV1 {};

class QuicSendStateV1 : public QuicStreamStateV1 {};

class QuicSendReadyStateV1 : public QuicSendStateV1 {};
class QuicSendSendStateV1 : public QuicSendStateV1 {};
class QuicSendDataSentStateV1 : public QuicSendStateV1 {};
class QuicSendDataRecvdStateV1 : public QuicSendStateV1 {};
class QuicSendResetSentStateV1 : public QuicSendStateV1 {};
class QuicSendResetRecvdStateV1 : public QuicSendStateV1 {};

class QuicReceiveStateV1 : public QuicStreamStateV1 {};

class QuicReceiveRecvStateV1 : public QuicReceiveStateV1 {};
class QuicReceiveSizeKnownStateV1 : public QuicReceiveStateV1 {};
class QuicReceiveDataRecvdStateV1 : public QuicReceiveStateV1 {};
class QuicReceiveDataReadStateV1 : public QuicReceiveStateV1 {};
class QuicReceiveResetRecvdStateV1 : public QuicReceiveStateV1 {};
class QuicReceiveResetReadStateV1 : public QuicReceiveStateV1 {};

}  // namespace bedrock::network

#endif
