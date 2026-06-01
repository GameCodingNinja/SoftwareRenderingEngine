
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
    m_radius(0.f)
{
}

/************************************************************************
*    DESC:  Destructor
************************************************************************/
CNode::~CNode()
{
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
    // Dispatch to payload
    std::visit([](auto & p)
    {
        using T = std::decay_t<decltype(p)>;

        if constexpr (std::is_same_v<T, std::unique_ptr<CSprite>>)
            p->Update();

        else if constexpr (std::is_same_v<T, std::unique_ptr<CUIControl>>)
            p->update();

        // CObject has no update — intentional no-op

    }, m_payload);

    // Recurse children
    for( auto & child : m_children )
        child->update();
}

/************************************************************************
*    DESC:  Transform the nodes — head node version (no parent)
************************************************************************/
void CNode::transform()
{
    getObject()->transform();

    for( auto & child : m_children )
        child->transform( *getObject() );
}

/************************************************************************
*    DESC:  Transform the nodes — child version (with parent)
************************************************************************/
void CNode::transform( const CObject & object )
{
    getObject()->transform( object );

    for( auto & child : m_children )
        child->transform( *getObject() );
}

/************************************************************************
*    DESC:  Render the nodes
************************************************************************/
void CNode::render( const CCamera & camera )
{
    std::visit([&camera](auto & p)
    {
        using T = std::decay_t<decltype(p)>;

        if constexpr (std::is_same_v<T, std::unique_ptr<CSprite>>)
            p->render( camera );

        else if constexpr (std::is_same_v<T, std::unique_ptr<CUIControl>>)
            p->render( camera );

    }, m_payload);

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
    return std::visit([](auto & p) -> CObject *
    {
        using T = std::decay_t<decltype(p)>;

        if constexpr (std::is_same_v<T, CObject>)
            return &p;

        else if constexpr (std::is_same_v<T, std::unique_ptr<CSprite>>)
            return static_cast<CObject *>(p.get());

        else if constexpr (std::is_same_v<T, std::unique_ptr<CUIControl>>)
            return static_cast<CObject *>(p.get());

        else
            return nullptr;

    }, m_payload);
}

/************************************************************************
*    DESC:  Get the sprite — returns nullptr if not a sprite node
************************************************************************/
CSprite * CNode::getSprite()
{
    if( auto * p = std::get_if<std::unique_ptr<CSprite>>(&m_payload) )
        return p->get();

    return nullptr;
}

/************************************************************************
*    DESC:  Get the control — returns nullptr if not a UI control node
************************************************************************/
CUIControl * CNode::getControl()
{
    if( auto * p = std::get_if<std::unique_ptr<CUIControl>>(&m_payload) )
        return p->get();

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
*    DESC:  Set the payload to a sprite
************************************************************************/
void CNode::setPayload( std::unique_ptr<CSprite> pSprite )
{
    m_payload = std::move(pSprite);
}

/************************************************************************
*    DESC:  Set the payload to a UI control
************************************************************************/
void CNode::setPayload( std::unique_ptr<CUIControl> pControl )
{
    m_payload = std::move(pControl);
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
