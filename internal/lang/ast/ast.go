// FILE: internal/lang/ast/ast.go

package ast

import (
	"bytes"
	//"strings"

	"github.com/DauletBai/tenge/internal/lang/token"
	"github.com/shopspring/decimal"
)

// Node represents a node in the Abstract Syntax Tree.
type Node interface {
	TokenLiteral() string
	String() string
}

// Statement represents a statement node.
type Statement interface {
	Node
	statementNode()
}

// Expression represents an expression node.
type Expression interface {
	Node
	expressionNode()
}

// Program is the root node of the AST.
type Program struct {
	Statements []Statement
}

func (p *Program) String() string {
	var out bytes.Buffer
	for _, s := range p.Statements {
		out.WriteString(s.String())
	}
	return out.String()
}
func (p *Program) TokenLiteral() string {
	if len(p.Statements) > 0 {
		return p.Statements[0].TokenLiteral()
	}
	return ""
}

// Identifier represents an identifier (e.g., a variable name).
type Identifier struct {
	Token token.Token // The IDENT token
	Value string
}

func (i *Identifier) expressionNode()      {}
func (i *Identifier) TokenLiteral() string { return i.Token.Literal }
func (i *Identifier) String() string       { return i.Value }

// TypeNode represents a type annotation (e.g., ': san').
type TypeNode struct {
	Token token.Token // The type token (e.g., token.SAN)
}

func (tn *TypeNode) expressionNode()      {}
func (tn *TypeNode) TokenLiteral() string { return tn.Token.Literal }
func (tn *TypeNode) String() string       { return tn.Token.Literal }

// BekitStatement represents a constant declaration (`bekit`).
type BekitStatement struct {
	Token token.Token // The 'bekit' token
	Name  *Identifier
	Type  *TypeNode
	Value Expression
}

func (bs *BekitStatement) statementNode()       {}
func (bs *BekitStatement) TokenLiteral() string { return bs.Token.Literal }
func (bs *BekitStatement) String() string       { return "bekit statement" } // Placeholder

// JasauStatement represents a variable declaration (`jasau`).
type JasauStatement struct {
	Token token.Token // The 'jasau' token
	Name  *Identifier
	Type  *TypeNode
	Value Expression
}

func (js *JasauStatement) statementNode()       {}
func (js *JasauStatement) TokenLiteral() string { return js.Token.Literal }
func (js *JasauStatement) String() string       { return "jasau statement" } // Placeholder

// QaytarStatement represents a return statement (`qaytar`).
type QaytarStatement struct {
	Token       token.Token // The 'qaytar' token
	ReturnValue Expression
}

func (qs *QaytarStatement) statementNode()       {}
func (qs *QaytarStatement) TokenLiteral() string { return qs.Token.Literal }
func (qs *QaytarStatement) String() string       { return "qaytar statement" } // Placeholder

// ExpressionStatement is a statement that consists of a single expression.
type ExpressionStatement struct {
	Token      token.Token
	Expression Expression
}

func (es *ExpressionStatement) statementNode()       {}
func (es *ExpressionStatement) TokenLiteral() string { return es.Token.Literal }
func (es *ExpressionStatement) String() string {
	if es.Expression != nil {
		return es.Expression.String()
	}
	return ""
}

// --- Expression Nodes ---

// SanLiteral represents an integer literal.
type SanLiteral struct {
	Token token.Token
	Value int64
}

func (sl *SanLiteral) expressionNode()      {}
func (sl *SanLiteral) TokenLiteral() string { return sl.Token.Literal }
func (sl *SanLiteral) String() string       { return sl.Token.Literal }

// AqshaLiteral represents a decimal literal.
type AqshaLiteral struct {
	Token token.Token
	Value decimal.Decimal
}

func (al *AqshaLiteral) expressionNode()      {}
func (al *AqshaLiteral) TokenLiteral() string { return al.Token.Literal }
func (al *AqshaLiteral) String() string       { return al.Token.Literal }

// AqıqatLiteral represents a boolean literal (`jan` or `j'n`).
type AqıqatLiteral struct {
	Token token.Token
	Value bool
}

func (al *AqıqatLiteral) expressionNode()      {}
func (al *AqıqatLiteral) TokenLiteral() string { return al.Token.Literal }
func (al *AqıqatLiteral) String() string       { return al.Token.Literal }
