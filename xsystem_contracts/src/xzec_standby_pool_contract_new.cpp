// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xvm/xsystem_contracts/xelection/xzec/xzec_standby_pool_contract_new.h"

#include "xbasic/xutility.h"
#include "xcodec/xmsgpack_codec.hpp"
#include "xconfig/xconfig_register.h"
#include "xdata/xcodec/xmsgpack/xstandby_result_store_codec.hpp"
#include "xdata/xelect_transaction.hpp"
#include "xdata/xelection/xstandby_node_info.h"
#include "xdata/xnative_contract_address.h"
#include "xconfig/xpredefined_configurations.h"
#include "xdata/xproperty.h"

#include <cinttypes>

#ifndef XSYSCONTRACT_MODULE
#    define XSYSCONTRACT_MODULE "SysContract_"
#endif
#define XCONTRACT_PREFIX "ZecStandby_"
#define XZEC_STANDBY XSYSCONTRACT_MODULE XCONTRACT_PREFIX

NS_BEG2(top, system_contracts)

using top::data::election::xstandby_node_info_t;
using top::data::election::xstandby_result_store_t;

void xtop_zec_standby_pool_contract_new::setup() {
    m_rec_standby_pool_data_last_read_height.set("0");
    m_rec_standby_pool_data_last_read_time.set("0");
}

void xtop_zec_standby_pool_contract_new::on_timer(common::xlogic_time_t const current_time) {
    XMETRICS_TIME_RECORD(XZEC_STANDBY "on_timer_all_time");
    // get standby nodes from rec_standby_pool
    XCONTRACT_ENSURE(sender() == address(), "xzec_standby_pool_contract_t instance is triggled by others");
    XCONTRACT_ENSURE(address().value() == sys_contract_zec_standby_pool_addr, "xzec_standby_pool_contract_t instance is not triggled by sys_contract_zec_standby_pool_addr");
    // XCONTRACT_ENSURE(current_time <= TIME(), "xzec_standby_pool_contract_t::on_timer current_time > consensus leader's time");

    xdbg("[xzec_standby_pool_contract_t] on_timer: %" PRIu64, current_time);

    bool update_rec_standby_pool_contract_read_status{false};

    auto const last_read_height = static_cast<std::uint64_t>(std::stoull(m_rec_standby_pool_data_last_read_height.value()));
    auto const last_read_time = static_cast<std::uint64_t>(std::stoull(m_rec_standby_pool_data_last_read_time.value()));

    auto const height_step_limitation = XGET_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_standby_pool_contract_height_step_limitation);
    auto const timeout_limitation = XGET_ONCHAIN_GOVERNANCE_PARAMETER(cross_reading_rec_standby_pool_contract_logic_timeout_limitation);

    uint64_t latest_height = state_height(common::xaccount_address_t{sys_contract_rec_standby_pool_addr});
    xdbg("[xzec_standby_pool_contract_t] get_latest_height: %" PRIu64, latest_height);

    XCONTRACT_ENSURE(latest_height >= last_read_height, "xzec_standby_pool_contract_t::on_timer latest_height < last_read_height");
    if (latest_height == last_read_height) {
        XMETRICS_PACKET_INFO(XZEC_STANDBY "update_status", "next_read_height", last_read_height, "current_time", current_time)
        m_rec_standby_pool_data_last_read_time.set(std::to_string(current_time));
        return;
    }

    // calc current_read_height:
    uint64_t next_read_height = last_read_height;
    if (latest_height - last_read_height >= height_step_limitation) {
        next_read_height = last_read_height + height_step_limitation;
        update_rec_standby_pool_contract_read_status = true;
    }
    if (!update_rec_standby_pool_contract_read_status && current_time - last_read_time > timeout_limitation) {
        next_read_height = latest_height;
        update_rec_standby_pool_contract_read_status = true;
    }
    xinfo("[xzec_standby_pool_contract_t] next_read_height: %" PRIu64, next_read_height);

    if (!block_exist(common::xaccount_address_t{sys_contract_rec_standby_pool_addr}, next_read_height)) {
        xwarn("[xzec_standby_pool_contract_t] fail to get the rec_standby_pool data. next_read_block height:%" PRIu64, next_read_height);
        return;
    }

    if (update_rec_standby_pool_contract_read_status) {
        m_rec_standby_pool_data_last_read_height.set(std::to_string(next_read_height));
        m_rec_standby_pool_data_last_read_time.set(std::to_string(current_time));
    }
    return;
}

NS_END2

#undef XZEC_STANDBY
#undef XCONTRACT_PREFIX
#undef XSYSCONTRACT_MODULE
