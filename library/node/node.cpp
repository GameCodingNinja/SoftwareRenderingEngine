
/************************************************************************
*    FILE NAME:       node.cpp
*
*    DESCRIPTION:     Composition-based scene graph node.
*                     See node.h for design overview.
************************************************************************/

// Physical component dependency
#include <node/node.h>

// Game lib dependencies
#include <sprite/sprite.h>
#include <gui/uicontrol.h>
#include <common/camera.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cmath>

// Initialize the atomic handle counter
std::atomic<handle16_t> CNode::m_hAtomicIter = defs_DEFAULT_HANDLE;

/************************************************************************
*    DESC:  Constructor — default group/object node
************************************************************************/
CNode::CNode( uint8_t nodeId, uint8_t parentId ) :
    m_handle(m_hAtomicIter++),
    m_userId(defs_DEFAULT_ID),
    m_crcName(0),
    m_nodeId(nodeId),
    m_parentId(parentId),
    m_headNode(false),
    m_type(ENodeType::OBJECT),
    m_parent(nullptr),
    m_pPayload(nullptr),
    m_pBranch(nullptr)
{
}

/************************************************************************
*    DESC:  Destructor
************************************************************************/
CNode::~CNode()
{
    if(m_pPayload != nullptr)
    {
        delete m_pPayload;
        m_pPayload = nullptr;
    }

    if(m_pBranch != nullptr)
    {
        delete m_pBranch;
        m_pBranch = nullptr;
    }
}

/************************************************************************
*    DESC:  Only called after node creation with all its children.
*           Marks as head node and computes accumulated size/radius.
************************************************************************/
void CNode::init()
{
    m_headNode = true;

    // Leaf nodes have no children — nothing to accumulate
    if( m_pBranch != nullptr )
    {
        // Compute the accumulated size from all children
        calcSize( this, m_pBranch->size );

        // Calculate the radius from the accumulated size
        m_pBranch->radius = std::sqrt( (m_pBranch->size.w / 2.f) * (m_pBranch->size.w / 2.f) +
                                       (m_pBranch->size.h / 2.f) * (m_pBranch->size.h / 2.f) );
    }
}

/************************************************************************
*    DESC:  Add a node — finds parent by m_nodeId match, attaches child
************************************************************************/
bool CNode::addNode( std::unique_ptr<CNode> pNode )
{
    // Find the parent node by the child's parent ID
    CNode * pParentNode = findParent( pNode->getParentId() );

    if( pParentNode != nullptr )
    {
        pNode->m_parent = pParentNode;
        pParentNode->pushBackNode( std::move(pNode) );
        return true;
    }

    return false;
}

/************************************************************************
*    DESC:  Push back node directly into children vector
************************************************************************/
void CNode::pushBackNode( std::unique_ptr<CNode> pNode )
{
    // Allocate the branch data on the first child
    if( m_pBranch == nullptr )
        m_pBranch = new SBranch;

    m_pBranch->children.push_back( std::move(pNode) );
}

/************************************************************************
*    DESC:  Find the parent by build-time ID.
*           NOTE: This is a recursive function
************************************************************************/
CNode * CNode::findParent( uint8_t parentId )
{
    CNode * pResult = nullptr;

    if( m_nodeId == parentId )
    {
        pResult = this;
    }
    else if( m_pBranch != nullptr )
    {
        auto iter = m_pBranch->children.begin();

        while( pResult == nullptr && iter != m_pBranch->children.end() )
        {
            pResult = (*iter)->findParent( parentId );
            ++iter;
        }
    }

    return pResult;
}

/************************************************************************
*    DESC:  Find a child by name CRC.
*           NOTE: This is a recursive function
************************************************************************/
CNode * CNode::findChild( uint16_t crcName )
{
    CNode * pResult = nullptr;

    if( crcName == m_crcName )
    {
        pResult = this;
    }
    else if( m_pBranch != nullptr )
    {
        auto iter = m_pBranch->children.begin();

        while( pResult == nullptr && iter != m_pBranch->children.end() )
        {
            pResult = (*iter)->findChild( crcName );
            ++iter;
        }
    }

    return pResult;
}

/************************************************************************
*    DESC:  Update the nodes
************************************************************************/
void CNode::update()
{
    if( m_type == ENodeType::SPRITE )
        static_cast<CSprite *>(m_pPayload)->Update();

    else if( m_type == ENodeType::UI_CONTROL )
        static_cast<CUIControl *>(m_pPayload)->update();

    if( m_pBranch != nullptr )
        for( auto & child : m_pBranch->children )
            child->update();
}

/************************************************************************
*    DESC:  Transform the nodes — head node version (no parent)
************************************************************************/
void CNode::transform()
{
    m_pPayload->transform();

    if( m_pBranch != nullptr )
        for( auto & child : m_pBranch->children )
            child->transform( *m_pPayload );
}

/************************************************************************
*    DESC:  Transform the nodes — child version (with parent)
************************************************************************/
void CNode::transform( const CObject & object )
{
    m_pPayload->transform( object );

    if( m_pBranch != nullptr )
        for( auto & child : m_pBranch->children )
            child->transform( *m_pPayload );
}

/************************************************************************
*    DESC:  Render the nodes
************************************************************************/
void CNode::render( const CCamera & camera )
{
    if( m_type == ENodeType::SPRITE )
        static_cast<CSprite *>(m_pPayload)->render( camera );

    else if( m_type == ENodeType::UI_CONTROL )
        static_cast<CUIControl *>(m_pPayload)->render( camera );

    if( m_pBranch != nullptr )
        for( auto & child : m_pBranch->children )
            child->render( camera );
}

/************************************************************************
*    DESC:  Get the unique handle
************************************************************************/
handle16_t CNode::getHandle() const
{
    return m_handle;
}

/************************************************************************
*    DESC:  Get the user id
************************************************************************/
int CNode::getId() const
{
    return m_userId;
}

/************************************************************************
*    DESC:  Get the node id
************************************************************************/
uint8_t CNode::getNodeId() const
{
    return m_nodeId;
}

/************************************************************************
*    DESC:  Get the parent id
************************************************************************/
uint8_t CNode::getParentId() const
{
    return m_parentId;
}

/************************************************************************
*    DESC:  Get the node type
************************************************************************/
ENodeType CNode::getType() const
{
    return m_type;
}

/************************************************************************
*    DESC:  Get the object — works for all payload types
************************************************************************/
CObject * CNode::getObject()
{
    return m_pPayload;
}

/************************************************************************
*    DESC:  Get the sprite — returns nullptr if not a sprite node
************************************************************************/
CSprite * CNode::getSprite()
{
    if( m_type == ENodeType::SPRITE )
        return static_cast<CSprite *>(m_pPayload);

    return nullptr;
}

/************************************************************************
*    DESC:  Get the control — returns nullptr if not a UI control node
************************************************************************/
CUIControl * CNode::getControl()
{
    if( m_type == ENodeType::UI_CONTROL )
        return static_cast<CUIControl *>(m_pPayload);

    return nullptr;
}

/************************************************************************
*    DESC:  Get the parent node
************************************************************************/
CNode * CNode::getParent()
{
    return m_parent;
}

/************************************************************************
*    DESC:  Get the radius
************************************************************************/
float CNode::getRadius() const
{
    return (m_pBranch != nullptr) ? m_pBranch->radius : 0.f;
}

/************************************************************************
*    DESC:  Get the size
************************************************************************/
const CSize<float> & CNode::getSize() const
{
    static const CSize<float> emptySize;
    return (m_pBranch != nullptr) ? m_pBranch->size : emptySize;
}

/************************************************************************
*    DESC:  Set the payload — takes ownership via CObject base pointer
************************************************************************/
void CNode::setPayload( CObject * pPayload )
{
    if( m_pPayload != nullptr )
        delete m_pPayload;
        
    m_pPayload = pPayload;
}

/************************************************************************
*    DESC:  Set the node name
************************************************************************/
void CNode::setName( const std::string & name )
{
    m_crcName = NGenFunc::CalcCRC16( name );
}

/************************************************************************
*    DESC:  Set the user id
************************************************************************/
void CNode::setUserId( int16_t userId )
{
    m_userId = userId;
}

/************************************************************************
*    DESC:  Set the node type
************************************************************************/
void CNode::setType( ENodeType type )
{
    m_type = type;
}

/************************************************************************
*    DESC:  Calculate the accumulated size from all children
************************************************************************/
void CNode::calcSize( CNode * pNode, CSize<float> & size )
{
    if( pNode->m_pBranch != nullptr )
    {
        for( auto & child : pNode->m_pBranch->children )
        {
            const CSize<float> & childSize = child->getSize();

            if( childSize.w > size.w )
                size.w = childSize.w;

            if( childSize.h > size.h )
                size.h = childSize.h;

            // Recurse into grandchildren
            calcSize( child.get(), size );
        }
    }
}
