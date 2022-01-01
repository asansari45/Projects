#pragma once

#include <assert.h>

namespace Interpreter
{

// Forwards
class Node;
class ValueNode;
class VarNode;
class BinaryNode;
class PrintNode;
class QuitNode;
class HelpNode;
class LoadNode;
class IfNode;
class WhileNode;
class ForNode;
class FunctionDefNode;
class FunctionCallNode;
class ClearNode;
class ReturnNode;
class DimNode;
class PwdNode;
class FunctionsNode;
class VarsCmdNode;
class VarListNode;
class SrandNode;
class RandNode;

class NodeVisitor
{
public:
    virtual void VisitNode(Node* pNode)
    {
        assert(false);
    }
    virtual void VisitValueNode(ValueNode* pNode)
    {
    }
    virtual void VisitVarNode(VarNode* pNode)
    {
    }
    virtual void VisitBinaryNode(BinaryNode* pNode)
    {
    }
    virtual void VisitPrintNode(PrintNode* pNode)
    {
    }
    virtual void VisitQuitNode(QuitNode* pNode)
    {
    }
    virtual void VisitHelpNode(HelpNode* pNode)
    {
    }
    virtual void VisitClearNode(ClearNode* pNode)
    {
    }
    virtual void VisitLoadNode(LoadNode* pNode)
    {
    }
    virtual void VisitIfNode(IfNode* pNode)
    {
    }
    virtual void VisitWhileNode(WhileNode* pNode)
    {
    }
    virtual void VisitForNode(ForNode* pNode)
    {
    }
    virtual void VisitFunctionDefNode(FunctionDefNode* pNode)
    {
    }
    virtual void VisitFunctionCallNode(FunctionCallNode* pNode)
    {
    }
    virtual void VisitReturnNode(ReturnNode* pNode)
    {
    }
    virtual void VisitDimNode(DimNode* pNode)
    {
    }
    virtual void VisitPwdNode(PwdNode* pNode)
    {
    }
    virtual void VisitFunctionsNode(FunctionsNode* pNode)
    {
    }
    virtual void VisitVarsCmdNode(VarsCmdNode* pNode)
    {
    }
    virtual void VisitVarListNode(VarListNode* pNode)
    {
    }
    virtual void VisitSrandNode(SrandNode* pNode)
    {
    }
    virtual void VisitRandNode(RandNode* pNode)
    {
    }
};

};
