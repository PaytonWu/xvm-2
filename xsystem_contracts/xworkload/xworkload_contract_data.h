// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcommon/xaddress.h"
#include "xstake/xstake_algorithm.h"

NS_BEG3(top, xvm, system_contracts)

struct xtop_workload_contract_data {
    std::map<common::xgroup_address_t, top::xstake::xgroup_workload_t> group_workloads;
    int64_t tgas;
    uint64_t height;
};
using xworkload_contract_data_t = xtop_workload_contract_data;

NS_END3

#include "xstake/xcodec/xmsgpack/xgroup_workload_codec.hpp"
#include "xcommon/xcodec/xmsgpack/xcluster_address_codec.hpp"

NS_BEG1(msgpack)
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    NS_BEG1(adaptor)

    XINLINE_CONSTEXPR std::size_t xworkload_contract_data_field_count{3};
    XINLINE_CONSTEXPR std::size_t xworkload_contract_data_group_workloads_field_index{0};
    XINLINE_CONSTEXPR std::size_t xworkload_contract_data_tgas_field_index{1};
    XINLINE_CONSTEXPR std::size_t xworkload_contract_data_height_field_index{2};

    template <>
    struct convert<top::xvm::system_contracts::xworkload_contract_data_t> final {
        msgpack::object const & operator()(msgpack::object const & o, top::xvm::system_contracts::xworkload_contract_data_t & v) const {
            if (o.type != msgpack::type::ARRAY) {
                throw msgpack::type_error{};
            }

            if (o.via.array.size == 0) {
                return o;
            }

            switch (o.via.array.size - 1) {
            default:
                XATTRIBUTE_FALLTHROUGH;

            case xworkload_contract_data_height_field_index:
                v.height = o.via.array.ptr[xworkload_contract_data_height_field_index].as<uint64_t>();
                XATTRIBUTE_FALLTHROUGH;

            case xworkload_contract_data_tgas_field_index:
                v.tgas = o.via.array.ptr[xworkload_contract_data_tgas_field_index].as<int64_t>();
                XATTRIBUTE_FALLTHROUGH;

            case xworkload_contract_data_group_workloads_field_index:
                v.group_workloads =
                    o.via.array.ptr[xworkload_contract_data_group_workloads_field_index].as<std::map<top::common::xgroup_address_t, top::xstake::xgroup_workload_t>>();
                XATTRIBUTE_FALLTHROUGH;
            }

            return o;
        }
    };

    template <>
    struct pack<top::xvm::system_contracts::xworkload_contract_data_t> {
        template <typename Stream>
        msgpack::packer<Stream> & operator()(msgpack::packer<Stream> & o, top::xvm::system_contracts::xworkload_contract_data_t const & message) const {
            o.pack_array(xworkload_contract_data_field_count);
            o.pack(message.group_workloads);
            o.pack(message.tgas);
            o.pack(message.height);

            return o;
        }
    };

    //template <>
    //struct object_with_zone<top::xvm::system_contracts::xworkload_contract_data_t> {
    //    void operator()(msgpack::object::with_zone & o, top::xvm::system_contracts::xworkload_contract_data_t const & message) const {
    //        o.type = type::ARRAY;
    //        o.via.array.size = xworkload_contract_data_group_workloads_field_index;
    //        o.via.array.ptr = static_cast<msgpack::object *>(o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));

    //        o.via.array.ptr[xworkload_contract_data_group_workloads_field_index] = msgpack::object{message.xip(), o.zone};
    //    }
    //};

    NS_END1
}
NS_END1
