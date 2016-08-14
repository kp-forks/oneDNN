/*******************************************************************************
* Copyright 2016 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef CPU_JIT_LRN_HPP
#define CPU_JIT_LRN_HPP

#include <assert.h>

#include "c_types_map.hpp"
#include "type_helpers.hpp"
#include "primitive.hpp"
#include "cpu_engine.hpp"

class xbyak_lrn;

namespace mkldnn { namespace impl { namespace cpu {

using namespace mkldnn::impl::status;
using namespace mkldnn::impl::precision;
using namespace mkldnn::impl::prop_kind;
using namespace mkldnn::impl::primitive_kind;

template <impl::precision_t prec>
class jit_avx2_lrn: public primitive {
private:
    const impl::lrn_primitive_desc_t &_ppd;

    // TODO: implement in cpp.
    status_t execute_forward();
    status_t execute_backward_data();

    // Computes output h (x) w (x) 8
    void(*ker_hw8_first)(const void *);
    void(*ker_hw8_last )(const void *);
    void(*ker_hw8      )(const void *);

    typename precision2type<prec>::type jit_alpha, jit_one;
    typename ::xbyak_lrn *jit_lrn, *jit_lrn_first, *jit_lrn_last;

protected:
    status_t execute_impl() {
        switch (_ppd.lrn_desc.prop_kind) {
        case forward: return execute_forward(); break;
        case backward_data: return execute_backward_data(); break;
        default: assert(0 && "invalid prop_kind"); // should never happen
        }
    }

public:
    typedef typename precision2type<prec>::type data_t;

    jit_avx2_lrn(const lrn_primitive_desc_t &ppd,
        const primitive_at_t *inputs, const primitive *outputs[]);

    ~jit_avx2_lrn();

    /* static magic */
    static status_t primitive_desc_init(primitive_desc_t *primitive_desc,
            const op_desc_t &op_desc, const mkldnn::impl::engine &aengine);
    static status_t create(primitive **aprimitive,
            const primitive_desc_t *primitive_desc,
            const primitive_at_t inputs[], const primitive *outputs[]);
    static const primitive_impl implementation;
};

}}}

#endif

// vim: et ts=4 sw=4 cindent cino^=l0,\:0,N-s
