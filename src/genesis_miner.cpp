// src/genesis_miner.cpp
#include <chainparams.h>
#include <consensus/consensus.h>
#include <hash.h>
#include <consensus/merkle.h>
#include <logging.h>
#include <streams.h>
#include <uint256.h>
#include <util/strencodings.h>
#include <util/system.h>

#include <pow.h>           // CheckProofOfWork
#include <script/script.h>
#include <script/standard.h>
#include <primitives/block.h>
#include <primitives/transaction.h>

#include <iostream>
#include <limits>
#include <vector>
#include <cstring>

static CBlock MakeGenesis(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, CAmount reward, int extraNonce)
{
    const char* pszTimestamp = "PayByte (PBE) genesis - 2026-02-21";

    // Build coinbase (like chainparams.cpp)
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);

    // Put extranonce in scriptSig so merkle changes when we bump it
    // Keep the original structure but append extranonce.
    txNew.vin[0].scriptSig =
        CScript()
        << 486604799
        << CScriptNum(4)
        << std::vector<unsigned char>((const unsigned char*)pszTimestamp,
                                      (const unsigned char*)pszTimestamp + std::strlen(pszTimestamp))
        << CScriptNum(extraNonce);

    txNew.vout[0].nValue = reward;

    // Use the same pubkey/script you already set in chainparams.cpp
    const CScript genesisOutputScript =
        CScript() << ParseHex("040184710fa689ad5023690c80f3a49c8f13f8d45b8c857fbcbc8bc4a8e4d3eb4b10f4d4604fa08dce601aaf0f470216fe1b51850b4acf21b179c45070ac7b03a9")
                 << OP_CHECKSIG;

    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

int main(int argc, char* argv[])
{
    // Pick the network you’re mining for:
    // Use the same bits/powLimit you want in chainparams.
    // Start with EASY bits to test workflow, then tighten.
    const uint32_t nTime    = 1771632000;
    const uint32_t nBits    = 0x207fffff;   // easy target for dev; later use 0x1e0fffff if desired
    const int32_t  nVersion = 1;
    const CAmount  reward   = 12 * COIN;

    Consensus::Params consensus;
    // For mining we only need powLimit consistent with nBits target cap.
    consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    consensus.fPowAllowMinDifficultyBlocks = true;
    consensus.fPowNoRetargeting = true;

    uint32_t nonce = 0;
    int extraNonce = 0;

    while (true) {
        // Try a batch of nonces for this extranonce
        for (nonce = 0; nonce < std::numeric_limits<uint32_t>::max(); ++nonce) {
            CBlock genesis = MakeGenesis(nTime, nonce, nBits, nVersion, reward, extraNonce);

            if (CheckProofOfWork(genesis.GetPoWHash(), genesis.nBits, consensus)) {
                std::cout << "FOUND!\n";
                std::cout << "time: " << genesis.nTime << "\n";
                std::cout << "nonce: " << genesis.nNonce << "\n";
                std::cout << "bits: " << std::hex << genesis.nBits << std::dec << "\n";
                std::cout << "merkle: " << genesis.hashMerkleRoot.ToString() << "\n";
                std::cout << "hash(GetHash): " << genesis.GetHash().ToString() << "\n";
                std::cout << "powhash(GetPoWHash): " << genesis.GetPoWHash().ToString() << "\n";
                std::cout << "extraNonce: " << extraNonce << "\n";
                return 0;
            }
        }

        // Exhausted nonce space, bump extranonce (changes merkle root)
        ++extraNonce;
        std::cout << "Bumping extraNonce to " << extraNonce << "\n";
    }
}