// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xdata/xblock_statistics_data.h"
#include "xdata/xtableblock.h"
#include "xstake/xstake_algorithm.h"
#include "xvm/xcontract/xcontract_base.h"
#include "xvm/xcontract/xcontract_exec.h"
#include "xvm/xcontract_helper.h"

NS_BEG3(top, xvm, system_contracts)

class xzec_workload_contract_v2 : public xcontract::xcontract_base
{
    using xbase_t = xcontract::xcontract_base;

public:
    XDECLARE_DELETED_COPY_DEFAULTED_MOVE_SEMANTICS(xzec_workload_contract_v2);
    XDECLARE_DEFAULTED_OVERRIDE_DESTRUCTOR(xzec_workload_contract_v2);

    explicit xzec_workload_contract_v2(common::xnetwork_id_t const &network_id);

    xcontract_base *clone() override { return new xzec_workload_contract_v2(network_id()); }

    /**
     * @brief setup the contract
     *
     */
    void setup();

    /**
     * @brief call zec reward contract to calculate reward
     *
     * @param timestamp the time to call
     */
    void on_timer(common::xlogic_time_t const timestamp);

    /**
     * @brief process on receiving workload
     *
     * @param workload_str workload
     */
    void on_receive_workload(std::string const & workload_str);

    BEGIN_CONTRACT_WITH_PARAM(xzec_workload_contract_v2)
    CONTRACT_FUNCTION_PARAM(xzec_workload_contract_v2, on_receive_workload);
    CONTRACT_FUNCTION_PARAM(xzec_workload_contract_v2, on_timer);
    END_CONTRACT_WITH_PARAM

    void on_receive_workload2(xbyte_buffer_t const & workload_bytes);

private:
    /**
     * @brief check if mainnet is activated
     *
     * @return int 0 - not activated, other - activated
     */
    bool is_mainnet_activated() const;
    
    /**
     * @brief update tgas
     *
     * @param table_pledge_balance_change_tgas table pledge balance change tgas
     */
    void update_tgas(int64_t table_pledge_balance_change_tgas);

    /**
     * @brief get_fullblock
     */
    std::vector<xobject_ptr_t<data::xblock_t>> get_fullblock(const uint64_t timestamp, const uint32_t table_id);

    /**
     * @brief add_workload_with_fullblock
     */
    void accumulate_workload(xstatistics_data_t const & stat_data, std::map<common::xgroup_address_t, xstake::xgroup_workload_t> & group_workload);

    /**
     * @brief add_workload_with_fullblock
     */
    void accumulate_workload_with_fullblock(common::xlogic_time_t const timestamp,
                                            const uint32_t start_table,
                                            const uint32_t end_table,
                                            std::map<common::xgroup_address_t, xstake::xgroup_workload_t> & group_workload);

    /**
     * @brief get_workload
     */
    xstake::xgroup_workload_t get_workload(common::xgroup_address_t const & group_address);

    /**
     * @brief set_workload
     */
    void set_workload(common::xgroup_address_t const & group_address, xstake::xgroup_workload_t const & group_workload);

    /**
     * @brief stash_workload
     */
    void update_workload(std::map<common::xgroup_address_t, xstake::xgroup_workload_t> const & group_workload);

    /**
     * @brief upload_workload
     */
    void upload_workload(common::xlogic_time_t const timestamp);

    /**
     * @brief clear_workload
     */
    void clear_workload();

    /**
     * @brief get_table_height
     */
    uint64_t get_table_height(const uint32_t table_id) const;

    /**
     * @brief update_table_height
     */
    void update_table_height(const uint32_t table_id, uint64_t cur_read_height);

    bool handle_recv_workload(std::string const & workload_str,
                              common::xaccount_address_t const & table_addr_of_workload_data,
                              uint16_t table_id,
                              uint64_t table_height,
                              bool mainnet_actived,
                              std::map<std::string, std::string> const & current_workload_data,
                              std::string & tgas_string,
                              std::map<std::string, std::string> & updated_workload_data);

    bool handle_recv_workload2(xbyte_buffer_t const & workload_bytes,
                               common::xaccount_address_t const & table_addr_of_workload_data,
                               uint16_t table_id,
                               uint64_t table_height,
                               bool mainnet_actived,
                               std::map<std::string, std::string> const & current_workload_data,
                               std::string & tgas_string,
                               std::map<std::string, std::string> & updated_workload_data);

    void do_update_tgas(int64_t input, std::string & current_tgas);

    std::map<std::string, std::string> do_update_workload(std::map<std::string, std::string> const & current_workload_data,
                                                          std::map<common::xgroup_address_t, top::xstake::xgroup_workload_t> const & uploaded_group_workload);

    std::map<std::string, std::string> do_update_workload2(std::map<std::string, std::string> const & current_workload_data,
                                                           std::map<common::xgroup_address_t, top::xstake::xgroup_workload_t> const & uploaded_group_workload);
};

NS_END3
