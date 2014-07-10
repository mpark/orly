/* <orly/synth/read_expr.cc>

   Implements <orly/synth/read_expr.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/synth/read_expr.h>

#include <base/assert_true.h>
#include <orly/expr/keys.h>
#include <orly/expr/read.h>
#include <orly/pos_range.h>
#include <orly/synth/context.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/new_type.h>

using namespace Orly;
using namespace Orly::Synth;

TReadExpr::TReadExpr(const TExprFactory *expr_factory, const Package::Syntax::TReadExpr *read_expr)
    : ReadExpr(Base::AssertTrue(read_expr)), Expr(nullptr), Type(nullptr) {
  assert(expr_factory);
  try {
    Expr = expr_factory->NewExpr(ReadExpr->GetExpr());
    Type = NewType(ReadExpr->GetType());
  } catch (...) {
    delete Expr;
    delete Type;
    throw;
  }
}

TReadExpr::~TReadExpr() {
  assert(this);
  delete Expr;
  delete Type;
}

Expr::TExpr::TPtr TReadExpr::Build() const {
  assert(this);
  auto keys_expr_ptr = Expr->Build();
  Type::TType as_type = Type->GetSymbolicType();
  Expr::TKeys *k = dynamic_cast<Expr::TKeys *>(keys_expr_ptr.get());
  /* If read is on a keys expression, check that the keys value type is the same as the as_type of the read expression */
  if (k && k->GetValueType() != as_type) {
    GetContext().AddError(GetPosRange(ReadExpr),
                          "read expression is dereferencing keys who's value type does not match the read's type");
  }
  return Expr::TRead::New(keys_expr_ptr, as_type, GetPosRange(ReadExpr));
}

void TReadExpr::ForEachInnerScope(const std::function<void (TScope *)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachInnerScope(cb);
}

void TReadExpr::ForEachRef(const std::function<void (TAnyRef &)> &cb) {
  assert(this);
  assert(&cb);
  assert(cb);
  Expr->ForEachRef(cb);
  Type->ForEachRef(cb);
}
