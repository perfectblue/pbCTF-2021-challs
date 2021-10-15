#include <cassert>
#include <iostream>
#include <set>

#include "ast.hh"

uint32_t ast_t::vars_count() const {
  uint32_t tmp = 0;
  switch (type) {
  case ast_type::AND:
  case ast_type::OR:
  case ast_type::XOR:
    tmp = ops[1]->vars_count();
    // fallthrough
  case ast_type::NOT:
    return std::max(ops[0]->vars_count(), tmp);
  case ast_type::VAR:
    return id.id + 1;
  default:
    return 0;
  }
}

void ast_t::dump(int level) const {
#ifdef DEBUG
  for (int i = 0; i < level; i++) {
    std::cout << "  ";
  }
  std::cout << "op(" << std::to_string(this->type) << "): " <<
    std::to_string(this->id) << std::endl;
  if (this->ops[0]) this->ops[0]->dump(level + 1);
  if (this->ops[1]) this->ops[1]->dump(level + 1);
#endif
}

std::shared_ptr<ast_t> var(var_t id) {
  return std::make_shared<ast_t>(ast_t(id, ast_type::VAR));
}

std::shared_ptr<ast_t> operator&&(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b) {
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::AND, std::move(a), std::move(b)));
}

std::shared_ptr<ast_t> operator||(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b) {
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::OR, std::move(a), std::move(b)));
}

std::shared_ptr<ast_t> operator^(std::shared_ptr<ast_t> &&a,
                                  std::shared_ptr<ast_t> &&b) {
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::XOR, std::move(a), std::move(b)));
}

std::shared_ptr<ast_t> operator!(std::shared_ptr<ast_t> &&a) {
  return std::make_shared<ast_t>(ast_t(var_t{0}, ast_type::NOT, std::move(a)));
}

std::shared_ptr<ast_t> operator&&(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b) {
  std::shared_ptr<ast_t> a2 = a;
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::AND, std::move(a2), std::move(b)));
}

std::shared_ptr<ast_t> operator||(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b) {
  std::shared_ptr<ast_t> a2 = a;
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::OR, std::move(a2), std::move(b)));
}

std::shared_ptr<ast_t> operator^(const std::shared_ptr<ast_t> &a,
                                  std::shared_ptr<ast_t> &&b) {
  std::shared_ptr<ast_t> a2 = a;
  return std::make_shared<ast_t>(ast_t(
      var_t{0}, ast_type::XOR, std::move(a2), std::move(b)));
}

std::shared_ptr<ast_t> operator!(const std::shared_ptr<ast_t> &a) {
  std::shared_ptr<ast_t> a2 = a;
  return std::make_shared<ast_t>(ast_t(var_t{0}, ast_type::NOT, std::move(a2)));
}

namespace std {

#ifdef DEBUG

string to_string(const var_t &var) {
  return "v" + to_string(var.id);
}

string to_string(const ast_type &type) {
  switch (type) {
  case ast_type::AND: return "&&";
  case ast_type::OR:  return "||";
  case ast_type::XOR: return "^";
  case ast_type::NOT: return "!";
  case ast_type::VAR: return "v";
  }
  assert(false);
}

string to_string(const ast_t &obj) {
  const char *op = "&&";
  string operand, operand2;

  switch (obj.type) {
  case ast_type::AND:
    op = "&&";
    goto two_ops;
  case ast_type::OR:
    op = "||";
    goto two_ops;
  case ast_type::XOR:
    op = "^";
two_ops:
    operand = to_string(*obj.ops[0]);
    if (obj.ops[0]->type > obj.type)
      operand = "(" + operand + ")";

    operand2 = to_string(*obj.ops[1]);
    if (obj.ops[1]->type >= obj.type)
      operand2 = "(" + operand2 + ")";

    return operand + " " + op + " " + operand2;
  case ast_type::NOT:
    operand = to_string(*obj.ops[0]);
    if (obj.ops[0]->type > obj.type)
      operand = "(" + operand + ")";
    return "!" + operand;
  case ast_type::VAR:
    return to_string(obj.id);
  }

  assert(false);
}

string to_string(const three_cnf_t &obj) {
  std::string ret;
  for (auto& term : obj) {
    if (!ret.empty()) ret += " && ";
    ret += "(";
    for (int i = 0; i < 3; i++) {
      if (i > 0) ret += " || ";
      if (term.v[i].is_not) ret += "!";
      ret += to_string(term.v[i].v.id);
    }
    ret += ")";
  }

  return ret;
}

string to_string(const simple_graph &g, const std::string &tab) {
  vector<set<uint32_t>> adjlist;
  adjlist.resize(g.verticies);
  for (auto &edge : g.edges) {
    adjlist[edge.a].insert(edge.b);
    adjlist[edge.b].insert(edge.a);
  }

  string ret = tab;
  for (uint32_t a = 0; a < g.verticies; a++) {
    ret += "v" + to_string(a) + ":";
    for (auto &b : adjlist[a]) {
      ret += " v" + to_string(b);
    }
    ret += "\n" + tab;
  }

  return ret;
}

string to_string(const vertex_problem &prob) {
  string ret = "Problem:\n";
  ret += to_string(prob.graph, "  ");
  ret += "Vertex set: <";
  for (auto &v : prob.vertex_set) {
    ret += " v" + to_string(v);
  }
  ret += " >\n";
  return ret;
}

#endif

}
