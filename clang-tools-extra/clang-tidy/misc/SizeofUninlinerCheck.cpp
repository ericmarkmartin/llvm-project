//===--- SizeofUninlinerCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//


#include "SizeofUninlinerCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void SizeofUninlinerCheck::pascalCase(std::string &str)  {
  char last = ' ';
  for (auto it = str.begin(); it != str.end(); it++) {
      if (last == ' ' && *it != ' ' && ::isalpha(*it)) {
          *it = ::toupper(*it);
          if (it != str.begin())
            str.erase(it - 1);
      }
      last = *it;
  }
}

void SizeofUninlinerCheck::registerMatchers(MatchFinder *Finder) {
  auto sizeofCall = sizeOfExpr(unaryExprOrTypeTraitExpr().bind("x"));
  Finder->addMatcher(sizeofCall, this);
}

void SizeofUninlinerCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedSizeof = Result.Nodes.getNodeAs<UnaryExprOrTypeTraitExpr>(
    "x");
  auto argTypeName = MatchedSizeof->getTypeOfArgument().getAsString();
  pascalCase(argTypeName);

  auto start = MatchedSizeof->getBeginLoc();
  auto end = start.getLocWithOffset(strlen("sizeof") - 1);
  SourceRange sizeofFnRange(start, end);
  auto newOperatorName = "sizeOf" + argTypeName;

  start = end.getLocWithOffset(2);
  end = MatchedSizeof->getEndLoc().getLocWithOffset(-1);
  SourceRange argumentRange(start, end);
  auto newArgName = "my" + argTypeName;

  diag(MatchedSizeof->getOperatorLoc(),
       "use of sizeof will not be visible from llvm ir")
      << FixItHint::CreateReplacement(sizeofFnRange, newOperatorName)
      << FixItHint::CreateReplacement(argumentRange, newArgName);
}

} // namespace misc
} // namespace tidy
} // namespace clang
