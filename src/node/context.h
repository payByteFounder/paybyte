// Copyright (c) 2019-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_NODE_CONTEXT_H
#define BITCOIN_NODE_CONTEXT_H

#include <functional>
#include <memory>
#include <vector>

class ArgsManager;
class BanMan;
class CConnman;
class CpuMiner;              // Forward declaration only
class CScheduler;
class CTxMemPool;
class ChainstateManager;
class PeerManager;

namespace interfaces {
class Chain;
class ChainClient;
class WalletClient;
} // namespace interfaces

//! NodeContext struct containing references to chain state and connection
//! state.
//
//! Used by init, rpc, and test code to pass object references around
//! without using globals. This struct should only contain references
//! and not contain business logic.
//
struct NodeContext {
    std::unique_ptr<CConnman> connman;

    // Single miner instance
    std::unique_ptr<CpuMiner> miner;

    std::unique_ptr<CTxMemPool> mempool;
    std::unique_ptr<PeerManager> peerman;

    // Raw pointer because lifetime is managed elsewhere
    ChainstateManager* chainman{nullptr};

    std::unique_ptr<BanMan> banman;

    // Raw pointer — owned elsewhere
    ArgsManager* args{nullptr};

    std::unique_ptr<interfaces::Chain> chain;

    //! Wallet or other chain clients
    std::vector<std::unique_ptr<interfaces::ChainClient>> chain_clients;

    //! Wallet client used by GUI
    interfaces::WalletClient* wallet_client{nullptr};

    std::unique_ptr<CScheduler> scheduler;

    std::function<void()> rpc_interruption_point = [] {};

    std::unique_ptr<CpuMiner> cpu_miner;

    NodeContext();
    ~NodeContext();
};

#endif // BITCOIN_NODE_CONTEXT_H