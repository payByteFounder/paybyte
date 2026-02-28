#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <script/script.h>
#include <string>
#include <thread>
#include <vector>

class ChainstateManager;
class CTxMemPool;
class CBlock;
struct NodeContext;

class CpuMiner
{
public:
    explicit CpuMiner(NodeContext& node);
    ~CpuMiner();

    bool Start(int threads, const CScript& scriptPubKey);
    void Stop();

    bool IsRunning() const { return m_running.load(); }

    uint64_t GetHashes() const { return m_hashes.load(); }
    uint64_t GetBlocksFound() const { return m_blocks_found.load(); }
    int GetThreads() const { return m_threads.load(); }
    double GetHashRate() const;
    void SetThreadCount(int threads);
    int GetThreadCount() const { return m_threads.load(); }

private:
    void WorkerLoop(int worker_id, CScript scriptPubKey);

    NodeContext& m_node;
    std::atomic<bool> m_running{false};
    std::atomic<int>  m_threads{0};

    std::atomic<uint64_t> m_hashes{0};
    std::atomic<uint64_t> m_blocks_found{0};

    std::atomic<int64_t> m_start_time{0};

    std::vector<std::thread> m_workers;
    CScript m_script;
};