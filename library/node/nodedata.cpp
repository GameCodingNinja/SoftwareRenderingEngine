
/************************************************************************
*    FILE NAME:       nodedata.cpp
*
*    DESCRIPTION:     Node Data Class
************************************************************************/

// Physical component dependency
#include <node/nodedata.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>

/************************************************************************
*    DESC:  Constructor — full XML parse
************************************************************************/
CNodeData::CNodeData(
    const XMLNode & node,
    const std::string & nodeName,
    int nodeId,
    int parentNodeId,
    const std::string & defGroup,
    const std::string & defObjName,
    int userId ) :
        m_xmlNode( node.getChildNode() ),
        m_group(defGroup),
        m_objectName(defObjName),
        m_nodeName(nodeName),
        m_nodeId(nodeId),
        m_parentNodeId(parentNodeId),
        m_userId(userId),
        m_nodeType(ENodeType::_NULL_),
        m_controlType(EControlType::_NULL_),
        m_hasChildrenNodes(false)
{
    // Does this node have children nodes?
    if( node.nChildNode("node") > 0 )
        m_hasChildrenNodes = true;

    // Get the group this sprite/object belongs to
    if( m_xmlNode.isAttributeSet( "group" ) )
        m_group = m_xmlNode.getAttribute( "group" );

    // Get the object data name
    // Init with the node name if no object name was specified and no default was provided
    if( m_objectName.empty() )
        m_objectName = nodeName;

    if( m_xmlNode.isAttributeSet( "objectName" ) )
        m_objectName = m_xmlNode.getAttribute( "objectName" );

    else if( defObjName.empty() )
        NGenFunc::PostDebugMsg(
            NGenFunc::FormatString(
                "Node name used for object data look-up (%s, %s)!",
                m_group, m_objectName ) );

    // Get the node type by examining child elements
    for( int i = 0; i < node.nChildNode(); ++i )
    {
        const XMLNode childNode = node.getChildNode( i );

        if( std::strcmp( childNode.getName(), "object" ) == 0 )
        {
            m_nodeType = ENodeType::OBJECT;
            break;
        }
        else if( std::strcmp( childNode.getName(), "sprite" ) == 0 )
        {
            m_nodeType = ENodeType::SPRITE;
            break;
        }
        else if( std::strcmp( childNode.getName(), "uiMeter" ) == 0 )
        {
            m_nodeType = ENodeType::UI_CONTROL;
            m_controlType = EControlType::METER;
            break;
        }
        else if( std::strcmp( childNode.getName(), "uiProgressBar" ) == 0 )
        {
            m_nodeType = ENodeType::UI_CONTROL;
            m_controlType = EControlType::PROGRESS_BAR;
            break;
        }
    }

    if( m_nodeType == ENodeType::_NULL_ )
        throw NExcept::CCriticalException("Node Load Error!",
            NGenFunc::FormatString(
                "Node type not defined (%s).\n\n%s\nLine: %d",
                nodeName, __FUNCTION__, __LINE__ ));
}

/************************************************************************
*    DESC:  Constructor — dynamic node data generation for sprite creation
************************************************************************/
CNodeData::CNodeData(
    const std::string & group,
    const std::string & objName ) :
        m_group(group),
        m_objectName(objName),
        m_nodeName(objName),
        m_nodeId(defs_DEFAULT_NODE_ID),
        m_parentNodeId(defs_DEFAULT_NODE_ID),
        m_userId(defs_DEFAULT_ID),
        m_nodeType(ENodeType::SPRITE),
        m_controlType(EControlType::_NULL_),
        m_hasChildrenNodes(false)
{
}

/************************************************************************
*    DESC:  Destructor
************************************************************************/
CNodeData::~CNodeData()
{}

/************************************************************************
*    DESC:  Get the node name
************************************************************************/
const std::string & CNodeData::getNodeName() const
{
    return m_nodeName;
}

/************************************************************************
*    DESC:  Get the node id
************************************************************************/
uint8_t CNodeData::getNodeId() const
{
    return m_nodeId;
}

/************************************************************************
*    DESC:  Get the parent node id
************************************************************************/
uint8_t CNodeData::getParentNodeId() const
{
    return m_parentNodeId;
}

/************************************************************************
*    DESC:  Get the user id
************************************************************************/
int CNodeData::getUserId() const
{
    return m_userId;
}

/************************************************************************
*    DESC:  Get the node type
************************************************************************/
ENodeType CNodeData::getNodeType() const
{
    return m_nodeType;
}

/************************************************************************
*    DESC:  Get the control type
************************************************************************/
EControlType CNodeData::getControlType() const
{
    return m_controlType;
}

/************************************************************************
*    DESC:  Does this node have children?
************************************************************************/
bool CNodeData::hasChildrenNodes() const
{
    return m_hasChildrenNodes;
}

/************************************************************************
*    DESC:  Get the group
************************************************************************/
const std::string & CNodeData::getGroup() const
{
    return m_group;
}

/************************************************************************
*    DESC:  Get the object name
************************************************************************/
const std::string & CNodeData::getObjectName() const
{
    return m_objectName;
}

/************************************************************************
*    DESC:  Get the XML Node
************************************************************************/
const XMLNode & CNodeData::getXMLNode() const
{
    return m_xmlNode;
}
