#ifndef _CALC_HH
#define _CALC_HH

#include "ast.hh"
#include <cassert>

#define TRUE (var_t{1})
#define FALSE (var_t{0})
#define FIRST_NONRESV (var_t{2})

#endif

struct ast_prgm {
  std::shared_ptr<ast_t> prgm;
  var_t next_pin;
#ifdef DEBUG
  std::vector<bool> sat;
#endif

  ast_prgm() {
    next_pin = FIRST_NONRESV;
    prgm = !var(FALSE) && var(TRUE);
#ifdef DEBUG
    sat.push_back(false);
    sat.push_back(true);
#endif
  }

  void seq(std::shared_ptr<ast_t> &&a) {
    this->prgm = std::move(this->prgm) && std::move(a);
  }


  var_t resv(uint16_t sz);

  template <typename Numb>
  var_t resv_num(Numb val) {
    auto lbl = resv(sizeof(Numb) * 8);
    for (uint16_t i = 0; i < sizeof(Numb) * 8; i++) {
#ifdef DEBUG
      sat.push_back((val >> i) & 1);
#endif
    }
    return lbl;
  }

  template <typename Numb>
  var_t decl_num(Numb val) {
    auto lbl = resv(sizeof(Numb) * 8);
    for (uint16_t i = 0; i < sizeof(Numb) * 8; i++) {
      if ((val >> i) & 1)
        seq(var(lbl + i));
      else
        seq(!var(lbl + i));
#ifdef DEBUG
      sat.push_back((val >> i) & 1);
#endif
    }
    return lbl;
  }

  bool eval(const std::shared_ptr<ast_t> v) const;

  var_t decl_bit(bool val);

  var_t add_num(uint16_t bit_size, var_t var_a, bool is_neg, var_t var_b);

  var_t all_bits(uint16_t bit_size, var_t var);

  var_t any_bits(uint16_t bit_size, var_t var);

};
