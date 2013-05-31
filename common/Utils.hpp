#pragma once

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

// decodes codepoints sequence c from start index to end index inclusive
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
}

bool IsNonDigit(int c)
{
    return ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
        || (c == '_')
        || IsInAnnexE1(c);
}

// given hex digit character c, return its value
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

