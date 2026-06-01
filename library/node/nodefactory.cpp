
/************************************************************************
*    FILE NAME:       nodefactory.cpp
*
*    DESCRIPTION:     Class factory for node creation
************************************************************************/

// Physical component dependency
#include <node/nodefactory.h>

// Game lib dependencies
#include <node/node.h>
#include <node/nodedata.h>
#include <node/nodedatalist.h>
#include <sprite/sprite.h>
#include <gui/uicontrol.h>
#include <objectdata/objectdatamanager.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

namespace NNodeFactory
{
    /************************************************************************
    *    DESC:  Create a single node from node data
    ************************************************************************/
    std::unique_ptr<CNode> Create( const CNodeData & rNodeData )
    {
        auto upNode = std::make_unique<CNode>( rNodeData.getNodeId(), rNodeData.getParentNodeId() );

        // Set the user id
        upNode->setUserId( rNodeData.getUserId() );

        // Set the node name
        if( !rNodeData.getNodeName().empty() )
            upNode->setName( rNodeData.getNodeName() );

        if( rNodeData.getNodeType() == ENodeType::SPRITE )
        {
            upNode->setType( ENodeType::SPRITE );

            // Create the sprite from object data
            auto * pSprite = new CSprite(
                CObjectDataMgr::Instance().getData(
                    rNodeData.getGroup(), rNodeData.getObjectName() ) );

            // Load transforms (position, rotation, scale) from the XML node
            pSprite->loadTransFromNode( rNodeData.getXMLNode() );

            // Set the sprite as the payload
            upNode->setPayload( pSprite );
        }
        else if( rNodeData.getNodeType() == ENodeType::OBJECT )
        {
            upNode->setType( ENodeType::OBJECT );

            // Create the object and load transforms
            auto * pObject = new CObject;
            pObject->loadTransFromNode( rNodeData.getXMLNode() );

            // Set the object as the payload
            upNode->setPayload( pObject );
        }
        else if( rNodeData.getNodeType() == ENodeType::UI_CONTROL )
        {
            upNode->setType( ENodeType::UI_CONTROL );

            // Create the UI control based on control type
            auto * pControl = new CUIControl( rNodeData.getGroup() );

            // Load from the XML node
            pControl->loadFromNode( rNodeData.getXMLNode() );

            // Load transforms
            pControl->loadTransFromNode( rNodeData.getXMLNode() );

            // Init the control
            pControl->init();

            // Set the control as the payload
            upNode->setPayload( pControl );
        }
        else
        {
            throw NExcept::CCriticalException("Node Create Error!",
                NGenFunc::FormatString(
                    "Node type not defined (%s).\n\n%s\nLine: %d",
                    rNodeData.getNodeName(), __FUNCTION__, __LINE__ ));
        }

        return upNode;
    }

    /************************************************************************
    *    DESC:  Create a full node tree from a node data list.
    *           The first entry is the root node. Remaining entries are
    *           added as children by matching parent IDs.
    ************************************************************************/
    std::unique_ptr<CNode> Create( const CNodeDataList & rNodeDataList )
    {
        const auto & dataVec = rNodeDataList.getData();

        if( dataVec.empty() )
            return nullptr;

        // Create the head node from the first entry
        auto upHeadNode = Create( dataVec[0] );

        // Create remaining nodes and add them to the tree
        for( size_t i = 1; i < dataVec.size(); ++i )
        {
            auto upNode = Create( dataVec[i] );

            if( !upHeadNode->addNode( std::move(upNode) ) )
            {
                throw NExcept::CCriticalException("Node Tree Error!",
                    NGenFunc::FormatString(
                        "Parent node not found for node (%s).\n\n%s\nLine: %d",
                        dataVec[i].getNodeName(), __FUNCTION__, __LINE__ ));
            }
        }

        // Initialize the head node — marks as head, computes accumulated size/radius
        upHeadNode->init();

        return upHeadNode;
    }
}
