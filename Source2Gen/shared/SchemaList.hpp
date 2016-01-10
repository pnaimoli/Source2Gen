#pragma once

#include <array>

#include "Address.hpp"
#include "Utility.hpp"

namespace schema
{
    class CSchemaClassBinding;
}

template <typename T>
class SchemaList
{
  public:
    struct Block
    {
        Block* prevBlock, * nextBlock;
        T* classBinding;
    };

    struct Blob
    {
        char irrelevant0[32];
        Block* firstBlock;
        char irrelevant1[8];
    };

    int m_BlockSize;
    int m_BlobsAllocated;
    int m_GrowMode;
    int m_BlocksPerBlob;
    char irrelevant[64];
    Blob blobs[256];
};
