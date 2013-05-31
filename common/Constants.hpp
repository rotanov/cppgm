#pragma once

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
