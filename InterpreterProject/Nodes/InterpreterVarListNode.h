#pragma once
#include "InterpreterNode.h"
namespace Interpreter
{

    class VarListNode : public Node
    {
    public:
        VarListNode();
        VarListNode(const VarListNode& rNode);
        virtual ~VarListNode();
        virtual Node* Clone();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        void SetList(Node* pList)
        {
            m_pList = pList;
        }

        Node* GetList()
        {
            return m_pList;
        }

        void Append(Node* pNode);
        int GetListCount();
    private:

        // The list of nodes.
        Node* m_pList;
    };
};
