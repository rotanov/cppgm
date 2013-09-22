#pragma once

#include <exception>

#include "Unicode.hpp"
#include "Constants.hpp"

vector<string>& Split(const string& s, char delimiter, vector<string>& elems);
vector<string> Split(const string& s, char delimiter);

string UTF8CodePointToString(int c);

// decodes codepoints sequence c from start index to end index inclusive
string UTF8CodePointToString(int* c, int start, int end);
bool IsWhiteSpace(int c);
bool IsDigit(int c);
bool IsDChar(int c);
bool IsInAnnexE1(int c);
bool IsInAnnexE2(int c);
bool IsNonDigit(int c);

// given hex digit character c, return its value
int HexCharToValue(int c);

// convert integer [0,15] to hexadecimal digit
char ValueToHexChar(int c);

// use these 3 functions to scan `floating-literals` (see PA2)
// for example PA2Decode_float("12.34") returns "12.34" as a `float` type
float PA2Decode_float(const string& s);
double PA2Decode_double(const string& s);
long double PA2Decode_long_double(const string& s);

// hex dump memory range
string HexDump(const void* pdata, size_t nbytes);

int MatchEscapeSequence(int* where);
int ReplaceEscapeSequences(int* where);

int MatchHexQuad(int* where, int& out);
int MatchPrefix(const string& prefix, int* where);
bool MatchSubsequence(int* source, int from, int begin, int end);

// splits string by |, if we want to match | then we're at fail
// returns true if at least one prefix matched
// if s == "u|ul|uLL" then first match returned keep that in mind
int MatchPrefixes(const string& s, int* where);

class PostTokenError : public std::exception
{
public:
    PostTokenError(const std::string& text)
        : text_(text)
    {

    }

    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT
    {
        return text_.c_str();
    }

private:
    string text_;
};
