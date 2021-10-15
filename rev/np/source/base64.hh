#ifndef _BASE64_HH
#define _BASE64_HH

#include <string>

class base64 {
public:
  std::string data;

  class b64char {
  private:
    char * const c;
  public:
    b64char(char *const c): c(c) {}

    b64char& operator=(char val);

    b64char& operator=(const b64char &val) { *c = *val.c; return *this; }

    operator char() const;

  };

  class b64bit {
  private:
    b64char ref;
    int bit_ind;
  public:
    b64bit(b64char ref, int bit_ind): ref(ref), bit_ind(bit_ind) {}

    b64bit& operator=(bool val);

    b64bit& operator=(const b64bit &val) { return *this = (bool)val; }

    operator bool() const { return (ref >> bit_ind) & 1; }
  };

  class iterator: public std::iterator<
                  std::input_iterator_tag,
                  bool, std::ptrdiff_t,
                  b64bit*, b64bit> {
  private:
    base64 &outer;
    uint32_t ind;
    uint32_t bit_ind;
  public:
    explicit iterator(base64 &outer, uint32_t ind, uint32_t bit_ind = 0):
      outer(outer), bit_ind(bit_ind), ind(ind) {}

    iterator& operator++();
    iterator operator++(int);

    std::ptrdiff_t operator-(const iterator &other) const {
      return (ind - other.ind) * 6 + bit_ind - other.bit_ind;
    }

    bool operator==(iterator other) const {
      return ind == other.ind && bit_ind == other.bit_ind;
    }
    bool operator!=(iterator other) const {return !(*this == other);}

    reference operator*();
  };

  base64(): data("") {}

  explicit base64(std::string data): data(data) {}

  iterator begin() { return iterator(*this, 0); }

  iterator end() { return iterator(*this, data.size()); }
};

#endif
