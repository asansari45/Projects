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

        StopNode(Reason reason);
        virtual ~StopNode();
        StopNode(StopNode& n);
        virtual Node* Clone();
        Reason GetReason() { return m_Reason; }
        void SetReason(Reason r) { m_Reason = r; }
    private:
        Reason m_Reason;
    };
};
