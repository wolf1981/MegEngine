/**
 * \file dnn/src/cuda/deformable_conv/bwd_flt/algo.h
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include "megdnn/oprs.h"

#include "src/common/utils.h"
#include "src/cuda/handle.h"

#include "src/cuda/deformable_conv/opr_impl.h"

namespace megdnn {
namespace cuda {

class DeformableConvBackwardFilterImpl::AlgoBase : public Algorithm {
protected:
    ~AlgoBase() = default;

public:
    struct SizeArgs {
        DeformableConvBackwardFilterImpl* opr;
        HandleImpl* handle;
        const TensorLayout& im_layout;
        const TensorLayout& offset_layout;
        const TensorLayout& mask_layout;
        const TensorLayout& out_grad_layout;
        CanonizedFilterMeta filter_grad_meta;

        std::string to_string() const;

        SizeArgs(DeformableConvBackwardFilterImpl* opr, const TensorLayout& im,
                 const TensorLayout& offset, const TensorLayout& mask,
                 const TensorLayout& out_grad, const TensorLayout& filter_grad);

        SizeArgs(DeformableConvBackwardFilterImpl* opr, const TensorLayout& im,
                 const TensorLayout& offset, const TensorLayout& mask,
                 const TensorLayout& out_grad,
                 const CanonizedFilterMeta& filter_grad_meta);
    };
    struct ExecArgs : public SizeArgs {
        const TensorND im_tensor, offset_tensor, mask_tensor, out_grad_tensor;
        TensorND filter_grad_tensor;
        Workspace workspace;

        ExecArgs(DeformableConvBackwardFilterImpl* opr, _megdnn_tensor_in im,
                 _megdnn_tensor_in offset, _megdnn_tensor_in mask,
                 _megdnn_tensor_in out_grad, _megdnn_tensor_out filter_grad,
                 _megdnn_workspace workspace);
    };
    virtual bool is_available(const SizeArgs& args) const = 0;
    virtual size_t get_workspace_in_bytes(const SizeArgs& args) const = 0;
    virtual void exec(const ExecArgs& args) const = 0;

    bool is_available_wk(const SizeArgs& args, size_t limit) {
        return is_available(args) && get_workspace_in_bytes(args) <= limit;
    }
    bool is_available_reproducible(
            const SizeArgs& args, bool reproducible = true,
            size_t limit = std::numeric_limits<size_t>::max()) {
        return (!reproducible || is_reproducible()) &&
               is_available_wk(args, limit);
    }
    AlgoBase& check_workspace(const SizeArgs& args,
                              const Workspace& workspace) {
        auto req = get_workspace_in_bytes(args);
        megdnn_assert(req <= workspace.size,
                      "deformable_conv bwd_flt algo %s: required workspace %zu "
                      "bytes, got %zu",
                      name(), req, workspace.size);
        return *this;
    }
};

class DeformableConvBackwardFilterImpl::AlgoMatmul final : public AlgoBase {
private:
    static void get_matmul_layout(const SizeArgs& args, TensorLayout& al,
                                  TensorLayout& bl, TensorLayout& cl);
    static WorkspaceBundle get_bundle(const SizeArgs& args);

public:
    AlgoMatmul() {}

    bool is_available(const SizeArgs& args) const override;
    size_t get_workspace_in_bytes(const SizeArgs& args) const override;
    void exec(const ExecArgs& args) const override;

    bool is_reproducible() const override { return true; }

    const char* name() const override { return "AlgoMatmul"; }
};

class DeformableConvBackwardFilterImpl::AlgoPack {
    AlgoPack(const AlgoPack&) = delete;
    AlgoPack& operator=(const AlgoPack&) = delete;

public:
    AlgoPack();

    AlgoMatmul algo_matmul;
    //! all algorithms
    std::vector<AlgoBase*> all_algos;
};

}  // namespace cuda
}  // namespace megdnn

// vim: syntax=cpp.doxygen
