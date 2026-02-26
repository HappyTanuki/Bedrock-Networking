#include "networking/quic/quic_packet.h"

namespace bedrock::network {

bool QuicPacketBase::HeaderForm() const { return data[0] & 0x80; }
std::uint8_t QuicPacketBase::VersionSpecificBits() const {
  return data[0] & 0x7F;
}

std::uint32_t LongQuicPacketHeader::Version() const {
  return *reinterpret_cast<const std::uint32_t*>(span().subspan(1).data());
}
std::uint8_t LongQuicPacketHeader::DestinationConnectionIDLength() const {
  return data[5];
}
std::span<const std::uint8_t> LongQuicPacketHeader::DestinationConnectionID()
    const {
  return span().subspan(6, DestinationConnectionIDLength());
}
std::uint8_t LongQuicPacketHeader::SourceConnectionIDLength() const {
  return data[6 + DestinationConnectionIDLength()];
}
std::span<const std::uint8_t> LongQuicPacketHeader::SourceConnectionID() const {
  return span().subspan(7 + DestinationConnectionIDLength(),
                        SourceConnectionIDLength());
}
std::span<const std::uint8_t> LongQuicPacketHeader::VersionspecificData()
    const {
  return span().subspan(7 + DestinationConnectionIDLength() +
                        SourceConnectionIDLength());
}

std::uint32_t VersionNegotiationQuicPacket::Version() const {
  return *reinterpret_cast<const std::uint32_t*>(data.data() + 1);
}
std::uint8_t VersionNegotiationQuicPacket::DestinationConnectionIDLength()
    const {
  return data[5];
}
std::span<const std::uint8_t>
VersionNegotiationQuicPacket::DestinationConnectionID() const {
  return span().subspan(6, DestinationConnectionIDLength());
}
std::uint8_t VersionNegotiationQuicPacket::SourceConnectionIDLength() const {
  return data[6 + DestinationConnectionIDLength()];
}
std::span<const std::uint8_t> VersionNegotiationQuicPacket::SourceConnectionID()
    const {
  return span().subspan(7 + DestinationConnectionIDLength(),
                        SourceConnectionIDLength());
}
std::span<const std::uint32_t> VersionNegotiationQuicPacket::SupportedVersion()
    const {
  std::size_t offset =
      7 + DestinationConnectionIDLength() + SourceConnectionIDLength();
  return std::span<const std::uint32_t>(
      reinterpret_cast<const std::uint32_t*>(data.data() + offset),
      (data.size() - offset) / 4);
}

}  // namespace bedrock::network