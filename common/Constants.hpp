#pragma once

#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>

using namespace std;

constexpr int EndOfFile = -1;

// See C++ standard 2.11 Identifiers and Appendix/Annex E.1
const vector<pair<int, int>> AnnexE1_Allowed_RangesSorted =
{
	{0xA8,0xA8},
	{0xAA,0xAA},
	{0xAD,0xAD},
	{0xAF,0xAF},
	{0xB2,0xB5},
	{0xB7,0xBA},
	{0xBC,0xBE},
	{0xC0,0xD6},
	{0xD8,0xF6},
	{0xF8,0xFF},
	{0x100,0x167F},
	{0x1681,0x180D},
	{0x180F,0x1FFF},
	{0x200B,0x200D},
	{0x202A,0x202E},
	{0x203F,0x2040},
	{0x2054,0x2054},
	{0x2060,0x206F},
	{0x2070,0x218F},
	{0x2460,0x24FF},
	{0x2776,0x2793},
	{0x2C00,0x2DFF},
	{0x2E80,0x2FFF},
	{0x3004,0x3007},
	{0x3021,0x302F},
	{0x3031,0x303F},
	{0x3040,0xD7FF},
	{0xF900,0xFD3D},
	{0xFD40,0xFDCF},
	{0xFDF0,0xFE44},
	{0xFE47,0xFFFD},
	{0x10000,0x1FFFD},
	{0x20000,0x2FFFD},
	{0x30000,0x3FFFD},
	{0x40000,0x4FFFD},
    {0x50000,0x5FFFD},
	{0x60000,0x6FFFD},
	{0x70000,0x7FFFD},
	{0x80000,0x8FFFD},
	{0x90000,0x9FFFD},
	{0xA0000,0xAFFFD},
	{0xB0000,0xBFFFD},
	{0xC0000,0xCFFFD},
	{0xD0000,0xDFFFD},
	{0xE0000,0xEFFFD}
};

// See C++ standard 2.11 Identifiers and Appendix/Annex E.2
const vector<pair<int, int>> AnnexE2_DisallowedInitially_RangesSorted =
{
	{0x300,0x36F},
	{0x1DC0,0x1DFF},
	{0x20D0,0x20FF},
	{0xFE20,0xFE2F}
};

// See C++ standard 2.13 Operators and punctuators
const unordered_set<string> Digraph_IdentifierLike_Operators =
{
	"new", "delete", "and", "and_eq", "bitand",
	"bitor", "compl", "not", "not_eq", "or",
	"or_eq", "xor", "xor_eq"
};

// See `simple-escape-sequence` grammar
const unordered_set<int> SimpleEscapeSequence_CodePoints =
{
    '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v',
};

const unordered_map<int, int> SimpleEscapeSequence_Replacements =
{
    {'\'', '\''}, {'"', '"'}, {'?', '?'}, {'\\', '\\'}, {'a', '\a'}, {'b', '\b'},
    {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'}, {'v', '\v'},
};

const unordered_set<int> HexadecimalCharachters =
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F',
};

const unordered_map<int, int> TrigraphReplacement =
{
    {'=', '#',}, {'/', '\\',}, {'\'', '^',}, {'(', '[',}, {')', ']',},
    {'!', '|',}, {'<', '{',}, {'>', '}',}, {'-', '~',},
};

const unordered_set<string> Punctuation4 =
{
    "%:%:",
};

const unordered_set<string> Punctuation3 =
{
    "<<=", ">>=", "...", "->*", /* surrogate */ "<::",
};

const unordered_set<string> Punctuation2 =
{
    "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<", ">>", "<=", ">=", "&&",
    "==", "!=", "||", "++", "--", "->", ".*", "::", "##", "<:", ":>", "<%", "%>", "%:"
};

const unordered_set<string> Punctuation1 =
{
    "{", "}", "[", "]", "#", "(", ")", ";", ":", "?", ".", "+", "-", "*", "/",
    "%", "^", "&", "|", "~", "!", "=", "<", ">", ",",
};

const vector<unordered_set<string>> Punctuation =
{
    Punctuation1, Punctuation2, Punctuation3, Punctuation4,
};

////////////////////////////////////////////////////////////////////////////////

// See 3.9.1: Fundamental Types
enum EFundamentalType
{
    // 3.9.1.2
    FT_SIGNED_CHAR,
    FT_SHORT_INT,
    FT_INT,
    FT_LONG_INT,
    FT_LONG_LONG_INT,

    // 3.9.1.3
    FT_UNSIGNED_CHAR,
    FT_UNSIGNED_SHORT_INT,
    FT_UNSIGNED_INT,
    FT_UNSIGNED_LONG_INT,
    FT_UNSIGNED_LONG_LONG_INT,

    // 3.9.1.1 / 3.9.1.5
    FT_WCHAR_T,
    FT_CHAR,
    FT_CHAR16_T,
    FT_CHAR32_T,

    // 3.9.1.6
    FT_BOOL,

    // 3.9.1.8
    FT_FLOAT,
    FT_DOUBLE,
    FT_LONG_DOUBLE,

    // 3.9.1.9
    FT_VOID,

    // 3.9.1.10
    FT_NULLPTR_T
};

// FundamentalTypeOf: convert fundamental type T to EFundamentalType
// for example: `FundamentalTypeOf<long int>()` will return `FT_LONG_INT`
template<typename T> constexpr EFundamentalType FundamentalTypeOf();
template<> constexpr EFundamentalType FundamentalTypeOf<signed char>() { return FT_SIGNED_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<short int>() { return FT_SHORT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<int>() { return FT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<long int>() { return FT_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<long long int>() { return FT_LONG_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned char>() { return FT_UNSIGNED_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned short int>() { return FT_UNSIGNED_SHORT_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned int>() { return FT_UNSIGNED_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned long int>() { return FT_UNSIGNED_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<unsigned long long int>() { return FT_UNSIGNED_LONG_LONG_INT; }
template<> constexpr EFundamentalType FundamentalTypeOf<wchar_t>() { return FT_WCHAR_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<char>() { return FT_CHAR; }
template<> constexpr EFundamentalType FundamentalTypeOf<char16_t>() { return FT_CHAR16_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<char32_t>() { return FT_CHAR32_T; }
template<> constexpr EFundamentalType FundamentalTypeOf<bool>() { return FT_BOOL; }
template<> constexpr EFundamentalType FundamentalTypeOf<float>() { return FT_FLOAT; }
template<> constexpr EFundamentalType FundamentalTypeOf<double>() { return FT_DOUBLE; }
template<> constexpr EFundamentalType FundamentalTypeOf<long double>() { return FT_LONG_DOUBLE; }
template<> constexpr EFundamentalType FundamentalTypeOf<void>() { return FT_VOID; }
template<> constexpr EFundamentalType FundamentalTypeOf<nullptr_t>() { return FT_NULLPTR_T; }

// convert EFundamentalType to a source code
const map<EFundamentalType, string> FundamentalTypeToStringMap
{
    {FT_SIGNED_CHAR, "signed char"},
    {FT_SHORT_INT, "short int"},
    {FT_INT, "int"},
    {FT_LONG_INT, "long int"},
    {FT_LONG_LONG_INT, "long long int"},
    {FT_UNSIGNED_CHAR, "unsigned char"},
    {FT_UNSIGNED_SHORT_INT, "unsigned short int"},
    {FT_UNSIGNED_INT, "unsigned int"},
    {FT_UNSIGNED_LONG_INT, "unsigned long int"},
    {FT_UNSIGNED_LONG_LONG_INT, "unsigned long long int"},
    {FT_WCHAR_T, "wchar_t"},
    {FT_CHAR, "char"},
    {FT_CHAR16_T, "char16_t"},
    {FT_CHAR32_T, "char32_t"},
    {FT_BOOL, "bool"},
    {FT_FLOAT, "float"},
    {FT_DOUBLE, "double"},
    {FT_LONG_DOUBLE, "long double"},
    {FT_VOID, "void"},
    {FT_NULLPTR_T, "nullptr_t"}
};

// token type enum for `simples`
enum ETokenType
{
    // keywords
    KW_ALIGNAS,
    KW_ALIGNOF,
    KW_ASM,
    KW_AUTO,
    KW_BOOL,
    KW_BREAK,
    KW_CASE,
    KW_CATCH,
    KW_CHAR,
    KW_CHAR16_T,
    KW_CHAR32_T,
    KW_CLASS,
    KW_CONST,
    KW_CONSTEXPR,
    KW_CONST_CAST,
    KW_CONTINUE,
    KW_DECLTYPE,
    KW_DEFAULT,
    KW_DELETE,
    KW_DO,
    KW_DOUBLE,
    KW_DYNAMIC_CAST,
    KW_ELSE,
    KW_ENUM,
    KW_EXPLICIT,
    KW_EXPORT,
    KW_EXTERN,
    KW_FALSE,
    KW_FLOAT,
    KW_FOR,
    KW_FRIEND,
    KW_GOTO,
    KW_IF,
    KW_INLINE,
    KW_INT,
    KW_LONG,
    KW_MUTABLE,
    KW_NAMESPACE,
    KW_NEW,
    KW_NOEXCEPT,
    KW_NULLPTR,
    KW_OPERATOR,
    KW_PRIVATE,
    KW_PROTECTED,
    KW_PUBLIC,
    KW_REGISTER,
    KW_REINTERPET_CAST,
    KW_RETURN,
    KW_SHORT,
    KW_SIGNED,
    KW_SIZEOF,
    KW_STATIC,
    KW_STATIC_ASSERT,
    KW_STATIC_CAST,
    KW_STRUCT,
    KW_SWITCH,
    KW_TEMPLATE,
    KW_THIS,
    KW_THREAD_LOCAL,
    KW_THROW,
    KW_TRUE,
    KW_TRY,
    KW_TYPEDEF,
    KW_TYPEID,
    KW_TYPENAME,
    KW_UNION,
    KW_UNSIGNED,
    KW_USING,
    KW_VIRTUAL,
    KW_VOID,
    KW_VOLATILE,
    KW_WCHAR_T,
    KW_WHILE,

    // operators/punctuation
    OP_LBRACE,
    OP_RBRACE,
    OP_LSQUARE,
    OP_RSQUARE,
    OP_LPAREN,
    OP_RPAREN,
    OP_BOR,
    OP_XOR,
    OP_COMPL,
    OP_AMP,
    OP_LNOT,
    OP_SEMICOLON,
    OP_COLON,
    OP_DOTS,
    OP_QMARK,
    OP_COLON2,
    OP_DOT,
    OP_DOTSTAR,
    OP_PLUS,
    OP_MINUS,
    OP_STAR,
    OP_DIV,
    OP_MOD,
    OP_ASS,
    OP_LT,
    OP_GT,
    OP_PLUSASS,
    OP_MINUSASS,
    OP_STARASS,
    OP_DIVASS,
    OP_MODASS,
    OP_XORASS,
    OP_BANDASS,
    OP_BORASS,
    OP_LSHIFT,
    OP_RSHIFT,
    OP_RSHIFTASS,
    OP_LSHIFTASS,
    OP_EQ,
    OP_NE,
    OP_LE,
    OP_GE,
    OP_LAND,
    OP_LOR,
    OP_INC,
    OP_DEC,
    OP_COMMA,
    OP_ARROWSTAR,
    OP_ARROW,
};

// StringToETokenTypeMap map of `simple` `preprocessing-tokens` to ETokenType
const unordered_map<string, ETokenType> StringToTokenTypeMap =
{
    // keywords
    {"alignas", KW_ALIGNAS},
    {"alignof", KW_ALIGNOF},
    {"asm", KW_ASM},
    {"auto", KW_AUTO},
    {"bool", KW_BOOL},
    {"break", KW_BREAK},
    {"case", KW_CASE},
    {"catch", KW_CATCH},
    {"char", KW_CHAR},
    {"char16_t", KW_CHAR16_T},
    {"char32_t", KW_CHAR32_T},
    {"class", KW_CLASS},
    {"const", KW_CONST},
    {"constexpr", KW_CONSTEXPR},
    {"const_cast", KW_CONST_CAST},
    {"continue", KW_CONTINUE},
    {"decltype", KW_DECLTYPE},
    {"default", KW_DEFAULT},
    {"delete", KW_DELETE},
    {"do", KW_DO},
    {"double", KW_DOUBLE},
    {"dynamic_cast", KW_DYNAMIC_CAST},
    {"else", KW_ELSE},
    {"enum", KW_ENUM},
    {"explicit", KW_EXPLICIT},
    {"export", KW_EXPORT},
    {"extern", KW_EXTERN},
    {"false", KW_FALSE},
    {"float", KW_FLOAT},
    {"for", KW_FOR},
    {"friend", KW_FRIEND},
    {"goto", KW_GOTO},
    {"if", KW_IF},
    {"inline", KW_INLINE},
    {"int", KW_INT},
    {"long", KW_LONG},
    {"mutable", KW_MUTABLE},
    {"namespace", KW_NAMESPACE},
    {"new", KW_NEW},
    {"noexcept", KW_NOEXCEPT},
    {"nullptr", KW_NULLPTR},
    {"operator", KW_OPERATOR},
    {"private", KW_PRIVATE},
    {"protected", KW_PROTECTED},
    {"public", KW_PUBLIC},
    {"register", KW_REGISTER},
    {"reinterpret_cast", KW_REINTERPET_CAST},
    {"return", KW_RETURN},
    {"short", KW_SHORT},
    {"signed", KW_SIGNED},
    {"sizeof", KW_SIZEOF},
    {"static", KW_STATIC},
    {"static_assert", KW_STATIC_ASSERT},
    {"static_cast", KW_STATIC_CAST},
    {"struct", KW_STRUCT},
    {"switch", KW_SWITCH},
    {"template", KW_TEMPLATE},
    {"this", KW_THIS},
    {"thread_local", KW_THREAD_LOCAL},
    {"throw", KW_THROW},
    {"true", KW_TRUE},
    {"try", KW_TRY},
    {"typedef", KW_TYPEDEF},
    {"typeid", KW_TYPEID},
    {"typename", KW_TYPENAME},
    {"union", KW_UNION},
    {"unsigned", KW_UNSIGNED},
    {"using", KW_USING},
    {"virtual", KW_VIRTUAL},
    {"void", KW_VOID},
    {"volatile", KW_VOLATILE},
    {"wchar_t", KW_WCHAR_T},
    {"while", KW_WHILE},

    // operators/punctuation
    {"{", OP_LBRACE},
    {"<%", OP_LBRACE},
    {"}", OP_RBRACE},
    {"%>", OP_RBRACE},
    {"[", OP_LSQUARE},
    {"<:", OP_LSQUARE},
    {"]", OP_RSQUARE},
    {":>", OP_RSQUARE},
    {"(", OP_LPAREN},
    {")", OP_RPAREN},
    {"|", OP_BOR},
    {"bitor", OP_BOR},
    {"^", OP_XOR},
    {"xor", OP_XOR},
    {"~", OP_COMPL},
    {"compl", OP_COMPL},
    {"&", OP_AMP},
    {"bitand", OP_AMP},
    {"!", OP_LNOT},
    {"not", OP_LNOT},
    {";", OP_SEMICOLON},
    {":", OP_COLON},
    {"...", OP_DOTS},
    {"?", OP_QMARK},
    {"::", OP_COLON2},
    {".", OP_DOT},
    {".*", OP_DOTSTAR},
    {"+", OP_PLUS},
    {"-", OP_MINUS},
    {"*", OP_STAR},
    {"/", OP_DIV},
    {"%", OP_MOD},
    {"=", OP_ASS},
    {"<", OP_LT},
    {">", OP_GT},
    {"+=", OP_PLUSASS},
    {"-=", OP_MINUSASS},
    {"*=", OP_STARASS},
    {"/=", OP_DIVASS},
    {"%=", OP_MODASS},
    {"^=", OP_XORASS},
    {"xor_eq", OP_XORASS},
    {"&=", OP_BANDASS},
    {"and_eq", OP_BANDASS},
    {"|=", OP_BORASS},
    {"or_eq", OP_BORASS},
    {"<<", OP_LSHIFT},
    {">>", OP_RSHIFT},
    {">>=", OP_RSHIFTASS},
    {"<<=", OP_LSHIFTASS},
    {"==", OP_EQ},
    {"!=", OP_NE},
    {"not_eq", OP_NE},
    {"<=", OP_LE},
    {">=", OP_GE},
    {"&&", OP_LAND},
    {"and", OP_LAND},
    {"||", OP_LOR},
    {"or", OP_LOR},
    {"++", OP_INC},
    {"--", OP_DEC},
    {",", OP_COMMA},
    {"->*", OP_ARROWSTAR},
    {"->", OP_ARROW}
};

// map of enum to string
const map<ETokenType, string> TokenTypeToStringMap =
{
    {KW_ALIGNAS, "KW_ALIGNAS"},
    {KW_ALIGNOF, "KW_ALIGNOF"},
    {KW_ASM, "KW_ASM"},
    {KW_AUTO, "KW_AUTO"},
    {KW_BOOL, "KW_BOOL"},
    {KW_BREAK, "KW_BREAK"},
    {KW_CASE, "KW_CASE"},
    {KW_CATCH, "KW_CATCH"},
    {KW_CHAR, "KW_CHAR"},
    {KW_CHAR16_T, "KW_CHAR16_T"},
    {KW_CHAR32_T, "KW_CHAR32_T"},
    {KW_CLASS, "KW_CLASS"},
    {KW_CONST, "KW_CONST"},
    {KW_CONSTEXPR, "KW_CONSTEXPR"},
    {KW_CONST_CAST, "KW_CONST_CAST"},
    {KW_CONTINUE, "KW_CONTINUE"},
    {KW_DECLTYPE, "KW_DECLTYPE"},
    {KW_DEFAULT, "KW_DEFAULT"},
    {KW_DELETE, "KW_DELETE"},
    {KW_DO, "KW_DO"},
    {KW_DOUBLE, "KW_DOUBLE"},
    {KW_DYNAMIC_CAST, "KW_DYNAMIC_CAST"},
    {KW_ELSE, "KW_ELSE"},
    {KW_ENUM, "KW_ENUM"},
    {KW_EXPLICIT, "KW_EXPLICIT"},
    {KW_EXPORT, "KW_EXPORT"},
    {KW_EXTERN, "KW_EXTERN"},
    {KW_FALSE, "KW_FALSE"},
    {KW_FLOAT, "KW_FLOAT"},
    {KW_FOR, "KW_FOR"},
    {KW_FRIEND, "KW_FRIEND"},
    {KW_GOTO, "KW_GOTO"},
    {KW_IF, "KW_IF"},
    {KW_INLINE, "KW_INLINE"},
    {KW_INT, "KW_INT"},
    {KW_LONG, "KW_LONG"},
    {KW_MUTABLE, "KW_MUTABLE"},
    {KW_NAMESPACE, "KW_NAMESPACE"},
    {KW_NEW, "KW_NEW"},
    {KW_NOEXCEPT, "KW_NOEXCEPT"},
    {KW_NULLPTR, "KW_NULLPTR"},
    {KW_OPERATOR, "KW_OPERATOR"},
    {KW_PRIVATE, "KW_PRIVATE"},
    {KW_PROTECTED, "KW_PROTECTED"},
    {KW_PUBLIC, "KW_PUBLIC"},
    {KW_REGISTER, "KW_REGISTER"},
    {KW_REINTERPET_CAST, "KW_REINTERPET_CAST"},
    {KW_RETURN, "KW_RETURN"},
    {KW_SHORT, "KW_SHORT"},
    {KW_SIGNED, "KW_SIGNED"},
    {KW_SIZEOF, "KW_SIZEOF"},
    {KW_STATIC, "KW_STATIC"},
    {KW_STATIC_ASSERT, "KW_STATIC_ASSERT"},
    {KW_STATIC_CAST, "KW_STATIC_CAST"},
    {KW_STRUCT, "KW_STRUCT"},
    {KW_SWITCH, "KW_SWITCH"},
    {KW_TEMPLATE, "KW_TEMPLATE"},
    {KW_THIS, "KW_THIS"},
    {KW_THREAD_LOCAL, "KW_THREAD_LOCAL"},
    {KW_THROW, "KW_THROW"},
    {KW_TRUE, "KW_TRUE"},
    {KW_TRY, "KW_TRY"},
    {KW_TYPEDEF, "KW_TYPEDEF"},
    {KW_TYPEID, "KW_TYPEID"},
    {KW_TYPENAME, "KW_TYPENAME"},
    {KW_UNION, "KW_UNION"},
    {KW_UNSIGNED, "KW_UNSIGNED"},
    {KW_USING, "KW_USING"},
    {KW_VIRTUAL, "KW_VIRTUAL"},
    {KW_VOID, "KW_VOID"},
    {KW_VOLATILE, "KW_VOLATILE"},
    {KW_WCHAR_T, "KW_WCHAR_T"},
    {KW_WHILE, "KW_WHILE"},
    {OP_LBRACE, "OP_LBRACE"},
    {OP_RBRACE, "OP_RBRACE"},
    {OP_LSQUARE, "OP_LSQUARE"},
    {OP_RSQUARE, "OP_RSQUARE"},
    {OP_LPAREN, "OP_LPAREN"},
    {OP_RPAREN, "OP_RPAREN"},
    {OP_BOR, "OP_BOR"},
    {OP_XOR, "OP_XOR"},
    {OP_COMPL, "OP_COMPL"},
    {OP_AMP, "OP_AMP"},
    {OP_LNOT, "OP_LNOT"},
    {OP_SEMICOLON, "OP_SEMICOLON"},
    {OP_COLON, "OP_COLON"},
    {OP_DOTS, "OP_DOTS"},
    {OP_QMARK, "OP_QMARK"},
    {OP_COLON2, "OP_COLON2"},
    {OP_DOT, "OP_DOT"},
    {OP_DOTSTAR, "OP_DOTSTAR"},
    {OP_PLUS, "OP_PLUS"},
    {OP_MINUS, "OP_MINUS"},
    {OP_STAR, "OP_STAR"},
    {OP_DIV, "OP_DIV"},
    {OP_MOD, "OP_MOD"},
    {OP_ASS, "OP_ASS"},
    {OP_LT, "OP_LT"},
    {OP_GT, "OP_GT"},
    {OP_PLUSASS, "OP_PLUSASS"},
    {OP_MINUSASS, "OP_MINUSASS"},
    {OP_STARASS, "OP_STARASS"},
    {OP_DIVASS, "OP_DIVASS"},
    {OP_MODASS, "OP_MODASS"},
    {OP_XORASS, "OP_XORASS"},
    {OP_BANDASS, "OP_BANDASS"},
    {OP_BORASS, "OP_BORASS"},
    {OP_LSHIFT, "OP_LSHIFT"},
    {OP_RSHIFT, "OP_RSHIFT"},
    {OP_RSHIFTASS, "OP_RSHIFTASS"},
    {OP_LSHIFTASS, "OP_LSHIFTASS"},
    {OP_EQ, "OP_EQ"},
    {OP_NE, "OP_NE"},
    {OP_LE, "OP_LE"},
    {OP_GE, "OP_GE"},
    {OP_LAND, "OP_LAND"},
    {OP_LOR, "OP_LOR"},
    {OP_INC, "OP_INC"},
    {OP_DEC, "OP_DEC"},
    {OP_COMMA, "OP_COMMA"},
    {OP_ARROWSTAR, "OP_ARROWSTAR"},
    {OP_ARROW, "OP_ARROW"}
};
