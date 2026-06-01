
/************************************************************************
*    FILE NAME:       nodefactory.h
*
*    DESCRIPTION:     Class factory for node creation.
*                     Creates individual nodes from CNodeData and
*                     assembles full node trees from CNodeDataList.
************************************************************************/

#pragma once

// Standard lib dependencies
#include <memory>

// Forward declaration(s)
class CNode;
class CNodeData;
class CNodeDataList;

namespace NNodeFactory
{
    // Create a single node from node data
    std::unique_ptr<CNode> Create( const CNodeData & rNodeData );

    // Create a full node tree from a node data list
    std::unique_ptr<CNode> Create( const CNodeDataList & rNodeDataList );
};
