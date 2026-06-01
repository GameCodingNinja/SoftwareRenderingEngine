
/************************************************************************
*    FILE NAME:       nodedata.h
*
*    DESCRIPTION:     Node Data Class — holds parsed XML data for one
*                     node in a strategy tree. Stores group, object name,
*                     and the XMLNode for deferred loading of transforms,
*                     scripts, etc.
************************************************************************/

#pragma once

// Game lib dependencies
#include <node/nodedefs.h>
#include <utilities/xmlParser.h>

// Standard lib dependencies
#include <string>

class CNodeData
{
public:

    // Constructor — full XML parse
    CNodeData(
        const XMLNode & node,
        const std::string & nodeName,
        int nodeId = defs_DEFAULT_NODE_ID,
        int parentNodeId = defs_DEFAULT_NODE_ID,
        const std::string & defGroup = std::string(),
        const std::string & defObjName = std::string(),
        int userId = defs_DEFAULT_ID );

    // Constructor — dynamic node data generation for sprite creation
    CNodeData(
        const std::string & group,
        const std::string & objName );

    // Destructor
    ~CNodeData();

    // Get the node name
    const std::string & getNodeName() const;

    // Get the node id
    uint8_t getNodeId() const;

    // Get the parent node id
    uint8_t getParentNodeId() const;

    // Get the user id
    int getUserId() const;

    // Get the node type
    ENodeType getNodeType() const;

    // Get the control type
    EControlType getControlType() const;

    // Does this node have children?
    bool hasChildrenNodes() const;

    // Get the group
    const std::string & getGroup() const;

    // Get the object name
    const std::string & getObjectName() const;

    // Get the XML Node
    const XMLNode & getXMLNode() const;

private:

    // XML Node holding sprite/object/control data
    XMLNode m_xmlNode;

    // Group name
    std::string m_group;

    // Object name
    std::string m_objectName;

    // Node name
    const std::string m_nodeName;

    // Node Id
    const uint8_t m_nodeId;

    // Parent Id
    const uint8_t m_parentNodeId;

    // User id
    int16_t m_userId;

    // Node type
    ENodeType m_nodeType;

    // Control type of node
    EControlType m_controlType;

    // Does this node have children nodes?
    bool m_hasChildrenNodes;
};
