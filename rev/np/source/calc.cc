#include <algorithm>
#include <cassert>

#include "ast.hh"
#include "calc.hh"

template <typename T>
T copy(const T& val) {
  return val;
}

var_t ast_prgm::resv(uint16_t sz) {
  if (sz == 0) return next_pin;
  assert (((uint16_t)sz + next_pin.id) > next_pin.id);

  auto ret = next_pin;
  next_pin = next_pin + sz;
  return ret;
}

#ifdef DEBUG
bool ast_prgm::eval(const std::shared_ptr<ast_t> v) const {
  switch(v->type) {
  case ast_type::AND:
    return eval(v->ops[0]) & eval(v->ops[1]);
  case ast_type::OR:
    return eval(v->ops[0]) | eval(v->ops[1]);
  case ast_type::XOR:
    return eval(v->ops[0]) ^ eval(v->ops[1]);
  case ast_type::NOT:
    return !eval(v->ops[0]);
  case ast_type::VAR:
    return this->sat.at(v->id.id);
  default:
    abort();
  }
}
#endif

var_t ast_prgm::add_num(uint16_t bit_size, var_t var_a, bool is_neg,
                           var_t var_b) {
  var_t carry = is_neg ? TRUE : FALSE;
  auto var_c = resv(bit_size);

#ifdef DEBUG
  for (uint16_t i = 0; i < bit_size; i++)
    this->sat.push_back(false);
#endif

  for (uint16_t i = 0; i < bit_size; i++) {
    auto a = var(var_a + i);
    auto b = is_neg ? !var(var_b + i) : var(var_b + i);
    auto carry_out = (var(carry) && copy(a)) || (var(carry) && copy(b)) ||
      (copy(a) && copy(b));
    auto res = std::move(a) ^ std::move(b) ^ var(carry);

    carry = resv(1);
    seq(var(carry) ^ !carry_out);
#ifdef DEBUG
    this->sat.push_back(eval(std::move(carry_out)));
    this->sat[var_c.id + i] = eval(res);
#endif
    seq(var(var_c + i) ^ !res);
  }
  return var_c;
}

var_t ast_prgm::decl_bit(bool val) {
  auto lbl = resv(1);
#ifdef DEBUG
  this->sat.push_back(val);
#endif
  seq(val ? var(lbl) : !var(lbl));
  return lbl;
}

var_t ast_prgm::all_bits(uint16_t bit_size, var_t v) {
  auto res = resv(1);
  if (bit_size == 0) return res;

  auto tmp = var(v);
  for (uint16_t i = 1; i < bit_size; i++)
    tmp = std::move(tmp) && var(v + i);
#ifdef DEBUG
  this->sat.push_back(eval(tmp));
#endif
  seq(var(res) ^ !std::move(tmp));
  return res;
}

var_t ast_prgm::any_bits(uint16_t bit_size, var_t v) {
  auto res = resv(1);
  if (bit_size == 0) return res;

  auto tmp = var(v);
  for (uint16_t i = 1; i < bit_size; i++)
    tmp = std::move(tmp) || var(v + i);
#ifdef DEBUG
  this->sat.push_back(eval(tmp));
#endif
  seq(var(res) ^ !std::move(tmp));
  return res;
}
