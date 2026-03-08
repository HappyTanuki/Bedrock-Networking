#include <iostream>
#include <vector>

#include "networking/quic/rfc9000.h"

int main() {
  bedrock::network::QuicVariableIntegerV1 variable_integer;

  std::vector<std::uint8_t> data(1);
  std::span<const std::uint8_t> stored_val;
  std::size_t size = 0;

  // 6-Bit test
  {
    std::uint8_t test6 = 0;
    std::uint8_t response6 = 0;

    test6 = 10;

    variable_integer = test6;
    stored_val = variable_integer.GetValue();
    response6 = variable_integer;

    if (test6 != response6) {
      std::cout << "mismatch" << std::endl;
      return -1;
    }

    test6 = 65;

    variable_integer = test6;
    stored_val = variable_integer.GetValue();
    response6 = variable_integer;

    if (test6 == response6) {
      std::cout << "match" << std::endl;
      return -1;
    }
  }
  // 14-Bit test
  {
    std::uint16_t test14 = 0;
    std::uint16_t response14 = 0;

    test14 = 10;

    variable_integer = test14;
    stored_val = variable_integer.GetValue();
    response14 = variable_integer;

    if (test14 != response14) {
      std::cout << "mismatch" << std::endl;
      return -1;
    }

    test14 = 16385;

    variable_integer = test14;
    stored_val = variable_integer.GetValue();
    response14 = variable_integer;

    if (test14 == response14) {
      std::cout << "match" << std::endl;
      return -1;
    }
  }
  // 30-Bit test
  {
    std::uint32_t test30 = 0;
    std::uint32_t response30 = 0;

    test30 = 10;

    variable_integer = test30;
    stored_val = variable_integer.GetValue();
    response30 = variable_integer;

    if (test30 != response30) {
      std::cout << "mismatch" << std::endl;
      return -1;
    }

    test30 = 1073741825;

    variable_integer = test30;
    stored_val = variable_integer.GetValue();
    response30 = variable_integer;

    if (test30 == response30) {
      std::cout << "match" << std::endl;
      return -1;
    }
  }
  // 62-Bit test
  {
    std::uint64_t test62 = 0;
    std::uint64_t response62 = 0;

    test62 = 10;

    variable_integer = test62;
    stored_val = variable_integer.GetValue();
    response62 = variable_integer;

    if (test62 != response62) {
      std::cout << "mismatch" << std::endl;
      return -1;
    }

    test62 = 4611686018427387905;

    variable_integer = test62;
    stored_val = variable_integer.GetValue();
    response62 = variable_integer;

    if (test62 == response62) {
      std::cout << "match" << std::endl;
      return -1;
    }
  }

  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b00000000;
  size = 0;
  variable_integer.SetValue(data, size);
  data.resize(size);
  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b00000000;
  variable_integer.SetValue(data, size);
  stored_val = variable_integer.GetValue();

  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b01000000;
  size = 0;
  variable_integer.SetValue(data, size);
  data.resize(size);
  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b01000000;
  variable_integer.SetValue(data, size);
  stored_val = variable_integer.GetValue();

  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b10000000;
  size = 0;
  variable_integer.SetValue(data, size);
  data.resize(size);
  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b10000000;
  variable_integer.SetValue(data, size);
  stored_val = variable_integer.GetValue();

  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b11000000;
  size = 0;
  variable_integer.SetValue(data, size);
  data.resize(size);
  data[0] = 0;
  data[0] &= 0x3F;
  data[0] |= 0b11000000;
  variable_integer.SetValue(data, size);
  stored_val = variable_integer.GetValue();

  return 0;
}
