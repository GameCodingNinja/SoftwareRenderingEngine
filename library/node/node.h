
/************************************************************************
*    FILE NAME:       node.h
*
*    DESCRIPTION:     Composition-based scene graph node.
*                     One class handles tree structure, payload dispatch,
*                     and recursive game loop operations. No subclasses.
*                     Payloads (CObject, CSprite, CUIControl) are owned
*                     via a CObject base pointer and cast by m_type.
*
*                     Branch-only data (children, accumulated radius/size)
*                     is held in a lazily-allocated SBranch, kept null for
*                     leaf nodes so a lone sprite carries no branch overhead.
************************************************************************/

#pragma once

// Game lib dependencies
#include <node/nodedefs.h>
#include <common/object.h>
#include <common/size.h>

// Standard lib dependencies
#include <vector>
#include <string>
#include <memory>
#include <atomic>

// Forward declaration(s)
class CSprite;
class CUIControl;
class CCamera;
class CNodeData;

class CNode
{
public:

    // Constructor — default group/object node
    CNode( uint8_t nodeId = defs_DEFAULT_NODE_ID, uint8_t parentId = defs_DEFAULT_NODE_ID );

    // Destructor
    ~CNode();

    // Non-copyable
    CNode(const CNode&) = delete;
    CNode& operator=(const CNode&) = delete;

    // Only called after node creation with all its children
    void init();

    // Add a node — finds parent by m_nodeId match, attaches child
    bool addNode( std::unique_ptr<CNode> pNode );

    // Push back node directly into children vector
    void pushBackNode( std::unique_ptr<CNode> pNode );

    // Find the parent by build-time ID
    // NOTE: This is a recursive function
    CNode * findParent( uint8_t parentId );

    // Find a child by name CRC
    // NOTE: This is a recursive function
    CNode * findChild( uint16_t crcName );

    // Update the nodes
    void update();

    // Transform the nodes
    void transform();
    void transform( const CObject & object );

    // Render the nodes
    void render( const CCamera & camera );

    // Get the unique handle
    handle16_t getHandle() const;

    // Get the user id
    int getId() const;

    // Get the node id
    uint8_t getNodeId() const;

    // Get the parent id
    uint8_t getParentId() const;

    // Get the node type
    ENodeType getType() const;

    // Get the object — works for all payload types since they all inherit CObject
    CObject * getObject();

    // Get the sprite — returns nullptr if not a sprite node
    CSprite * getSprite();

    // Get the control — returns nullptr if not a UI control node
    CUIControl * getControl();

    // Get the parent node
    CNode * getParent();

    // Get the radius
    float getRadius() const;

    // Get the size
    const CSize<float> & getSize() const;

    // Set the payload — takes ownership via CObject base pointer
    void setPayload( CObject * pPayload );

    // Set the node name
    void setName( const std::string & name );

    // Set the user id
    void setUserId( int16_t userId );

    // Set the node type
    void setType( ENodeType type );

private:

    // Calculate the accumulated size from all children
    void calcSize( CNode * pNode, CSize<float> & size );

private:

    // Atomic handle incrementer
    static std::atomic<handle16_t> m_hAtomicIter;

    // Unique node handle
    handle16_t m_handle;

    // User id
    int16_t m_userId;

    // Node name CRC (CRC-16 of the node name)
    uint16_t m_crcName;

    // Build-time node id
    uint8_t m_nodeId;

    // Build-time parent node id
    uint8_t m_parentId;

    // Is this the head node?
    bool m_headNode;

    // Node type
    ENodeType m_type;

    // Parent node pointer — O(1) parent access
    CNode * m_parent;

    // Payload — CObject, CSprite, or CUIControl owned via base pointer.
    // Cast to derived type based on m_type. Freed in destructor.
    CObject * m_pPayload;

    // Branch-only data — allocated on first child, null for leaf nodes.
    struct SBranch
    {
        // Child nodes — RAII ownership
        std::vector<std::unique_ptr<CNode>> children;

        // Accumulated bounding radius
        float radius = 0.f;

        // Accumulated bounding size
        CSize<float> size;
    };

    // Null for leaf nodes. Freed in destructor.
    SBranch * m_pBranch;
};
