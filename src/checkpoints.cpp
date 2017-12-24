// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,   uint256("0x000000770c6aea829bb1ace7b06497f71799a6358e0e292740c4f9443a17bfb6") ) //genesis
        ( 2879,   uint256("0x675ebb9f0f934c35bbf1955565482601ef68977b2fef8248689626004fcbce2d") ) //last pow
        ( 2880,   uint256("0x3818386bc9fced59f21d5a9d1eab8a532c85efecb831eceee995ae1a7dc90419") ) //first pos
        ( 100000,   uint256("0x97b8764441d1d0eff98e834cc743ec4cd6779d53f149dec9403c9758370abd8c") )
        ( 200000,   uint256("0x98b0bcf0218293549a30d938a1d641b6bc6b498361cb432e3fafd6935c592846") )
        ( 300000,   uint256("0xdcb25c38e2731a68d5bfa16b00b8479fed57c6a352cfd94d8e0dfdd690ac9ea2") )
        ( 440000,   uint256("0x88914855018cf647d1b20162eed2dbc7cdf69757f8216868c1b55687aafe1a75") )
        ( 500000,   uint256("0x41950e8d38381ce56fc0fd9eca76e73b49ce256c01ab2521897c5c6285b39c8d") )
        ( 550000,   uint256("0x8a39262d43135792a429a4e2a6deeb669c88b618ecfa3ca855edaf1e4c3631af") )
        ( 600000,   uint256("0x23141467be3b6fe23419c8a9fbcd4df431b4b1abcd43a1fa689e7b6d137e89ef") )
        ( 619480,   uint256("0x17895abf56169141d553891d00a4c46d37023250a0089800e389bfba37e91f9a") ) // avg_fee start block
        ( 625249,   uint256("0x677def44b35e162452fb12f00c4e4fa5f1339a90a4b82753266e1cd9f5b359e7") ) // problem block on avg_fee
        ( 626372,   uint256("0x977494d951029df80bee05bdd461e6e767c916c2b60ebd356133c38ae5cf5823") ) // pre avg_fee revert block
        ( 626555,   uint256("0xab4e8bdd76283070e11377f9497d7a644d9a813fc354152b36ded76d68eb5388") ) // post avg_fee revert block
        ( 655200,   uint256("0x0dbcd2799042832cb3c1c0255e34b1ca797d30db9438d836e9e830394e98b999") ) // Radium 1.4.5 Code release
        ( 700000,   uint256("0x6ceebeff2760988638367e5e91a9061bdc9623db1dceeaa7cef1eb8da88ec2bd") )
        ( 750000,   uint256("0xa865bc03e1ce183173b2892e1ed7422e92e6f6cc71f5da5b45db7c346d557960") )
        ( 800000,   uint256("0xbce932a50e264133d5456f1a8076552f5186f3f6ff46310f4c769ae83c4a1723") )
        ( 850000,   uint256("0xb29d546f1d8d57fbaef9ef2f40b107107ad5b124ed90f01556650ab9191e42c1") )
        ( 900000,   uint256("0x5c58c0b472ab44dbde75c31e549615fc00feea3a93844d8c78837b2e85245c4e") )
        ( 950000,   uint256("0x75ece61af3a0bf5243ff0a7773a8f2bb560f52ba6aa2652f0de1ce3c1199ede8") )
        ( 1000000,   uint256("0x6950e1ca2bef4adfb963df7edaf92cac8127d3445554d23738bd88c135ce59e4") )
        ( 1050000,   uint256("0x104c2d6ffa918160fb3878f89323920d037f857854782c0436a5df4be42e0492") )
        ( 1100000,   uint256("0x0978eceb49ed5dc681c766b5cba395b5008d8dbadba81f37e86fdeff61fc38d5") )
        ( 1150000,   uint256("0x12c172f98f5c491f5f230bef5040a50c4ddf8526c5160b419a708564dd1ec0c9") )
        ( 1200000,   uint256("0x619ffe5e099a182afaf5c37b0c73e60fd858acb2231ee29d95f4a223ba6e429a") )
        ( 1250000,   uint256("0x12cd34ed7b24ecf5414fdf175b72fbf30db043cf51b53d4efa79a5272e91ab9d") )
        ( 1300000,   uint256("0x493d0c754b68a09f0c22bea134264aceebc8acc13c02a80bd57632876b54e2f6") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
