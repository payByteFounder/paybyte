#include <cpuminer.h>

#include <chainparams.h>
#include <consensus/consensus.h>
#include <consensus/validation.h>
#include <consensus/merkle.h>
#include <miner.h>                 // BlockAssembler, BlockTemplate
#include <node/context.h>          // NodeContext
#include <pow.h>                   // CheckProofOfWork
#include <primitives/block.h>
#include <script/script.h>
#include <uint256.h>
#include <validation.h>            // ProcessNewBlock
#include <util/system.h>
#include <util/time.h>

#include <chrono>

CpuMiner::CpuMiner(NodeContext& node) : m_node(node) {}
CpuMiner::~CpuMiner() { Stop(); }

bool CpuMiner::Start(int threads, const CScript& scriptPubKey)
{
    m_script = scriptPubKey;
    if (threads <= 0) return false;
    if (m_running.exchange(true)) return false; // already running

    m_threads.store(threads);
    m_hashes.store(0);
    m_blocks_found.store(0);
    m_start_time.store(GetTimeMillis());

    m_workers.clear();
    m_workers.reserve((size_t)threads);

    for (int i = 0; i < threads; i++) {
        m_workers.emplace_back(&CpuMiner::WorkerLoop, this, i, scriptPubKey);
    }
    return true;
}

void CpuMiner::Stop()
{
    if (!m_running.exchange(false)) return;

    for (auto& t : m_workers) {
        if (t.joinable()) t.join();
    }
    m_workers.clear();
    m_threads.store(0);
    m_hashes.store(0);
    m_start_time.store(0);
}

void CpuMiner::WorkerLoop(int worker_id, CScript scriptPubKey)
{
    while (m_running.load()) {

        if (!m_node.chainman || !m_node.mempool) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        ChainstateManager& chainman = *m_node.chainman;
        CTxMemPool& mempool = *m_node.mempool;

        std::unique_ptr<CBlockTemplate> pblocktemplate;

        try {
            BlockAssembler assembler(mempool, Params());
            pblocktemplate = assembler.CreateNewBlock(scriptPubKey);
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        if (!pblocktemplate) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            continue;
        }

        CBlock& block = pblocktemplate->block;

        // ensure merkle root matches vtx
        block.hashMerkleRoot = BlockMerkleRoot(block);

        const uint32_t stride = (uint32_t)m_threads.load();
        block.nNonce = (uint32_t)worker_id;

        const Consensus::Params& consensus = Params().GetConsensus();

        // Optional: start nonce from 0 for each new template
        // block.nNonce = 0;

        while (m_running.load()) {

            block.nNonce += stride;

            const uint256 powHash = block.GetPoWHash();
            m_hashes.fetch_add(1);

            if (CheckProofOfWork(powHash, block.nBits, consensus)) {

                bool new_block = false;

                std::shared_ptr<const CBlock> block_ptr =
                    std::make_shared<const CBlock>(block);

                    const bool processed =
                        chainman.ProcessNewBlock(
                            Params(),
                            block_ptr,
                            /*force_processing=*/true,
                            &new_block
                        );

                    if (!processed) {
                        LogPrintf("CPUMINER: Block rejected by ProcessNewBlock\n");
                    }

                    if (processed && new_block) {
                        LogPrintf("CPUMINER: Block accepted at height %d\n",
                                chainman.ActiveChain().Height());
                        m_blocks_found.fetch_add(1);
                    }

                // break to rebuild template
                break;
            }
        }
    }
}

double CpuMiner::GetHashRate() const
{
    if (!m_running.load())
        return 0.0;

    int64_t start = m_start_time.load();
    if (start == 0)
        return 0.0;

    int64_t now = GetTimeMillis();
    double seconds = (now - start) / 1000.0;

    if (seconds <= 0.0)
        return 0.0;

    return m_hashes.load() / seconds;
}

void CpuMiner::SetThreadCount(int threads)
{
    if (threads <= 0)
        threads = 1;

    if (!m_running.load()) {
        m_threads.store(threads);
        return;
    }

    Stop();
    Start(threads, m_script);
}