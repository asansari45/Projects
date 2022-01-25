#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    // An internal node to indicate a stop in execution with the reason why.
    // This node is not intended to be visited.
    class StopNode : public Node
    {
    public:
        enum Reason
        {
            RETURN = 1,
            BREAK = 2
        };

        StopNode(Reason reason) :
            Node(),
            m_Reason(reason)
        {
        }

        virtual ~StopNode()
        {
        }

        StopNode(StopNode& n) :
            Node(n),
            m_Reason(n.m_Reason)
        {
        }

        virtual Node* Clone()
        {
            return new StopNode(*this);
        }

        Reason GetReason() { return m_Reason; }
        void SetReason(Reason r) { m_Reason = r; }
    private:
        Reason m_Reason;
    };
};
