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
#include "clang/Lex/Lexer.h"
#include <iostream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void SizeofUninlinerCheck::registerMatchers(MatchFinder *Finder) {
  auto sizeofCall = sizeOfExpr(unaryExprOrTypeTraitExpr().bind("x"));
  Finder->addMatcher(sizeofCall, this);
}

void SizeofUninlinerCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedSizeof =
      Result.Nodes.getNodeAs<UnaryExprOrTypeTraitExpr>("x");
  const ASTContext &Ctx = *Result.Context;
  auto desugaredArgTypeName =
      MatchedSizeof->getTypeOfArgument().getDesugaredType(Ctx).getAsString();
  SourceRange sizeofRange(MatchedSizeof->getBeginLoc(),
                          MatchedSizeof->getEndLoc());
  std::string sizeofExprString =
      Lexer::getSourceText(CharSourceRange::getTokenRange(sizeofRange),
                           Ctx.getSourceManager(), LangOptions());
  std::string sizeOfCallString =
      "sizeOf(\"" + desugaredArgTypeName + "\", " + sizeofExprString + ")";

  diag(MatchedSizeof->getOperatorLoc(),
       "use of sizeof will not be visible from llvm ir")
      << FixItHint::CreateReplacement(sizeofRange, newFunctionCall3);
}

} // namespace misc
} // namespace tidy
} // namespace clang
