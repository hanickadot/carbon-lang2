// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/semantics/semantics_node_kind.h"

namespace Carbon {

CARBON_DEFINE_ENUM_CLASS_NAMES(SemanticsNodeKind) = {
#define CARBON_SEMANTICS_NODE_KIND(Name) CARBON_ENUM_CLASS_NAME_STRING(Name)
#include "toolchain/semantics/semantics_node_kind.def"
};

// Returns the name to use for this node kind in Semantics IR.
[[nodiscard]] auto SemanticsNodeKind::ir_name() const -> llvm::StringRef {
  static constexpr llvm::StringRef Table[] = {
#define CARBON_SEMANTICS_NODE_KIND_WITH_IR_NAME(Name, IR_Name) IR_Name,
#include "toolchain/semantics/semantics_node_kind.def"
  };
  return Table[AsInt()];
}

auto SemanticsNodeKind::value_kind() const -> SemIR::NodeValueKind {
  static constexpr SemIR::NodeValueKind Table[] = {
#define CARBON_SEMANTICS_NODE_KIND_WITH_VALUE_KIND(Name, Kind) \
  SemIR::NodeValueKind::Kind,
#include "toolchain/semantics/semantics_node_kind.def"
  };
  return Table[AsInt()];
}

auto SemanticsNodeKind::terminator_kind() const -> SemIR::TerminatorKind {
  static constexpr SemIR::TerminatorKind Table[] = {
#define CARBON_SEMANTICS_NODE_KIND_WITH_TERMINATOR_KIND(Name, Kind) \
  SemIR::TerminatorKind::Kind,
#include "toolchain/semantics/semantics_node_kind.def"
  };
  return Table[AsInt()];
}

}  // namespace Carbon
