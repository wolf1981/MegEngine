/**
 * \file dnn/src/cuda/cudnn_wrapper.h
 * MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
 *
 * Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once

#include "megdnn/basic_types.h"
#include "megdnn/oprs/nn.h"
#include "src/cuda/cudnn_with_check.h"

namespace megdnn {
namespace cuda {

/*!
 * \brief get compute_type of convolution operations
 */
cudnnDataType_t get_compute_type_fp16(
        param::Convolution::ComputeMode comp_mode);

class TensorDesc {
    public:
        TensorDesc();
        //! default layout is nchw
        void set(const TensorLayout& layout, const param::Convolution::Format = 
                param::Convolution::Format::NCHW);
        ~TensorDesc();
        cudnnTensorDescriptor_t desc;
};

template <typename Param>
class FilterDesc {
    public:
        FilterDesc();
        void set(const typename ConvolutionBase<Param>::CanonizedFilterMeta &meta);
        ~FilterDesc();
        cudnnFilterDescriptor_t desc;
};

class ConvDesc {
    public:
        ConvDesc();
        void set(DType data_type, const param::Convolution& param,
                 const size_t nr_group);
        ~ConvDesc();
        cudnnConvolutionDescriptor_t desc;
};

class PoolingDesc {
    public:
        PoolingDesc();
        void set(const param::Pooling &param);
        ~PoolingDesc();
        cudnnPoolingDescriptor_t desc;
};

class LRNDesc {
    public:
        LRNDesc();
        void set(const param::LRN &param);
        ~LRNDesc();
        cudnnLRNDescriptor_t desc;
};


class BNParamDesc {
    public:
        BNParamDesc();
        void set(const cudnnTensorDescriptor_t xDesc,
                cudnnBatchNormMode_t mode);
        ~BNParamDesc();
        cudnnTensorDescriptor_t desc;
};

// the classes below is used to deal with 3d situations
class Tensor3DDesc {
    public:
        Tensor3DDesc();
        //! default layout is NCDHW
        void set(const TensorLayout &layout, bool is_ndhwc = false);
        ~Tensor3DDesc();
        cudnnTensorDescriptor_t desc;
};

class Filter3DDesc {
    public:
        Filter3DDesc();
        void set(const Convolution3DBase::CanonizedFilterMeta &meta);
        ~Filter3DDesc();
        cudnnFilterDescriptor_t desc;
};

class Conv3DDesc {
    public:
        Conv3DDesc();
        void set(const param::Convolution3D &param, const size_t nr_group);
        ~Conv3DDesc();
        cudnnConvolutionDescriptor_t desc;
};



} // namespace cuda
} // namespace megdnn

// vim: syntax=cpp.doxygen
