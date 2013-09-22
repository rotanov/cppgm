#include "Utils.hpp"

#include <stdexcept>
#include <sstream>
#include <cwchar>

vector<string>& Split(const string& s, char delimiter, vector<string>& elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delimiter))
    {
        elems.push_back(item);
    }
    return elems;
}

vector<string> Split(const string& s, char delimiter)
{
    vector<string> elements;
    Split(s, delimiter, elements);
    return elements;
}

string UTF8CodePointToString(int c)
{
    unsigned char utf8[5];
    int bytes = UTF8Encode(c, utf8);
    utf8[bytes] = 0;
    return string(reinterpret_cast<const char*>(utf8));
}

string UTF8CodePointToString(int* c, int start, int end)
{
    int i = 0;
    unsigned char* utf8 = new unsigned char[(end - start + 1) * 4 + 1];

    while (start <= end)
    {
        i += UTF8Encode(c[start], utf8 + i);
        start++;
    }

    utf8[i] = 0;

    string r(reinterpret_cast<const char*>(utf8));

    delete [] utf8;

    return r;
}

bool IsWhiteSpace(int c)
{
    return c == ' ' || c == '\t' || c == '\v' || c == '\f';
}

bool IsDigit(int c)
{
    return '0' <= c  && c <= '9';
}

bool IsDChar(int c)
{
    if (IsWhiteSpace(c)
     || c == '('
     || c == ')'
     || c == '\\')
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool IsInAnnexE1(int c)
{
    for (auto p : AnnexE1_Allowed_RangesSorted)
    {
        if (p.first > c)
        {
            return false;
        }
        else if (c <= p.second)
        {
            return true;
        }
    }
    return false;
}

// copypasted like a bad person
bool IsInAnnexE2(int c)
{
    for (auto p : AnnexE2_DisallowedInitially_RangesSorted)
    {
        if (p.first > c)
        {
            return false;
        }
        else if (c <= p.second)
        {
            return true;
        }
    }
    return false;
}

bool IsNonDigit(int c)
{
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || (c == '_')
        || IsInAnnexE1(c);
}

int HexCharToValue(int c)
{
    switch (c)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': return 10;
    case 'a': return 10;
    case 'B': return 11;
    case 'b': return 11;
    case 'C': return 12;
    case 'c': return 12;
    case 'D': return 13;
    case 'd': return 13;
    case 'E': return 14;
    case 'e': return 14;
    case 'F': return 15;
    case 'f': return 15;
    default: throw logic_error("HexCharToValue of nonhex char");
    }
}

float PA2Decode_float(const string& s)
{
    istringstream iss(s);
    float x;
    iss >> x;
    return x;
}

double PA2Decode_double(const string& s)
{
    istringstream iss(s);
    double x;
    iss >> x;
    return x;
}

long double PA2Decode_long_double(const string& s)
{
    istringstream iss(s);
    long double x;
    iss >> x;
    return x;
}

// convert integer [0,15] to hexadecimal digit
char ValueToHexChar(int c)
{
    switch (c)
    {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'A';
    case 11: return 'B';
    case 12: return 'C';
    case 13: return 'D';
    case 14: return 'E';
    case 15: return 'F';
    default: throw logic_error("ValueToHexChar of nonhex value");
    }
}

string HexDump(const void* pdata, size_t nbytes)
{
    unsigned char* p = (unsigned char*) pdata;

    string s(nbytes*2, '?');

    for (size_t i = 0; i < nbytes; i++)
    {
        s[2*i+0] = ValueToHexChar((p[i] & 0xF0) >> 4);
        s[2*i+1] = ValueToHexChar((p[i] & 0x0F) >> 0);
    }

    return s;
}

int MatchEscapeSequence(int* where)
{
    int c = where[0];
    if (SimpleEscapeSequence_CodePoints.count(c) > 0)
    {
        return 1;
    }
    else if ('0' <= c && c <= '7')
    {
        int i = 0;
        while (i < 3 && '0' <= where[i] && where[i] <= '7')
        {
            i++;
        }
        return i;
    }
    else if (c == 'x')
    {
        int i = 0;
        while (HexadecimalCharachters.count(where[i + 1]) != 0)
        {
            i++;
        }
        if (i == 0)
        {
            return 0;
        }
        else
        {
            return i + 1;
        }
    }
    else
    {
        return 0;
    }
}

int ReplaceEscapeSequences(int* where)
{
    int* write = where;
    int* read = where;
    int c = read[0];
    int state = 0;

    while (c != EndOfFile && c != 0)
    {
        switch (state)
        {
//------------------------------------------------------------------------------
        case 0:
            if (c == '\\')
            {
                state = 1;
                read++;
            }
            else
            {
                write[0] = read[0];
                write++;
                read++;
            }
        break;
//------------------------------------------------------------------------------
        case 1:
            if (SimpleEscapeSequence_CodePoints.count(c) > 0)
            {
                write[0] = SimpleEscapeSequence_Replacements.find(c)->second;
                write++;
                state = 0;
                read++;
            }
            else if (c == 'x')
            {
                long int codePoint = wcstol(reinterpret_cast<wchar_t*>(read + 1),
                                            reinterpret_cast<wchar_t**>(&read), 16);
                write[0] = codePoint;
                write++;
                state = 0;
            }
            else
            {
                long int codePoint = wcstol(reinterpret_cast<wchar_t*>(read),
                                            reinterpret_cast<wchar_t**>(&read), 8);
                write[0] = codePoint;
                write++;
                state = 0;
            }
        break;
        }
        c = read[0];
    }
    write[0] = c;
    return write - where;
}

int MatchHexQuad(int* where, int& out)
{
    out = 0;
    for (int i = 0; i < 4; i++)
    {
        int c = *(where + i);
        if (HexadecimalCharachters.count(c) == 0)
        {
            return 0;
        }
        else
        {
            out |= HexCharToValue(c) << ((3 - i) * 4);
        }
    }
    return 4;
}

int MatchPrefix(const string& prefix, int* where)
{
    int* i = where;
    for (auto c : prefix)
    {
        if (i[0] != c || i[0] == EndOfFile)
        {
            return 0;
        }
        else
        {
            i++;
        }
    }
    return i - where;
}

bool MatchSubsequence(int* source, int from, int begin, int end)
{
    int i = 0;
    while (source[i] != EndOfFile
        && source[from + i] == source[begin + i]
        && i <= end - begin)
    {
        i++;
    }
    return i - 1 == end - begin;
}

int MatchPrefixes(const string& s, int* where)
{
    vector<string> prefixes;
    Split(s, '|', prefixes);
    for (auto prefix : prefixes)
    {
        int matched = MatchPrefix(prefix, where);
        if (matched == prefix.size())
        {
            return matched;
        }
    }
    return 0;
}
