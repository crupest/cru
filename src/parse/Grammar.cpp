#include "cru/parse/Grammar.hpp"
#include "cru/common/String.hpp"
#include "cru/parse/Symbol.hpp"
#include "cru/common/Format.hpp"

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>

namespace cru::parse {
Grammar::Grammar() {}

Grammar::~Grammar() {
  for (auto symbol : symbols_) {
    delete symbol;
  }

  for (auto production : productions_) {
    delete production;
  }
}

void Grammar::SetStartSymbol(Nonterminal* start_symbol) {
  start_symbol_ = start_symbol;
}

Terminal* Grammar::CreateTerminal(String name) {
  auto terminal = new Terminal(this, std::move(name));
  terminals_.push_back(terminal);
  symbols_.push_back(terminal);
  return terminal;
}

Nonterminal* Grammar::CreateNonterminal(String name) {
  auto nonterminal = new Nonterminal(this, std::move(name));
  nonterminals_.push_back(nonterminal);
  symbols_.push_back(nonterminal);
  return nonterminal;
}

Production* Grammar::CreateProduction(String name, Nonterminal* left,
                                      std::vector<Symbol*> right) {
  Expects(left->GetGrammar() == this);
  Expects(std::all_of(right.cbegin(), right.cend(), [this](Symbol* symbol) {
    return symbol->GetGrammar() == this;
  }));

  auto production =
      new Production(this, std::move(name), left, std::move(right));
  productions_.push_back(production);
  return production;
}

bool Grammar::RemoveSymbol(Symbol* symbol) {
  for (auto production : productions_) {
    if (production->GetLeft() == symbol) return false;
    for (auto s : production->GetRight()) {
      if (s == symbol) return false;
    }
  }

  auto i = std::find(symbols_.begin(), symbols_.end(), symbol);

  if (i == symbols_.cend()) return false;

  symbols_.erase(i);

  if (auto t = dynamic_cast<Terminal*>(symbol)) {
    terminals_.erase(std::find(terminals_.begin(), terminals_.end(), t));
  } else if (auto n = dynamic_cast<Nonterminal*>(symbol)) {
    nonterminals_.erase(
        std::find(nonterminals_.begin(), nonterminals_.end(), n));
  }

  delete symbol;

  return true;
}

bool Grammar::RemoveProduction(Production* production) {
  auto i = std::find(productions_.begin(), productions_.end(), production);
  if (i == productions_.cend()) return false;
  productions_.erase(i);
  delete production;
  return true;
}

Grammar* Grammar::Clone() const {
  Grammar* g = new Grammar();

  std::unordered_map<Symbol*, Symbol*> symbol_map;

  for (auto old_terminal : terminals_) {
    auto new_terminal = g->CreateTerminal(old_terminal->GetName());
    symbol_map.emplace(old_terminal, new_terminal);
  }

  for (auto old_nonterminal : nonterminals_) {
    auto new_nonterminal = g->CreateNonterminal(old_nonterminal->GetName());
    symbol_map.emplace(old_nonterminal, new_nonterminal);
  }

  g->SetStartSymbol(static_cast<Nonterminal*>(symbol_map[start_symbol_]));

  for (auto old_production : productions_) {
    std::vector<Symbol*> new_right;
    std::transform(old_production->GetRight().cbegin(),
                   old_production->GetRight().cend(),
                   std::back_inserter(new_right),
                   [&symbol_map](Symbol* old) { return symbol_map[old]; });

    g->CreateProduction(
        old_production->GetName(),
        static_cast<Nonterminal*>(symbol_map[old_production->GetLeft()]),
        std::move(new_right));
  }

  return g;
}

std::unordered_map<Nonterminal*, std::vector<Production*>>
Grammar::GenerateLeftProductionMap() const {
  std::unordered_map<Nonterminal*, std::vector<Production*>> result;
  for (auto p : productions_) {
    result[p->GetLeft()].push_back(p);
  }
  return result;
}

void Grammar::EliminateLeftRecursions() {
  auto nonterminals = nonterminals_;
  for (int i = 0; i < nonterminals.size(); i++) {
    auto ni = nonterminals[i];

    for (int j = 0; j < i; j++) {
      auto nj = nonterminals[j];
      std::vector<Production*> j_productions;
      std::copy_if(
          productions_.cbegin(), productions_.cend(),
          std::back_inserter(j_productions),
          [nj](Production* production) { return production->GetLeft() == nj; });

      auto productions = productions_;
      for (auto production : productions) {
        if (production->GetLeft() == ni &&
            production->GetRight().front() == nj) {
          const std::vector<Symbol*> right(production->GetRight().cbegin() + 1,
                                           production->GetRight().cend());
          RemoveProduction(production);

          for (auto jp : j_productions) {
            auto new_right = right;
            new_right.insert(new_right.cbegin(), jp->GetRight().cbegin(),
                             jp->GetRight().cend());
            CreateProduction(
                Format(u"Merge of {} and {} (Eliminate Left Recursion)",
                       production->GetName(), jp->GetName()),
                ni, std::move(new_right));
          }
        }
      }
    }

    std::vector<Production*> i_r_ps;
    std::vector<Production*> i_nr_ps;

    for (auto p : productions_) {
      if (p->GetLeft() == ni) {
        if (p->GetRight().front() == ni) {
          i_r_ps.push_back(p);
        } else {
          i_nr_ps.push_back(p);
        }
      }
    }

    auto ni_h = CreateNonterminal(ni->GetName() +
                                  u" (Eliminate Left Recursion Helper)");

    for (auto p : i_nr_ps) {
      auto right = p->GetRight();
      right.push_back(ni_h);
      CreateProduction(p->GetName() + u" (Eliminate Left Recursion Of " +
                           ni->GetName() + u")",
                       ni, std::move(right));
    }

    for (auto p : i_r_ps) {
      auto right = p->GetRight();
      right.erase(right.cbegin());
      right.push_back(ni_h);
      CreateProduction(p->GetName() + u" (Eliminate Left Recursion Of " +
                           ni->GetName() + u")",
                       ni_h, std::move(right));
    }

    CreateProduction(u"Empty Production (Eliminate Left Recursion Of " +
                         ni->GetName() + u")",
                     ni_h, std::vector<Symbol*>{});

    for (auto p : i_r_ps) {
      RemoveProduction(p);
    }

    for (auto p : i_nr_ps) {
      RemoveProduction(p);
    }
  }
}

void Grammar::LeftFactor() {
  auto left_production_map = GenerateLeftProductionMap();

  for (auto nonterminal : nonterminals_) {
    auto productions = left_production_map[nonterminal];
    if (productions.size() <= 1) continue;

    while (true) {
      std::unordered_set<Production*> productions_with_longest_common_prefix;
      std::vector<Symbol*> longest_common_prefix;

      for (int i = 0; i < productions.size(); i++) {
        for (int j = i + 1; j < productions.size(); j++) {
          // Get common prefix of the two productions.
          std::vector<Symbol*> common_prefix;
          for (int k = 0; k < std::min(productions[i]->GetRight().size(),
                                       productions[j]->GetRight().size());
               k++) {
            if (productions[i]->GetRight()[k] ==
                productions[j]->GetRight()[k]) {
              common_prefix.push_back(productions[i]->GetRight()[k]);
            } else {
              break;
            }
          }

          if (common_prefix.size() ==
              productions_with_longest_common_prefix.size()) {
            if (std::equal(common_prefix.cend(), common_prefix.cend(),
                           longest_common_prefix.cbegin())) {
              productions_with_longest_common_prefix.insert(productions[i]);
              productions_with_longest_common_prefix.insert(productions[j]);
            }
          } else if (common_prefix.size() >
                     productions_with_longest_common_prefix.size()) {
            longest_common_prefix = common_prefix;
            productions_with_longest_common_prefix.clear();
            productions_with_longest_common_prefix.insert(productions[i]);
            productions_with_longest_common_prefix.insert(productions[j]);
          }
        }
      }

      if (longest_common_prefix.empty()) break;

      for (auto p : productions_with_longest_common_prefix) {
        RemoveProduction(p);
      }

      // Also update local list.
      productions.erase(
          std::remove_if(
              productions.begin(), productions.end(),
              [&productions_with_longest_common_prefix](Production* p) {
                return productions_with_longest_common_prefix.count(p);
              }),
          productions.end());

      auto helper_nonterminal =
          CreateNonterminal(nonterminal->GetName() + u" (Left Factor Helper)");

      for (auto p : productions_with_longest_common_prefix) {
        auto right = p->GetRight();
        right.erase(right.cbegin(),
                    right.cbegin() + longest_common_prefix.size());
        CreateProduction(p->GetName() + u" (Left Factor Helper)",
                         helper_nonterminal, std::move(right));
      }

      auto new_right = longest_common_prefix;
      new_right.push_back(helper_nonterminal);

      CreateProduction(nonterminal->GetName() + u" (Left Factor)", nonterminal,
                       new_right);
    }
  }
}

String Grammar::ProductionsToString() const {
  String result;

  for (const auto& production : productions_) {
    result += production->GetName() + u" : ";
    result += production->GetLeft()->GetName() + u" := ";
    for (auto s : production->GetRight()) {
      result += s->GetName() + u" ";
    }
    result += u"\n";
  }

  return result;
}
}  // namespace cru::parse
