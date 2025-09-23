// FILE: internal/lang/token/token.go

package token

import "fmt"

type TokenType string

type Token struct {
	Type    TokenType
	Literal string
}

func (t Token) String() string {
	return fmt.Sprintf("Token{Type:%s, Literal:`%s`}", t.Type, t.Literal)
}

// All token types are now based on the tenge language keywords.
const (
	// Special Tokens
	ILLEGAL = "ILLEGAL" // Represents a token we don't know
	EOF     = "EOF"     // End of File

	// Identifiers & Literals
	IDENT     = "IDENT"     // a, myVar, etc.
	SAN_LIT   = "SAN_LIT"   // 123
	AQSHA_LIT = "AQSHA_LIT" // 12.34
	JOL_LIT   = "JOL_LIT"   // "hello"

	// Keywords - Official Latin Kazakh
	JASA    = "jasau"   // create/declare
	BEKIT   = "bekit"   // constant
	ATQAR   = "atqar"   // function
	QAYTAR  = "qaytar"  // return
	EGER    = "eger"    // if
	AITPESE = "aitpese" // else
	AZIRSHE = "azirshe" // while
	JAN     = "jan"     // true
	JIN     = "jin"     // false
	KORSET  = "korset"  // show/print

	// Types - Official Latin Kazakh
	SAN    = "san"    // number/int
	AQSHA  = "aqsha"  // money/decimal
	JOL    = "jol"    // string
	TANBA  = "tanba"  // equal
	AQIQAT = "aqiqat" // boolean
	JIM    = "jim"    // arrow

	// Operators - Kazakh translations
	TAYINDAU = "="  // assignment (тағайындау)
	KOSU     = "+"  // addition (қосу)
	AZAYTU   = "-"  // subtraction (азайту)
	KOBEYTU  = "*"  // multiplication (көбейту)
	BOLU     = "/"  // division (бөлу)
	TEN      = "==" // equality (тең)
	ULKEN    = ">"  // greater than (үлкен)

	// Delimiters - Kazakh translations
	VIRGUL    = ","  // comma (үтір)
	EKI_NUQTA = ":"  // colon (екі нүкте)
	SOL_JAI   = "("  // left parenthesis (сол жақ)
	ON_JAI    = ")"  // right parenthesis (оң жақ)
	SOL_KOSHA = "["  // left bracket (сол қосша)
	ON_KOSHA  = "]"  // right bracket (оң қосша)
	OK        = "->" // arrow (оқ)
)
