#ifndef BEDROCK_NETWORKING_NETWORKING_QUIC_QUIC_PACKET_H_
#define BEDROCK_NETWORKING_NETWORKING_QUIC_QUIC_PACKET_H_

#include <cstdint>
#include <span>
#include <vector>

namespace bedrock::network {

struct QuicPacketRawData {
 public:
  std::vector<std::uint8_t> data;

  std::span<const uint8_t> span() const { return {data.data(), data.size()}; }
};

struct QuicPacketBase : public QuicPacketRawData {
 public:
  bool HeaderForm() const;
  std::uint8_t VersionSpecificBits() const;
};

// QUIC Packet Header Types
// refer to rfc8999

// Long Header Packet {
//   Header Form (1) = 1,
//   Version-Specific Bits (7),
//   Version (32),
//   Destination Connection ID Length (8),
//   Destination Connection ID (0..2040),
//   Source Connection ID Length (8),
//   Source Connection ID (0..2040),
//   Version-Specific Data (..),
// }
struct QuicLongPacketHeader : public QuicPacketBase {
 public:
  // bool HeaderForm() const;
  // std::uint8_t VersionSpecificBits() const;

  std::uint32_t Version() const;
  std::uint8_t DestinationConnectionIDLength() const;
  std::span<const std::uint8_t> DestinationConnectionID() const;
  std::uint8_t SourceConnectionIDLength() const;
  std::span<const std::uint8_t> SourceConnectionID() const;
  std::span<const std::uint8_t> VersionspecificData() const;
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
struct QuicVersionNegotiationPacket : public QuicPacketBase {
 public:
  // bool HeaderForm() const;
  // std::uint8_t VersionSpecificBits() const;

  std::uint32_t Version() const;
  std::uint8_t DestinationConnectionIDLength() const;
  std::span<const std::uint8_t> DestinationConnectionID() const;
  std::uint8_t SourceConnectionIDLength() const;
  std::span<const std::uint8_t> SourceConnectionID() const;
  std::span<const std::uint32_t> SupportedVersion() const;
};

// Short Header Packet {
//   Header Form (1) = 0,
//   Version-Specific Bits (7),
//   Destination Connection ID (..),
//   Version-Specific Data (..),
// }
struct QuicShortPacketHeader : public QuicPacketBase {
 public:
  virtual ~QuicShortPacketHeader();
  // bool HeaderForm() const;
  // std::uint8_t VersionSpecificBits() const;

  virtual std::span<const std::uint8_t> DestinationConnectionID() const = 0;
  virtual std::span<const std::uint8_t> VersionspecificData() const = 0;
};

}  // namespace bedrock::network

#endif
