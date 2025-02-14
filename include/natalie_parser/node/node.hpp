#pragma once

#include "natalie_parser/creator.hpp"
#include "natalie_parser/token.hpp"

namespace NatalieParser {

class Node {
public:
    enum class Type {
        Alias,
        Arg,
        Array,
        ArrayPattern,
        Assignment,
        Begin,
        BeginRescue,
        Block,
        BlockPass,
        Break,
        Call,
        Case,
        CaseIn,
        CaseWhen,
        Class,
        Colon2,
        Colon3,
        Constant,
        Def,
        Defined,
        EvaluateToString,
        False,
        Float,
        Hash,
        HashPattern,
        Identifier,
        If,
        Integer,
        Iter,
        InterpolatedRegexp,
        InterpolatedShell,
        InterpolatedString,
        KeywordArg,
        KeywordSplat,
        LogicalAnd,
        LogicalOr,
        Match,
        Module,
        MultipleAssignment,
        MultipleAssignmentArg,
        Next,
        Nil,
        NilSexp,
        Not,
        OpAssign,
        OpAssignAccessor,
        OpAssignAnd,
        OpAssignOr,
        Pin,
        Range,
        Regexp,
        Return,
        SafeCall,
        Sclass,
        Self,
        Shell,
        Splat,
        SplatValue,
        StabbyProc,
        String,
        Super,
        Symbol,
        ToArray,
        True,
        Until,
        While,
        Yield,
    };

    Node(const Token &token)
        : m_token { token } { }

    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;

    virtual ~Node() { }

    virtual Type type() const = 0;

    virtual bool is_callable() const { return false; }

    virtual Node *clone() const {
        printf("Need to implement Node::clone() in a subclass (type=%d)...\n", (int)type());
        TM_UNREACHABLE();
    }

    virtual void transform(Creator *creator) const {
        creator->set_type("NOT_YET_IMPLEMENTED");
        creator->append_integer((int)type());
    }

    SharedPtr<String> file() const { return m_token.file(); }
    size_t line() const { return m_token.line(); }
    size_t column() const { return m_token.column(); }

    const Token &token() const { return m_token; }

protected:
    Token m_token {};
};

}
