#include "networking/quic/quic_packet.h"

namespace bedrock::network {

bool QuicPacketBase::HeaderForm() const { return data[0] & 0x80; }
std::uint8_t QuicPacketBase::VersionSpecificBits() const {
  return data[0] & 0x7F;
}

std::uint32_t QuicLongPacketHeader::Version() const {
  return *reinterpret_cast<const std::uint32_t*>(span().subspan(1).data());
}
std::uint8_t QuicLongPacketHeader::DestinationConnectionIDLength() const {
  return data[5];
}
std::span<const std::uint8_t> QuicLongPacketHeader::DestinationConnectionID()
    const {
  return span().subspan(6, DestinationConnectionIDLength());
}
std::uint8_t QuicLongPacketHeader::SourceConnectionIDLength() const {
  return data[6 + DestinationConnectionIDLength()];
}
std::span<const std::uint8_t> QuicLongPacketHeader::SourceConnectionID() const {
  return span().subspan(7 + DestinationConnectionIDLength(),
                        SourceConnectionIDLength());
}
std::span<const std::uint8_t> QuicLongPacketHeader::VersionspecificData()
    const {
  return span().subspan(7 + DestinationConnectionIDLength() +
                        SourceConnectionIDLength());
}

std::uint32_t QuicVersionNegotiationPacket::Version() const {
  return *reinterpret_cast<const std::uint32_t*>(data.data() + 1);
}
std::uint8_t QuicVersionNegotiationPacket::DestinationConnectionIDLength()
    const {
  return data[5];
}
std::span<const std::uint8_t>
QuicVersionNegotiationPacket::DestinationConnectionID() const {
  return span().subspan(6, DestinationConnectionIDLength());
}
std::uint8_t QuicVersionNegotiationPacket::SourceConnectionIDLength() const {
  return data[6 + DestinationConnectionIDLength()];
}
std::span<const std::uint8_t> QuicVersionNegotiationPacket::SourceConnectionID()
    const {
  return span().subspan(7 + DestinationConnectionIDLength(),
                        SourceConnectionIDLength());
}
std::span<const std::uint32_t> QuicVersionNegotiationPacket::SupportedVersion()
    const {
  std::size_t offset =
      7 + DestinationConnectionIDLength() + SourceConnectionIDLength();
  return std::span<const std::uint32_t>(
      reinterpret_cast<const std::uint32_t*>(data.data() + offset),
      (data.size() - offset) / 4);
}

QuicShortPacketHeader::~QuicShortPacketHeader() = default;

}  // namespace bedrock::network