#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <list>
#include <iostream>
#include <fstream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <assert.h>
#include <errno.h>

#include "ast.hh"
#include "base64.hh"
#include "calc.hh"

uint32_t stage1_pre(sat_problem<ast_t> &sat) {
  std::list<std::shared_ptr<ast_t>> queue;

  var_t top_id{(uint16_t)sat.vars.size()};
  queue.push_back(sat.ast);
  while (!queue.empty()) {
    auto ast = queue.front();
    queue.pop_front();
    switch (ast->type) {
    case ast_type::OR:
    case ast_type::AND:
    case ast_type::XOR:
      queue.push_back(ast->ops[1]);
      // Fall through
    case ast_type::NOT:
      queue.push_back(ast->ops[0]);
      ast->id = top_id;
      top_id = top_id.next();
      break;
    case ast_type::VAR:
      break;
    default:
      break;
    }
  }

  return top_id.id;
}

bool stage1_helper(const ast_t &ast, const std::vector<bool> &old_ans,
                            std::vector<bool> &new_ans) {
  bool res;
  switch (ast.type) {
  case ast_type::AND:
    res = stage1_helper(*ast.ops[0], old_ans, new_ans) &
      stage1_helper(*ast.ops[1], old_ans, new_ans);
    break;
  case ast_type::NOT:
    res = !stage1_helper(*ast.ops[0], old_ans, new_ans);
    break;
  case ast_type::OR:
    res = stage1_helper(*ast.ops[0], old_ans, new_ans) |
      stage1_helper(*ast.ops[1], old_ans, new_ans);
    break;
  case ast_type::XOR:
    res = stage1_helper(*ast.ops[0], old_ans, new_ans) ^
      stage1_helper(*ast.ops[1], old_ans, new_ans);
    break;
  case ast_type::VAR:
    res = old_ans[ast.id.id];
    break;
  default:
    assert(false);
  }

  new_ans[ast.id.id] = res;
  return res;
}

// Convert SAT to 3-SAT
void stage1(const sat_problem<ast_t> &sat,
                     sat_problem<three_cnf_t> &sat3)
{
  std::list<std::shared_ptr<ast_t>> queue;
  three_cnf_t cnf_ret;

  // Convert the SAT ast into a 3-SAT thing
  term_t root{ 0, sat.ast->id };
  queue.push_back(sat.ast);
  cnf_ret.push_back({root, root, root});

  while (!queue.empty()) {
    auto ast = queue.front();
    queue.pop_front();

    if (ast->type == ast_type::VAR)
      continue;

    term_t a, a_, b, b_, c, c_;

    a  = term_t{ 0, ast->id };
    a_ = term_t{ 1, ast->id };

    if (ast->ops[0]) {
      b  = term_t{ 0, ast->ops[0]->id };
      b_ = term_t{ 1, ast->ops[0]->id };
    }

    if (ast->ops[1]) {
      c  = term_t{ 0, ast->ops[1]->id };
      c_ = term_t{ 1, ast->ops[1]->id };
    }

    switch (ast->type) {
    // a  b  c | a <-> (b || c)
    // 0  0  0 | 1
    // 0  0  1 | 0  1
    // 0  1  0 | 0  2
    // 0  1  1 | 0  1
    // 1  0  0 | 0  3
    // 1  0  1 | 1
    // 1  1  0 | 1
    // 1  1  1 | 1
    // (a || a || ~c) && (a || ~b || c) && (~a || b || c)
    case ast_type::OR:
      cnf_ret.push_back({{a, a, c_}});
      cnf_ret.push_back({{a, b_, c}});
      cnf_ret.push_back({{a_, b, c}});
      queue.push_back(ast->ops[0]);
      queue.push_back(ast->ops[1]);
      break;
    // a  b  c | a <-> (b && c)
    // 0  0  0 | 1
    // 0  0  1 | 1
    // 0  1  0 | 1
    // 0  1  1 | 0  3
    // 1  0  0 | 0  1
    // 1  0  1 | 0  2
    // 1  1  0 | 0  1
    // 1  1  1 | 1
    // (~a || c || c) && (~a || b || ~c) && (a || ~b || ~c)
    case ast_type::AND:
      cnf_ret.push_back({{a_, c, c}});
      cnf_ret.push_back({{a_, b, c_}});
      cnf_ret.push_back({{a, b_, c_}});
      queue.push_back(ast->ops[0]);
      queue.push_back(ast->ops[1]);
      break;
    // a  b  c | a <-> (b ^ c)
    // 0  0  0 | 1
    // 0  0  1 | 0
    // 0  1  0 | 0
    // 0  1  1 | 1
    // 1  0  0 | 0
    // 1  0  1 | 1
    // 1  1  0 | 1
    // 1  1  1 | 0
    // (a || b || ~c) && (a || ~b || c) && (~a || b || c) && (~a || ~b || ~c)
    case ast_type::XOR:
      cnf_ret.push_back({{a, b, c_}});
      cnf_ret.push_back({{a, b_, c}});
      cnf_ret.push_back({{a_, b, c}});
      cnf_ret.push_back({{a_, b_, c_}});
      queue.push_back(ast->ops[0]);
      queue.push_back(ast->ops[1]);
      break;
    // a  b | a <-> ~b
    // 0  0 | 0
    // 0  1 | 1
    // 1  0 | 1
    // 1  1 | 0
    // (a || b) && (~a || ~b)
    case ast_type::NOT:
      cnf_ret.push_back({{a, a, b}});
      cnf_ret.push_back({{a_, a_, b_}});
      queue.push_back(ast->ops[0]);
      break;
    case ast_type::VAR:
      break;
    }
  }

  // Put into 3-SAT problem, adding the additional satisfiable variables from
  // stage1 helper.
  sat3 = sat_problem<three_cnf_t>(std::make_shared<three_cnf_t>(cnf_ret));
  stage1_helper(*sat.ast, sat.vars, sat3.vars);
}

// Convert 3-SAT to CLIQUE
void stage2(const sat_problem<three_cnf_t> &sat3,
                     vertex_problem &clique) {

  // Create a graph from the 3-SAT problem such that each term represents a
  // vertex, and any two edges connect IIF the two terms do not contradict
  // itself.
  std::unordered_set<edge_t> edges;
  for (uint32_t cnf1 = 0; cnf1 < sat3.ast->size(); cnf1++) {
    for (uint32_t cnf2 = cnf1 + 1; cnf2 < sat3.ast->size(); cnf2++) {
      for (uint32_t term1 = 0; term1 < 3; term1++) {
        for (uint32_t term2 = 0; term2 < 3; term2++) {
          auto ind1 = cnf1 * 3 + term1;
          auto ind2 = cnf2 * 3 + term2;
          auto &a = sat3.ast->at(cnf1).v[term1];
          auto &b = sat3.ast->at(cnf2).v[term2];
          if (a.v.id != b.v.id || a.is_not == b.is_not) {
            edges.insert({ind1, ind2});
          }
        }
      }
    }
  }

  // Construct an answer that models the 3-satisfiablity answer we got.
  std::vector<uint32_t> clique_set;
  for (uint32_t cnf = 0; cnf < sat3.ast->size(); cnf++) {
    for (uint32_t term = 0; term < 3; term++) {
      auto &term_val = sat3.ast->at(cnf).v[term];
      if (sat3.vars[term_val.v.id] ^ term_val.is_not) {
        clique_set.push_back(cnf * 3 + term);
        goto good;
      }
    }

    // Failed to do SAT, so just choose anyone of them
    clique_set.push_back(cnf * 3);
good:;
  }

  clique.graph.verticies = sat3.ast->size() * 3;
  clique.graph.edges = std::move(edges);
  clique.vertex_set = std::move(clique_set);
}

// CLIQUE to VERTEX COVER
void stage3(const vertex_problem &clique, vertex_problem &cover) {
  // Invert the graph
  std::unordered_set<edge_t> edges;
  auto &graph = clique.graph;

  for (uint32_t i = 0; i < graph.verticies; i++) {
    for (uint32_t j = i + 1; j < graph.verticies; j++) {
      if (graph.edges.find({i, j}) == graph.edges.end()) {
        edges.insert({i, j});
      }
    }
  }

  // Invert the vertex set
  std::vector<uint32_t> cover_set;
  std::vector<bool> clique_set(graph.verticies, false);
  clique_set.reserve(clique.vertex_set.size());
  for (auto vertex : clique.vertex_set)
    clique_set[vertex] = true;
  for (uint32_t vertex = 0; vertex < graph.verticies; vertex++) {
    if (!clique_set[vertex])
      cover_set.push_back(vertex);
  }

  // Create cover problem
  simple_graph graph2{ graph.verticies, std::move(edges) };
  cover.graph = std::move(graph2);
  cover.vertex_set = std::move(cover_set);
}

// VERTEX COVER to DOMINATING SET
void stage4(const vertex_problem &cover, vertex_problem &dominating) {
  auto &graph = cover.graph;
  std::unordered_set<edge_t> edges;

  // Insert edge (a_v, a_w) where v != w
  for (uint32_t i = 0; i < graph.verticies; i++)
    for (uint32_t j = i + 1; j < graph.verticies; j++)
      edges.insert({i, j});


  // Insert edge (a_v, b_vw) where (v, w) is an edge
  auto vind = graph.verticies;
  for (auto &edge : graph.edges) {
    if (edge.a > edge.b && edges.find({edge.b, edge.a}) != edges.end())
      continue;

    edges.insert({edge.a, vind});
    edges.insert({edge.b, vind});
    vind++;
  }

  // Create dominating set problem
  simple_graph graph2{ vind, std::move(edges) };
  dominating.graph = std::move(graph2);
  dominating.vertex_set = cover.vertex_set;
}

bool verify(const vertex_problem &dominating) {
  std::vector<bool> used(dominating.graph.verticies, false);
  std::unordered_set<uint32_t> dom_set;

  // Mark all verticies in the set as used
  for (auto v: dominating.vertex_set) {
    dom_set.insert(v);
    used[v] = true;
  }

  // Iterate through all edges
  for (auto &edge: dominating.graph.edges) {
    if (dom_set.find(edge.a) != dom_set.end() ||
        dom_set.find(edge.b) != dom_set.end()) {
      used[edge.a] = true;
      used[edge.b] = true;
    }
  }

  // Check if all verticies are adjacent to the dom set
  for (uint32_t i = 0; i < used.size(); i++)
    if (!used[i]) return false;
  return true;
}

struct serial_graph {
  uint32_t verticies;
  uint32_t edges_num;
  edge_t edges[];

  uint32_t size() {
    return sizeof(*this) + sizeof(edges[0]) * edges_num;
  }

  static serial_graph *create_from(uint32_t verticies, uint32_t edges_num) {
    serial_graph *out = reinterpret_cast<serial_graph*>(
      malloc(sizeof(*out) + sizeof(out->edges[0]) * edges_num));
    *out = {
      .verticies = verticies,
      .edges_num = edges_num
    };
    return out;
  }

};

static std::istream& operator>> (std::istream &in, serial_graph* &out) {
  serial_graph header;
  in.read(reinterpret_cast<char*>(&header), sizeof(header));
  assert(in);
  out = serial_graph::create_from(header.verticies, header.edges_num);
  in.read(reinterpret_cast<char*>(out->edges),
          sizeof(out->edges[0]) * out->edges_num);
  return in;
}

#define ROUNDS 9
#define TOTAL_PROGRESS (ROUNDS * 5 + 1)
const char spinner[] = "|/-\\";
void progress() {
  static int progress_status = 0;

  if (progress_status < TOTAL_PROGRESS) {
    progress_status++;
  }

  std::cout << "[" << spinner[progress_status % 4] << "] Checking [";
  int i;
  for (i = 0; i < progress_status; i++)
    std::cout << "#";
  for (; i < TOTAL_PROGRESS; i++)
    std::cout << ".";
  std::cout << "]\r";
  std::cout.flush();
}

bool round(uint32_t flagbits, base64::iterator &bitstream, serial_graph *graph) {
  ast_prgm prgm;
  auto a = prgm.resv_num(flagbits);
  auto b = prgm.decl_num(flagbits);
  auto cmp = prgm.add_num(32, a, true, b);
  auto neq = prgm.any_bits(32, cmp);
  prgm.seq(!var(neq));


  auto ast = prgm.prgm;
  sat_problem<ast_t> prob(std::move(ast));
  for (auto var: prob.vars)
    var = *(bitstream++);
  stage1_pre(prob);
  progress();

  sat_problem<three_cnf_t> prob2;
  stage1(prob, prob2);
  progress();

  vertex_problem prob3;
  stage2(prob2, prob3);
  progress();

  vertex_problem prob4;
  stage3(prob3, prob4);
  progress();

  vertex_problem prob5;
  stage4(prob4, prob5);
  progress();

  if (prob5.graph.verticies != graph->verticies)
    return false;
  if (prob5.graph.edges.size() != graph->edges_num)
    return false;
  for (int i = 0; i < graph->edges_num; i++) {
    if (prob5.graph.edges.find(graph->edges[i]) == prob5.graph.edges.end())
      return false;
  }

  return verify(prob5);
}

#ifdef DEBUG

const char flag[] = "W0w_NP-C0mpl3t3n3ss_1s_pr33ty_c0ol!!";
//                   ****....****....****....****....****
static_assert(sizeof(flag) == ROUNDS * 4 + 1, "Invalid number of rounds");

serial_graph *generate_round(uint32_t flagbits, base64::iterator &bitstream) {
  ast_prgm prgm;
  auto a = prgm.resv_num(flagbits);
  auto b = prgm.decl_num(flagbits);
  auto cmp = prgm.add_num(32, a, true, b);
  auto neq = prgm.any_bits(32, cmp);
  prgm.seq(!var(neq));

  auto ast = prgm.prgm;
  sat_problem<ast_t> prob(std::move(ast), prgm.sat);
  for (auto var: prgm.sat)
    *(bitstream++) = var;
  stage1_pre(prob);

  sat_problem<three_cnf_t> prob2;
  stage1(prob, prob2);

  vertex_problem prob3;
  stage2(prob2, prob3);

  vertex_problem prob4;
  stage3(prob3, prob4);

  vertex_problem prob5;
  stage4(prob4, prob5);

  assert(verify(prob5));

  auto out = serial_graph::create_from(prob5.graph.verticies,
                                       prob5.graph.edges.size());
  uint32_t i = 0;
  for (auto &edge: prob5.graph.edges)
    out->edges[i++] = edge;
  return out;
}

class generate_job {
private:
  pthread_t thread;
  uint32_t flag32;
  bool joined;

public:
  base64 token;
  serial_graph *out;
  base64::iterator stream;

  explicit generate_job(uint32_t flag32): stream(token.begin()), out(nullptr),
  flag32(flag32), joined(false) {
    auto res = pthread_create(&this->thread, NULL, run, this);
    if (res != 0) {
      errno = res;
      perror("Creating thread");
      exit(1);
    }
  }

  ~generate_job() {
    free(out);
  }

  int join() {
    if (joined) return 0;
    auto res = pthread_join(thread, NULL);
    if (res == 0) joined = true;
    return res;
  }

  static void *run(void *arg) {
    auto info = reinterpret_cast<generate_job*>(arg);
    auto graph = generate_round(info->flag32, info->stream);
    info->out = graph;
    return arg;
  }
};

class generate_res {
private:
  std::ofstream lock_out;
  base64 token;
  base64::iterator stream;

public:
  generate_res(const char *lock_file): lock_out(lock_file), stream(token.begin()) { }

  std::string nonce() const { return token.data; }

  void combine_with(generate_job *job) {
    auto res = job->join();
    if (res != 0) {
      errno = res;
      perror("thread join failed");
      exit(1);
    }
    lock_out.write(reinterpret_cast<const char *>(job->out), job->out->size());
    for (auto itr_in = job->token.begin(); itr_in != job->stream; ++itr_in) {
      *(stream++) = *itr_in;
    }
  }
};

void generate(const char *lock_file) {
  auto *flag32 = reinterpret_cast<const uint32_t*>(flag);
  generate_job *jobs[9];
  generate_res res(lock_file);

  for (int i = 0; i < ROUNDS; i++)
    jobs[i] = new generate_job(flag32[i]);
  for (int i = 0; i < ROUNDS; i++) {
    res.combine_with(jobs[i]);

    std::cerr << "***********" << std::endl;
    std::cerr << "ROUND " << i << std::endl;
    std::cerr << res.nonce() << std::endl;
    std::cerr << "***********" << std::endl;
  }

  std::cout << res.nonce() << std::endl;
}
#endif

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("Usage: %s LOCKFILE\n", program_invocation_short_name);
    return 2;
  }

#ifdef DEBUG
  generate(argv[1]);
  return 0;
#endif

  std::ifstream lock_file(argv[1]);
  if (!lock_file) {
    std::cout << "Cannot open file" << std::endl;
    return 1;
  }

  std::string flag;
  std::string token;
  std::cout << "Not Password (NP) presents: ";
  std::cin >> flag;
  std::cout << "Nonce: ";
  std::cin >> token;

  if (flag.size() != ROUNDS * 4 + 7) {
wrong:
    std::cout << ">> WRONG FORMAT!" << std::endl;
    return 1;
  } else if (flag.substr(0, 6) != "pbctf{") {
    goto wrong;
  } else if (flag[flag.size() - 1] != '}') {
    goto wrong;
  }

  base64 token_b64(token);
  auto token_stream = token_b64.begin();
  auto *flag32 = reinterpret_cast<const uint32_t*>(flag.c_str() + 6);
  for (int i = 0; i < ROUNDS; i++) {
    serial_graph *graph;
    lock_file >> graph;
    if (!round(flag32[i], token_stream, graph)) {
      std::cout << std::endl;
      std::cout << ">> WRONG!" << std::endl;
      return 1;
    }
  }

  std::cout << std::endl;
  std::cout << ">> CORRECT!" << std::endl;
  return 0;
}
