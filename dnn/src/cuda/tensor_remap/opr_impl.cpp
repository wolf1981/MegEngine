/**
 * \file dnn/src/cuda/tensor_remap/opr_impl.cpp
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include "src/cuda/tensor_remap/opr_impl.h"

#include "src/cuda/utils.h"
#include "src/cuda/tensor_remap/tensor_remap.cuh"

namespace megdnn {
namespace cuda {

void IndexingRemapForwardImpl::exec(_megdnn_tensor_in src,
        _megdnn_tensor_in map,
        _megdnn_tensor_out dst,
        _megdnn_workspace workspace)
{
    check_exec(src.layout, map.layout, dst.layout, workspace.size);
    constexpr auto NDIM = TensorShape::MAX_NDIM;
    array_wrapper<int, NDIM> sstride;
    array_wrapper<int, NDIM> dstride;
    array_wrapper<uint32_t, NDIM> dshape;
    // Initialize array_wrappers.
    for (size_t i = 0_z; i < src.layout.ndim; ++i) {
        sstride.data[i] = src.layout.stride[i];
    }
    for (size_t i = 0_z; i < dst.layout.ndim; ++i) {
        dstride.data[i] = dst.layout.stride[i];
    }
    for (size_t i = 0_z; i < dst.layout.ndim; ++i) {
        dshape.data[i] = dst.layout.shape[i];
    }
    // Invoke kernel
    tensor_remap::forward(src.ptr<dt_float32>(),
            map.ptr<dt_int32>(),
            dst.ptr<dt_float32>(),
            src.layout.ndim, dst.layout.ndim,
            sstride, dstride, dshape,
            cuda_stream(handle()));
}

void IndexingRemapBackwardImpl::exec(_megdnn_tensor_in diff,
        _megdnn_tensor_in map,
        _megdnn_tensor_out grad,
        _megdnn_workspace workspace)
{
    check_exec(diff.layout, map.layout, grad.layout, workspace.size);
    constexpr auto NDIM = TensorShape::MAX_NDIM;
    array_wrapper<int, NDIM> sstride;
    array_wrapper<int, NDIM> dstride;
    array_wrapper<uint32_t, NDIM> sshape;
    array_wrapper<uint32_t, NDIM> dshape;
    // Intialize array_wrappers.
    for (size_t i = 0_z; i < grad.layout.ndim; ++i) {
        sstride.data[i] = grad.layout.stride[i];
    }
    for (size_t i = 0_z; i < diff.layout.ndim; ++i) {
        dstride.data[i] = diff.layout.stride[i];
    }
    for (size_t i = 0_z; i < grad.layout.ndim; ++i) {
        sshape.data[i] = grad.layout.shape[i];
    }
    for (size_t i = 0_z; i < diff.layout.ndim; ++i) {
        dshape.data[i] = diff.layout.shape[i];
    }
    // Invoke kernel
    tensor_remap::backward(diff.ptr<dt_float32>(),
            map.ptr<dt_int32>(),
            grad.ptr<dt_float32>(),
            grad.layout.ndim, diff.layout.ndim,
            sstride, dstride, sshape, dshape,
            param().is_non_overlapping,
            cuda_stream(handle()));
}

} // namespace cuda
} // namespace megdnn

// vim: syntax=cpp.doxygen

