
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
    m_nodeId(nodeId),
    m_parentId(parentId),
    m_headNode(false),
    m_type(ENodeType::OBJECT),
    m_parent(nullptr),
    m_pPayload(nullptr),
    m_radius(0.f)
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
}

/************************************************************************
*    DESC:  Only called after node creation with all its children.
*           Marks as head node and computes accumulated size/radius.
************************************************************************/
void CNode::init()
{
    m_headNode = true;

    // Compute the accumulated size from all children
    calcSize( this, m_size );

    // Calculate the radius from the accumulated size
    m_radius = std::sqrt( (m_size.w / 2.f) * (m_size.w / 2.f) +
                          (m_size.h / 2.f) * (m_size.h / 2.f) );
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
    m_children.push_back( std::move(pNode) );
}

/************************************************************************
*    DESC:  Find the parent by build-time ID.
*           NOTE: This is a recursive function
************************************************************************/
CNode * CNode::findParent( uint8_t parentId )
{
    if( m_nodeId == parentId )
        return this;

    for( auto & child : m_children )
    {
        CNode * pResult = child->findParent( parentId );
        if( pResult != nullptr )
            return pResult;
    }

    return nullptr;
}

/************************************************************************
*    DESC:  Find a child by name.
*           NOTE: This is a recursive function
************************************************************************/
CNode * CNode::findChild( const std::string & name )
{
    if( name == m_name )
        return this;

    for( auto & child : m_children )
    {
        CNode * pResult = child->findChild( name );
        if( pResult != nullptr )
            return pResult;
    }

    return nullptr;
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

    for( auto & child : m_children )
        child->update();
}

/************************************************************************
*    DESC:  Transform the nodes — head node version (no parent)
************************************************************************/
void CNode::transform()
{
    m_pPayload->transform();

    for( auto & child : m_children )
        child->transform( *m_pPayload );
}

/************************************************************************
*    DESC:  Transform the nodes — child version (with parent)
************************************************************************/
void CNode::transform( const CObject & object )
{
    m_pPayload->transform( object );

    for( auto & child : m_children )
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

    for( auto & child : m_children )
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
*    DESC:  Get the node name
************************************************************************/
const std::string & CNode::getName() const
{
    return m_name;
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
    return m_radius;
}

/************************************************************************
*    DESC:  Get the size
************************************************************************/
const CSize<float> & CNode::getSize() const
{
    return m_size;
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
    m_name = name;
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
    for( auto & child : pNode->m_children )
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
