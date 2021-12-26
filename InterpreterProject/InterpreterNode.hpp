#pragma once

#include <string>
#include <vector>
#include "InterpreterNodeVisitor.hpp"
#include "InterpreterValue.h"
#include "InterpreterArrayValue.h"
#include "InterpreterContext.h"

namespace Interpreter
{

class Node
{
public:
    Node() :
        m_pNext(nullptr),
        m_File(),
        m_Line(0),
        m_Column(0)
    {
        Context* pContext = contextStack.size() != 0 ? contextStack.back() : nullptr;
        if (pContext != nullptr)
        {
            m_File = pContext->GetFile();
            m_Line = pContext->GetLine();
            m_Column = pContext->GetColumn();
        }
    }

    Node(const Node& rNode) :
        m_pNext(nullptr),
        m_File(rNode.m_File),
        m_Line(rNode.m_Line),
        m_Column(rNode.m_Column)
    {
    }

    virtual ~Node()
    {
    }

    // Clone only node and sons.
    virtual Node* Clone()
    {
        Node* pThis = new Node;
        assert(pThis != nullptr);
        return pThis;
    }

    Node* CloneList()
    {
        Node* pCopyStart = nullptr;
        Node* pCopyCurr = nullptr;
        for (Node* pCurr = this; pCurr != nullptr; pCurr = pCurr->GetNext())
        {
            Node* pCopy = pCurr->Clone();
            if (pCopyStart == nullptr)
            {
                pCopyStart = pCopy;
                pCopyCurr = pCopy;
            }
            else
            {
                pCopyCurr->SetNext(pCopy);
                pCopyCurr = pCopy;
            }
        }

        return pCopyStart;
    }

    void FreeList()
    {
        Node* pCurr = this;
        while (pCurr != nullptr)
        {
            Node* pNext = pCurr->GetNext();
            pCurr->Free();
            pCurr = pNext;
        }
    }

    virtual void Free()
    {
        delete this;
    }

    virtual void Accept(NodeVisitor& rVisitor)
    {
        rVisitor.VisitNode(this);
    }

    Node* GetNext() { return m_pNext; }
    void SetNext(Node* pNext) { m_pNext = pNext; }

    void SetFile(std::string f) { m_File = f; }
    std::string GetFile() { return m_File; }
    void SetLine(int line) { m_Line = line; }
    int GetLine() { return m_Line; }
    void SetColumn(int column) { m_Column = column; }
    int GetColumn() { return m_Column; }

private:
    Node* m_pNext;
    std::string m_File;
    int m_Line;
    int m_Column;
};

class ValueNode : public Node
{
public:
    ValueNode() :
        Node(),
        m_Array(false),
        m_ArrayValue(),
        m_Value()
    {
    }

    ValueNode(Value v) :
        Node(),
        m_Array(false),
        m_ArrayValue(),
        m_Value(v)
    {
    }

    ValueNode(ArrayValue v) :
        Node(),
        m_Array(true),
        m_ArrayValue(v),
        m_Value()
    {
    }

    ValueNode(const ValueNode& rNode) :
        Node(rNode),
        m_Value(rNode.m_Value)
    {
    }

    virtual ~ValueNode()
    {
    }

    Node* Clone()
    {
        return new ValueNode(*this);
    }

    virtual void Accept(NodeVisitor& rVisitor)
    {
        rVisitor.VisitValueNode(this);
    }

    Value GetValue() { return m_Value; }
    void SetValue(Value v) { m_Value = v; }

    ArrayValue GetArrayValue() { return m_ArrayValue; }
    void SetArrayValue(ArrayValue v) { m_ArrayValue = v; }

    bool IsArray() { return m_Array; }

private:
    bool m_Array;
    ArrayValue m_ArrayValue;
    Value m_Value;
};

class VarNode : public Node
{
public:
    VarNode() :
        Node(),
        m_Name(),
        m_ArraySpecifier()
    {
    }

    VarNode(const VarNode& rNode) :
        Node(rNode),
        m_Name(rNode.m_Name),
        m_ArraySpecifier(rNode.m_ArraySpecifier)
    {
    }

    virtual ~VarNode()
    {
    }

    virtual Node* Clone()
    {
        return new VarNode(*this);
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitVarNode(this);
    }

    std::string GetName() { return m_Name; }
    void SetName(std::string n) { m_Name = n; }

    std::vector<int> GetArraySpecifier() { return m_ArraySpecifier; }
    void SetArraySpecifier(std::vector<int> arraySpecifier) { m_ArraySpecifier = arraySpecifier; }

private:
    std::string m_Name;
    std::vector<int> m_ArraySpecifier;
};

class BinaryNode : public Node
{
public:

    enum Operator
    {
        EQU = 1,
        ADD,
        SUB,
        MUL,
        DIV,
        LES,
        LEQ,
        GRT,
        GEQ,
        DEQ,
        NEQ,
        NEG,
        DIM,
        ARY
    };

    BinaryNode() :
        m_Operator(EQU),
        m_pLeft(nullptr),
        m_pRight(nullptr)
    {
    }

    BinaryNode(const BinaryNode& rNode) :
        Node(rNode),
        m_Operator(rNode.m_Operator)
    {
    }

    virtual ~BinaryNode()
    {
    }

    virtual Node* Clone()
    {
        BinaryNode* pNode = new BinaryNode(*this);
        if (m_pLeft != nullptr)
        {
            pNode->SetLeft(m_pLeft->Clone());
        }

        if (m_pRight != nullptr)
        {
            pNode->SetRight(m_pRight->Clone());
        }

        return pNode;
    }

    virtual void Free()
    {
        // delete left child and right children first
        if (m_pLeft)
        {
            m_pLeft->Free();
            m_pLeft = nullptr;
        }

        if (m_pRight)
        {
            m_pRight->Free();
            m_pRight = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitBinaryNode(this);
    }

    Operator GetOperator() { return m_Operator; }
    void SetOperator(Operator o) { m_Operator = o; }

    Node* GetLeft() { return m_pLeft; }
    void SetLeft(Node* pNode) { m_pLeft = pNode; }
    Node* GetRight() { return m_pRight; }
    void SetRight(Node* pNode) { m_pRight = pNode; }

private:
    Operator m_Operator;
    Node* m_pLeft;
    Node* m_pRight;
};

class PrintNode : public Node
{
public:
    PrintNode() :
        m_pChild(nullptr)
    {
    }

    virtual ~PrintNode()
    {
    }

    virtual Node* Clone()
    {
        PrintNode* pNode =  new PrintNode;
        if (m_pChild)
        {
            pNode->SetChild(m_pChild->CloneList());
        }
        return pNode;
    }

    virtual void Free()
    {
        if (m_pChild)
        {
            m_pChild->FreeList();
            m_pChild = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitPrintNode(this);
    }

    Node* GetChild() { return m_pChild; }
    void SetChild(Node* pChild) { m_pChild = pChild; }
private:
    Node* m_pChild;
};

class QuitNode : public Node
{
public:
    QuitNode()
    {
    }

    virtual ~QuitNode()
    {
    }

    virtual Node* Clone()
    {
        return new QuitNode;
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitQuitNode(this);
    }
};

class HelpNode : public Node
{
public:
    HelpNode()
    {
    }

    virtual ~HelpNode()
    {
    }

    virtual Node* Clone()
    {
        return new HelpNode;
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitHelpNode(this);
    }
};

class LoadNode : public Node
{
public:
    LoadNode() :
        m_pChild(nullptr)
    {
    }

    virtual ~LoadNode()
    {
    }

    virtual Node* Clone()
    {
        return new PrintNode;
    }

    virtual void Free()
    {
        if (m_pChild)
        {
            m_pChild->Free();
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitLoadNode(this);
    }

    VarNode* GetChild() { return m_pChild; }
    void SetChild(VarNode* pChild) { m_pChild = pChild; }
private:
    VarNode* m_pChild;
};

class ClearNode : public Node
{
public:
    ClearNode()
    {
    }

    virtual ~ClearNode()
    {
    }

    virtual Node* Clone()
    {
        return new ClearNode;
    }

    virtual void Free()
    {
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitClearNode(this);
    }
private:
};

class IfNode : public Node
{
public:
    IfNode() :
        m_pExpr(nullptr),
        m_pThen(nullptr),
        m_pElif(nullptr),
        m_pElse(nullptr)
    {
    }

    virtual ~IfNode()
    {
    }

    virtual Node* Clone()
    {
        IfNode* pNode = new IfNode;

        // One node to clone.
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }

        if (m_pThen != nullptr)
        {
            pNode->SetThen(m_pThen->CloneList());
        }

        // Several nodes to clone.
        if (m_pElif != nullptr)
        {
            pNode->SetElif(dynamic_cast<Interpreter::IfNode*>(m_pElif->CloneList()));
        }

        // Several nodes to clone.
        if (m_pElse != nullptr)
        {
            pNode->SetElse(m_pElse->CloneList());
        }

        return pNode;
    }

    virtual void Free()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }

        if (m_pElse)
        {
            m_pElse->FreeList();
            m_pElse = nullptr;
        }

        if (m_pElif)
        {
            m_pElif->FreeList();
            m_pElif = nullptr;
        }

        if (m_pThen)
        {
            m_pThen->FreeList();
            m_pThen = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitIfNode(this);
    }

    Node* GetExpr() { return m_pExpr; }
    void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

    Node* GetThen() { return m_pThen; }
    void SetThen(Node* pThen) { m_pThen = pThen; }

    IfNode* GetElif() { return m_pElif; }
    void SetElif(IfNode* pElif) { m_pElif = pElif; }

    Node* GetElse() { return m_pElse; }
    void SetElse(Node* pElse) { m_pElse = pElse; }

private:
    Node* m_pExpr;
    Node* m_pThen;
    IfNode* m_pElif;
    Node* m_pElse;
};


class WhileNode : public Node
{
public:
    WhileNode() :
        m_pExpr(nullptr),
        m_pThen(nullptr)
    {
    }

    virtual ~WhileNode()
    {
    }

    virtual Node* Clone()
    {
        WhileNode* pNode = new WhileNode;

        // One node to clone.
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }

        // Several nodes to clone.
        if (m_pThen != nullptr)
        {
            pNode->SetThen(m_pThen->CloneList());
        }

        return pNode;
    }

    virtual void Free()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }

        if (m_pThen)
        {
            m_pThen->FreeList();
            m_pThen = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitWhileNode(this);
    }

    Node* GetExpr() { return m_pExpr; }
    void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

    Node* GetThen() { return m_pThen; }
    void SetThen(Node* pThen) { m_pThen = pThen; }

private:
    Node* m_pExpr;
    Node* m_pThen;
};


class ForNode : public Node
{
public:
    ForNode() :
        m_pInit(nullptr),
        m_pExpr(nullptr),
        m_pThen(nullptr)
    {
    }

    virtual ~ForNode()
    {
    }

    virtual Node* Clone()
    {
        ForNode* pNode = new ForNode;

        // One node to clone.
        if (m_pInit != nullptr)
        {
            pNode->SetInit(m_pInit->Clone());
        }

        // One node to clone.
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }

        // Several nodes to clone.
        if (m_pThen != nullptr)
        {
            pNode->SetThen(m_pThen->CloneList());
        }

        return pNode;
    }

    virtual void Free()
    {
        if (m_pInit)
        {
            m_pInit->Free();
            m_pInit = nullptr;
        }

        if (m_pExpr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }

        if (m_pThen)
        {
            m_pThen->FreeList();
            m_pThen = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitForNode(this);
    }

    Node* GetInit() { return m_pInit; }
    void SetInit(Node* pExpr) { m_pInit = pExpr; }

    Node* GetExpr() { return m_pExpr; }
    void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

    Node* GetThen() { return m_pThen; }
    void SetThen(Node* pThen) { m_pThen = pThen; }

private:
    Node* m_pInit;
    Node* m_pExpr;
    Node* m_pThen;
};

class FunctionDefNode : public Node
{
public:
    FunctionDefNode() :
        m_pNameVar(nullptr),
        m_pInputVars(nullptr),
        m_pCode(nullptr)
    {
    }

    virtual ~FunctionDefNode()
    {
    }

    virtual Node* Clone()
    {
        FunctionDefNode* pNode = new FunctionDefNode;
        pNode->SetNameVar(dynamic_cast<VarNode*>(m_pNameVar->Clone()));
        pNode->SetInputVars(dynamic_cast<VarNode*>(m_pInputVars->CloneList()));
        pNode->SetCode(m_pCode->CloneList());

        return pNode;
    }

    virtual void Free()
    {
        m_pNameVar->Free();
        if (m_pInputVars)
        {
            m_pInputVars->FreeList();
            m_pInputVars = nullptr;
        }

        if (m_pCode)
        {
            m_pCode->FreeList();
            m_pCode = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitFunctionDefNode(this);
    }

    VarNode* GetNameVar() { return m_pNameVar; }
    void SetNameVar(VarNode* pNameVar) { m_pNameVar = pNameVar; }

    int GetInputVarCount() 
    { 
        int count = 0;
        for (Interpreter::Node* pNode = m_pInputVars; pNode; pNode = pNode->GetNext())
        {
            count++;
        }
        return count;
    }

    VarNode* GetInputVars() { return m_pInputVars; }
    void SetInputVars(VarNode* pInputVars) { m_pInputVars = pInputVars; }

    Node* GetCode() { return m_pCode; }
    void SetCode(Node* pCode) { m_pCode = pCode; }

private:
    VarNode* m_pNameVar;
    VarNode* m_pInputVars;
    Node* m_pCode;
};


class FunctionCallNode : public Node
{
public:
    FunctionCallNode() :
        m_pNameVar(nullptr),
        m_pInputVars(nullptr)
    {
    }

    virtual ~FunctionCallNode()
    {
    }

    virtual Node* Clone()
    {
        FunctionCallNode* pNode = new FunctionCallNode;
        pNode->SetNameVar(dynamic_cast<VarNode*>(m_pNameVar->Clone()));
        pNode->SetInputVars(m_pInputVars->CloneList());
        return pNode;
    }

    virtual void Free()
    {
        if (m_pNameVar)
        {
            m_pNameVar->Free();
        }

        if (m_pInputVars)
        {
            m_pInputVars->FreeList();
            m_pInputVars = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitFunctionCallNode(this);
    }

    int GetInputVarCount()
    {
        int count = 0;
        for (Interpreter::Node* pNode = m_pInputVars; pNode; pNode = pNode->GetNext())
        {
            count++;
        }
        return count;
    }

    Node* GetInputVars() { return m_pInputVars; }
    void SetInputVars(Node* pInputVars) { m_pInputVars = pInputVars; }

    VarNode* GetNameVar() { return m_pNameVar; }
    void SetNameVar(VarNode* pVarNode) { m_pNameVar = pVarNode; }

private:
    VarNode* m_pNameVar;
    Node* m_pInputVars;
};

class ReturnNode : public Node
{
public:
    ReturnNode() :
        Node(),
        m_pExpr(nullptr)
    {
    }

    virtual ~ReturnNode()
    {
    }

    virtual Node* Clone()
    {
        ReturnNode* pNode = new ReturnNode;
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }
        return pNode;
    }

    virtual void Free()
    {
        if (m_pExpr != nullptr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }
        Node::Free();
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitReturnNode(this);
    }

    Node* GetExpr() { return m_pExpr; }
    void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

private:
    Node* m_pExpr;
};

class DimNode : public Node
{
public:
    DimNode() :
        Node(),
        m_pDim(nullptr)
    {
    }

    Node* Clone()
    {
        DimNode* pClone = new DimNode;
        for (Node* pNode = m_pDim; pNode != nullptr; pNode = pNode->GetNext())
        {
            pClone->AddDim(pNode->Clone());
        }
        return pClone;
    }

    virtual ~DimNode()
    {
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitDimNode(this);
    }

    void AddDim(Node* pDim)
    {
        if (m_pDim == nullptr)
        {
            m_pDim = pDim;
            return;
        }

        Node* pNode = m_pDim;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext(pDim);
    }

    Node* GetDim()
    {
        return m_pDim;
    }

    void ClearDim()
    {
        m_pDim->FreeList();
        m_pDim = nullptr;
    }

private:
    Node* m_pDim;
};

class PwdNode : public Node
{
public:
    PwdNode() :
        Node()
    {
    }

    virtual ~PwdNode()
    {
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitPwdNode(this);
    }
};

class FunctionsNode : public Node
{
public:
    FunctionsNode() :
        Node()
    {
    }

    virtual ~FunctionsNode()
    {
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitFunctionsNode(this);
    }
};

class VarsCmdNode : public Node
{
public:
    VarsCmdNode() :
        Node()
    {
    }

    virtual ~VarsCmdNode()
    {
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitVarsCmdNode(this);
    }
};

};
