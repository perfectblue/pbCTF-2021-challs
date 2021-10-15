#include <algorithm>
#include <cstddef>
#include <iterator>
#include <iostream>
#include <vector>

#include "base64.hh"

const char base64_url_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '-', '_'
};
//   0    1    2    3    4    5    6    7    8    9

base64::b64char& base64::b64char::operator=(char val) {
  *c = (val >= 0 && val < 64) ? base64_url_alphabet[val] : 'A';
  return *this;
}

base64::b64char::operator char() const {
  for (char i = 0; i < 64; i++) {
    if (base64_url_alphabet[i] == *c) return i;
  }
  return 0;
}


base64::b64bit& base64::b64bit::operator=(bool val) {
  auto bit = 1 << bit_ind;
  char before = ref;
  ref = (char)(val ? (ref | bit) : (ref & ~bit));
  return *this;
}

base64::iterator& base64::iterator::operator++() {
  if (bit_ind >= 5) {
    bit_ind = 0;
    ind++;
  } else {
    bit_ind++;
  }
  return *this;
}

base64::iterator base64::iterator::operator++(int) {
  iterator retval = *this; ++(*this); return retval;
}

base64::iterator::reference base64::iterator::operator*() {
  if (ind >= outer.data.size()) {
    outer.data.resize(ind + 1, 'A');
  }
  return b64bit(b64char(&outer.data.at(ind)), bit_ind);
}
