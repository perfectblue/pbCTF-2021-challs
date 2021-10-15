#ifndef _AST_HH
#define _AST_HH

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

enum class ast_type {
  VAR, NOT, AND, XOR, OR
};

struct var_t {
  uint16_t id;

  var_t next() {
    return *this + (uint16_t)1;
  }

  var_t operator+(uint16_t incr) const {
    return var_t{ (uint16_t)(id + incr) };
  }

  var_t& operator+=(uint16_t incr) {
    this->id += incr;
    return *this;
  }
};

struct term_t {
  uint16_t is_not;
  var_t v;
};

template <typename Ast>
struct sat_problem {
  std::shared_ptr<Ast> ast;
  std::vector<bool> vars;

  sat_problem() {
    ast = std::make_shared<Ast>(Ast());
  }

  sat_problem(std::shared_ptr<Ast> &&ast, const std::vector<bool> &vars =
              std::vector<bool>()) : ast(std::move(ast)), vars(vars) {
    this->vars.resize(this->ast->vars_count(), false);
  }
};

struct ast_t {
  var_t id;
  ast_type type;
  std::shared_ptr<ast_t> ops[2];

  ast_t(var_t id, ast_type type) :
    ast_t(id, type, std::shared_ptr<ast_t>(), std::shared_ptr<ast_t>()) {
  }

  ast_t(var_t id, ast_type type, std::shared_ptr<ast_t> &&op1) :
    ast_t(id, type, std::move(op1), std::shared_ptr<ast_t>()){
  }

  ast_t(var_t id, ast_type type, std::shared_ptr<ast_t> &&op1,
        std::shared_ptr<ast_t> &&op2) : id(id), type(type),
        ops{std::move(op1), std::move(op2)} {
  }

  uint32_t vars_count() const;

  void dump(int level = 0) const;
};

std::shared_ptr<ast_t> var(var_t id);
std::shared_ptr<ast_t> operator&&(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator||(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator^(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator!(std::shared_ptr<ast_t> &&a);
std::shared_ptr<ast_t> operator&&(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator||(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator^(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b);
std::shared_ptr<ast_t> operator!(const std::shared_ptr<ast_t> &a);

struct three_var_t {
  term_t v[3];
};

struct three_cnf_t : public std::vector<three_var_t> {
  uint32_t vars_count() {
    int max = 0;
    for (auto cnf : *this) {
      max = std::max(std::max(max, cnf.v[0].v.id + 1),
                     std::max(cnf.v[1].v.id + 1, cnf.v[2].v.id + 1));
    }
    return max;
  }
};

struct edge_t {
  uint32_t a;
  uint32_t b;

  size_t operator==(const edge_t &other) const {
    return this->a == other.a && this->b == other.b;
  }
};

namespace std {
  template <>
  class hash<edge_t> {
public:
    size_t operator()(const edge_t &edge) const {
      return std::hash<uint64_t>{}((uint64_t)edge.a << 32 | edge.b);
    }
  };
}

struct simple_graph {
  uint32_t verticies;
  std::unordered_set<edge_t> edges;
};

struct vertex_problem {
  simple_graph graph;
  std::vector<uint32_t> vertex_set;
};

namespace std {
  string to_string(const var_t &var);
  string to_string(const ast_type &type);
  string to_string(const ast_t &obj);
  string to_string(const three_cnf_t &obj);
  string to_string(const simple_graph &g, const std::string &tab="");
  string to_string(const vertex_problem &prob);

  template <typename Ast>
  string to_string(const sat_problem<Ast> &obj) {
    string ret;
    ret += "Problem: " + to_string(*obj.ast) + "\n";

    ret += "Sat: <";
    for (auto v : obj.vars)
      ret += v ? "1" : "0";
    ret += ">\n";
    return ret;
  }
}


#endif
