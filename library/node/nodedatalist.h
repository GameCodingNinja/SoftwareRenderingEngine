
/************************************************************************
*    FILE NAME:       nodedatalist.h
*
*    DESCRIPTION:     Based on how the XML is written, creates a node
*                     list for easy Parent/child node creation.
*                     Recursively parses the XML tree into a flat
*                     vector of CNodeData with parent/child IDs.
************************************************************************/

#pragma once

// Game lib dependencies
#include <node/nodedefs.h>

// Standard lib dependencies
#include <vector>
#include <string>

// Forward declaration(s)
struct XMLNode;
class CNodeData;

class CNodeDataList
{
public:

    // Constructor — full XML parse
    CNodeDataList(
        const XMLNode & xmlNode,
        const std::string & defGroup,
        const std::string & defObjName,
        int userId = defs_DEFAULT_ID );

    // Constructor — dynamic node data generation for sprite creation
    CNodeDataList(
        const std::string & group,
        const std::string & objName );

    // Destructor
    ~CNodeDataList();

    // Get the node data
    const std::vector<CNodeData> & getData() const;

private:

    // Load the node data recursively
    void loadNode(
        const XMLNode & xmlNode,
        CNodeData & nodeData,
        const std::string & defGroup,
        const std::string & defObjName,
        int userId );

private:

    // id counter
    int m_idCounter;

    // Vector of the node data
    std::vector<CNodeData> m_dataVec;
};
