// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "toolchain/lowering/lowering_function_context.h"

#include "toolchain/semantics/semantics_ir.h"

namespace Carbon {

LoweringFunctionContext::LoweringFunctionContext(
    LoweringContext& lowering_context, llvm::Function* function)
    : lowering_context_(&lowering_context),
      function_(function),
      builder_(lowering_context.llvm_context()) {}

auto LoweringFunctionContext::GetBlock(SemIR::NodeBlockId block_id)
    -> llvm::BasicBlock* {
  llvm::BasicBlock*& entry = blocks_[block_id];
  if (!entry) {
    entry = llvm::BasicBlock::Create(llvm_context(), "", function_);
  }
  return entry;
}

auto LoweringFunctionContext::TryToReuseBlock(SemIR::NodeBlockId block_id,
                                              llvm::BasicBlock* block) -> bool {
  if (!blocks_.insert({block_id, block}).second) {
    return false;
  }
  if (block == synthetic_block_) {
    synthetic_block_ = nullptr;
  }
  return true;
}

auto LoweringFunctionContext::GetBlockArg(SemIR::NodeBlockId block_id,
                                          SemIR::TypeId type_id)
    -> llvm::PHINode* {
  llvm::BasicBlock* block = GetBlock(block_id);

  // Find the existing phi, if any.
  auto phis = block->phis();
  if (!phis.empty()) {
    CARBON_CHECK(std::next(phis.begin()) == phis.end())
        << "Expected at most one phi, found "
        << std::distance(phis.begin(), phis.end());
    return &*phis.begin();
  }

  // The number of predecessor slots to reserve.
  static constexpr unsigned NumReservedPredecessors = 2;
  auto* phi = llvm::PHINode::Create(GetType(type_id), NumReservedPredecessors);
  phi->insertInto(block, block->begin());
  return phi;
}

auto LoweringFunctionContext::CreateSyntheticBlock() -> llvm::BasicBlock* {
  synthetic_block_ = llvm::BasicBlock::Create(llvm_context(), "", function_);
  return synthetic_block_;
}

auto LoweringFunctionContext::GetLocalLoaded(SemIR::NodeId node_id)
    -> llvm::Value* {
  auto* value = GetLocal(node_id);
  if (llvm::isa<llvm::AllocaInst, llvm::GetElementPtrInst>(value)) {
    auto* load_type = GetType(semantics_ir().GetNode(node_id).type_id());
    return builder().CreateLoad(load_type, value);
  } else {
    // No load is needed.
    return value;
  }
}

}  // namespace Carbon
