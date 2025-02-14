#pragma once

#include "natalie_parser/node/node.hpp"
#include "natalie_parser/node/node_with_args.hpp"
#include "tm/hashmap.hpp"
#include "tm/string.hpp"

namespace NatalieParser {

using namespace TM;

class IdentifierNode : public Node {
public:
    IdentifierNode(const Token &token, bool is_lvar)
        : Node { token }
        , m_is_lvar { is_lvar } { }

    IdentifierNode(const IdentifierNode &other)
        : Node { other.token() }
        , m_is_lvar { other.is_lvar() } { }

    virtual Node *clone() const override {
        return new IdentifierNode(*this);
    }

    virtual Type type() const override { return Type::Identifier; }

    Token::Type token_type() const { return m_token.type(); }

    SharedPtr<String> name() const { return m_token.literal_string(); }

    void append_to_name(char c) {
        auto literal = m_token.literal_string();
        literal->append_char(c);
        m_token.set_literal(literal);
    }

    virtual bool is_callable() const override {
        switch (token_type()) {
        case Token::Type::BareName:
        case Token::Type::Constant:
            return !m_is_lvar;
        default:
            return false;
        }
    }

    bool is_lvar() const { return m_is_lvar; }
    void set_is_lvar(bool is_lvar) { m_is_lvar = is_lvar; }

    size_t nth_ref() const {
        auto str = name();
        size_t len = str->length();
        if (*str == "$0")
            return 0;
        if (len <= 1)
            return 0;
        int ref = 0;
        for (size_t i = 1; i < len; i++) {
            char c = (*str)[i];
            if (i == 1 && c == '0')
                return 0;
            int num = c - 48;
            if (num < 0 || num > 9)
                return 0;
            ref *= 10;
            ref += num;
        }
        return ref;
    }

    void add_to_locals(TM::Hashmap<const char *> &locals) const {
        if (token_type() == Token::Type::BareName)
            locals.set(name()->c_str());
    }

    virtual void transform(Creator *creator) const override {
        if (creator->assignment())
            return transform_assignment(creator);
        switch (token_type()) {
        case Token::Type::BareName:
            if (is_lvar()) {
                creator->set_type("lvar");
                creator->append_symbol(name());
            } else {
                creator->set_type("call");
                creator->append_nil();
                creator->append_symbol(name());
            }
            break;
        case Token::Type::ClassVariable:
            creator->set_type("cvar");
            creator->append_symbol(name());
            break;
        case Token::Type::Constant:
            creator->set_type("const");
            creator->append_symbol(name());
            break;
        case Token::Type::GlobalVariable: {
            auto ref = nth_ref();
            if (ref > 0) {
                creator->set_type("nth_ref");
                creator->append_integer(ref);
            } else {
                creator->set_type("gvar");
                creator->append_symbol(name());
            }
            break;
        }
        case Token::Type::InstanceVariable:
            creator->set_type("ivar");
            creator->append_symbol(name());
            break;
        default:
            TM_UNREACHABLE();
        }
    }

    void transform_assignment(Creator *creator) const {
        switch (token().type()) {
        case Token::Type::BareName:
            creator->set_type("lasgn");
            break;
        case Token::Type::ClassVariable:
            creator->set_type("cvdecl");
            break;
        case Token::Type::Constant:
        case Token::Type::ConstantResolution:
            creator->set_type("cdecl");
            break;
        case Token::Type::GlobalVariable:
            creator->set_type("gasgn");
            break;
        case Token::Type::InstanceVariable:
            creator->set_type("iasgn");
            break;
        default:
            printf("got token type %d\n", (int)token().type());
            TM_UNREACHABLE();
        }
        creator->append_symbol(name());
    }

protected:
    bool m_is_lvar { false };
};
}
