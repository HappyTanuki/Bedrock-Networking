#ifndef BEDROCK_NETWORKING_NETWORKING_QUIC_H_
#define BEDROCK_NETWORKING_NETWORKING_QUIC_H_

#include <common/interfaces.h>

namespace bedrock::network {

class Quic : public Validatable {
 public:
  Quic() = default;

  bool IsValid() const override { return valid; }

 private:
  bool valid = false;
};

}  // namespace bedrock::network

#endif