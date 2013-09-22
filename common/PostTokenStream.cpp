#include "PostTokenStream.hpp"

#include <cwchar>
#include <climits>
#include <cerrno>

PostTokenStream::PostTokenStream(DebugPostTokenOutputStream &output)
    : output_(output)
    , codePoints_(NULL)
    , codePointsAllocated_(NULL)
    , codePointsCount_(0)
{
    codePointsAllocated_ = 512;
    codePoints_ = new int[codePointsAllocated_];
}

PostTokenStream::~PostTokenStream()
{
    delete [] codePoints_;
}

void PostTokenStream::DecodeInput_(const string &data)
{
    int i = 0;
    codePointsCount_ = 0;
    int* utf8Data = new int [data.size()];

    if (data.size() >= codePointsAllocated_)
    {
        while (codePointsAllocated_ <= data.size())
        {
            codePointsAllocated_ *= 2;
        }
        delete [] codePoints_;
        codePoints_ = new int[codePointsAllocated_];
    }

    while (i < data.size())
    {
        utf8Data[i] = static_cast<unsigned char>(data[i]);
        i++;
    }

    i = 0;
    while (i < data.size())
    {
        i += UTF8Decode(utf8Data + i, codePoints_[codePointsCount_]);
        codePointsCount_++;
    }

    delete [] utf8Data;
    codePoints_[codePointsCount_] = 0;
}

void PostTokenStream::PushStringRecord_(const PostTokenStream::StringLiteralRecord &record)
{
    stringLiterals_.push_back(record);
}

void PostTokenStream::emit_whitespace_sequence()
{
}

void PostTokenStream::emit_eof()
{
    FlushAdjacentStringLiterals();
    output_.emit_eof();
}

void PostTokenStream::emit_new_line()
{
}

void PostTokenStream::emit_header_name(const string &data)
{
    FlushAdjacentStringLiterals();
    output_.emit_invalid(data);
}

void PostTokenStream::emit_identifier(const string &data)
{
    FlushAdjacentStringLiterals();
    if (StringToTokenTypeMap.find(data) != StringToTokenTypeMap.end())
    {
        output_.emit_simple(data, StringToTokenTypeMap.find(data)->second);
    }
    else
    {
        output_.emit_identifier(data);
    }
}

void PostTokenStream::emit_pp_number(const string &data)
{
    FlushAdjacentStringLiterals();
    DecodeInput_(data);

    enum
    {
        START,
        FINISH,
        HEX_LITERAL,
        INTEGER_SUFFIX,
        UD_SUFFIX,
        FRACTIONAL_PART,
        EXPONENT,
        FLOAT_SUFFIX,
    };

    int state = START;
    unsigned long long intValue = 0;
    long double longDoubleValue = 0.0;
    double doubleValue = 0.0;
    float floatValue = 0.0;
    EFundamentalType type = FT_VOID;
    int* udSuffixBegin = 0;
    bool decimal = false;
    int* floatPartEnd = 0;
    int* prefixPartEnd = 0;
    int base = 0;

    int i = 0;
    while (state != FINISH)
    {
        int* where = codePoints_ + i;
        int c = codePoints_[i];

        switch (state)
        {
//------------------------------------------------------------------------------
        case START:
            if (MatchPrefixes("0x|0X", where) == 2)
            {
                state = HEX_LITERAL;
                i += 2;
            }
            else if (c == '.')
            {
                if (!IsDigit(where[1]))
                {
                    output_.emit_invalid(data);
                    return;
                }
                i++;
                state = FRACTIONAL_PART;
            }
            else if (IsDigit(c))
            {
                while (IsDigit(c))
                {
                    i++;
                    c = codePoints_[i];
                }
                if (c == '.')
                {
                    i++;
                    state = FRACTIONAL_PART;
                }
                else if (c == 'e' || c == 'E')
                {
                    i++;
                    state = EXPONENT;
                }
                else
                {
                    if (codePoints_[0] == '0')
                    {
                        base = 8;
                        i = 0;
                        while('0' <= codePoints_[i] && codePoints_[i] <= '7')
                        {
                            i++;
                        }
                        state = INTEGER_SUFFIX;
                    }
                    else
                    {
                        base = 10;
                        decimal = true;
                        i = 0;
                        while('0' <= codePoints_[i] && codePoints_[i] <= '9')
                        {
                            i++;
                        }
                        state = INTEGER_SUFFIX;
                    }
                }
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
            break;
//------------------------------------------------------------------------------
        case HEX_LITERAL:
        {
            base = 16;
            int j = i;
            while(HexadecimalCharachters.count(where[0]) == 1)
            {
                i++;
                where = codePoints_ + i;
            }
            if (i == j)
            {
                output_.emit_invalid(data);
                return;
            }
            state = INTEGER_SUFFIX;
        }
            break;
//------------------------------------------------------------------------------
        case INTEGER_SUFFIX:
            if (MatchPrefixes("ull|uLL|Ull|ULL|llu|llU|LLu|LLU", where) == 3)
            {
                type = FT_UNSIGNED_LONG_LONG_INT;
                i += 3;
                state = FINISH;
            }
            else if (MatchPrefixes("ul|uL|Ul|UL|lu|lU|Lu|LU", where) == 2)
            {
                type = FT_UNSIGNED_LONG_INT;
                i += 2;
                state = FINISH;
            }
            else if (MatchPrefixes("ll|LL", where) == 2)
            {
                type = FT_LONG_LONG_INT;
                i += 2;
                state = FINISH;
            }
            else if (MatchPrefixes("l|L", where) == 1)
            {
                type = FT_LONG_INT;
                i += 1;
                state = FINISH;
            }
            else if (MatchPrefixes("u|U", where) == 1)
            {
                type = FT_UNSIGNED_INT;
                i += 1;
                state = FINISH;
            }
            else
            {
                prefixPartEnd = codePoints_ + i - 1;
                type = FT_INT;
                state = UD_SUFFIX;
            }
            floatPartEnd = codePoints_ + i - 1;
            break;
//------------------------------------------------------------------------------
        case FRACTIONAL_PART:
            while(IsDigit(c))
            {
                i++;
                c = codePoints_[i];
            }

            if (c == 'e' || c == 'E')
            {
                i++;
                state = EXPONENT;
            }
            else
            {
                state = FLOAT_SUFFIX;
            }
            break;

//------------------------------------------------------------------------------
        case EXPONENT:
            if (c == '+' || c == '-')
            {
                i++;
            }
            else if (IsDigit(c))
            {
                while(IsDigit(c))
                {
                    i++;
                    c = codePoints_[i];
                }
                state = FLOAT_SUFFIX;
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
            break;

//------------------------------------------------------------------------------
        case UD_SUFFIX:
            if (c == 0)
            {
                state == FINISH;
            }
            else if (c != '_')
            {
                output_.emit_invalid(data);
                return;
            }
            udSuffixBegin = where;
            while (IsNonDigit(c) || IsDigit(c))
            {
                i++;
                c = codePoints_[i];
            }
            state = FINISH;
            break;
//------------------------------------------------------------------------------
        case FLOAT_SUFFIX:
            if (c == 'f' || c == 'F')
            {
                i++;
                type = FT_FLOAT;
                state = FINISH;
            }
            else if (c == 'l' || c == 'L')
            {
                i++;
                type = FT_LONG_DOUBLE;
                state = FINISH;
            }
            else
            {
                type = FT_DOUBLE;
                state = UD_SUFFIX;
            }
            floatPartEnd = codePoints_ + i - 1;
            break;

//------------------------------------------------------------------------------
        case FINISH:
            break;
        }
    }
    if (i != codePointsCount_)
    {
        output_.emit_invalid(data);
        return;
    }

    string floatString = "";
    if (floatPartEnd != 0)
    {
        floatString = UTF8CodePointToString(codePoints_, 0, floatPartEnd - codePoints_);
    }

    string udSuffix = "";
    if (udSuffixBegin != 0)
    {
        udSuffix = UTF8CodePointToString(codePoints_, udSuffixBegin - codePoints_, codePointsCount_ - 1);
    }

    string prefix = "";
    if (prefixPartEnd != 0)
    {
        prefix = UTF8CodePointToString(codePoints_, 0, prefixPartEnd - codePoints_);
    }

    if (!udSuffix.empty())
    {
        if (type == FT_INT)
        {
            output_.emit_user_defined_literal_integer(data, udSuffix, prefix);
        }
        else if (type == FT_DOUBLE)
        {
            output_.emit_user_defined_literal_floating(data, udSuffix, floatString);
        }
        return;
    }

    if (base != 0)
    {
        errno = 0;
        int* where = codePoints_;
        if (base == 16)
        {
            where += 2;
        }
        int* whereWas = where;
        intValue = wcstoull(reinterpret_cast<wchar_t*>(where),
                            reinterpret_cast<wchar_t**>(&where), base);
        if (errno == ERANGE || whereWas == where)
        {
            output_.emit_invalid(data);
            return;
        }
    }

    switch (type)
    {
    case FT_INT:
        if (decimal)
        {
            if (intValue <= INT_MAX)
            {
                int t = intValue;
                output_.emit_literal(data, FT_INT, &t, sizeof(t));
            }
            else if (intValue <= LONG_MAX)
            {
                long int t = intValue;
                output_.emit_literal(data, FT_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
        }
        else
        {
            if (intValue <= INT_MAX)
            {
                int t = intValue;
                output_.emit_literal(data, FT_INT, &t, sizeof(t));
            }
            else if (intValue <= UINT_MAX)
            {
                unsigned int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_INT, &t, sizeof(t));
            }
            else if (intValue <= LONG_MAX)
            {
                long int t = intValue;
                output_.emit_literal(data, FT_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= ULLONG_MAX)
            {
                unsigned long int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= ULLONG_MAX)
            {
                unsigned long long int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
        }
        break;
    case FT_UNSIGNED_INT:
    {
        if (intValue <= UINT_MAX)
        {
            unsigned int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_INT, &t, sizeof(t));
        }
        else if (intValue <= ULONG_MAX)
        {
            unsigned long int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_LONG_INT, &t, sizeof(t));
        }
        else if (intValue <= ULLONG_MAX)
        {
            unsigned long long int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
        }
        else
        {
            output_.emit_invalid(data);
            return;
        }
    }
        break;
    case FT_LONG_INT:
        if (decimal)
        {
            if (intValue <= LONG_MAX)
            {
                long int t = intValue;
                output_.emit_literal(data, FT_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
        }
        else
        {
            if (intValue <= LONG_MAX)
            {
                long int t = intValue;
                output_.emit_literal(data, FT_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= ULONG_MAX)
            {
                unsigned long int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= ULLONG_MAX)
            {
                unsigned long long int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
        }
        break;
    case FT_LONG_LONG_INT:
        if (decimal)
        {
            if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }
        }
        else
        {
            if (intValue <= LLONG_MAX)
            {
                long long int t = intValue;
                output_.emit_literal(data, FT_LONG_LONG_INT, &t, sizeof(t));
            }
            else if (intValue <= ULLONG_MAX)
            {
                unsigned long long int t = intValue;
                output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
            }
            else
            {
                output_.emit_invalid(data);
                return;
            }

        }
        break;
    case FT_UNSIGNED_LONG_INT:
    {
        if (intValue <= ULONG_MAX)
        {
            unsigned long int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_LONG_INT, &t, sizeof(t));
        }
        else if (intValue <= ULLONG_MAX)
        {
            unsigned long long int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
        }
        else
        {
            output_.emit_invalid(data);
            return;
        }
    }
        break;
    case FT_UNSIGNED_LONG_LONG_INT:
    {
        if (intValue <= ULLONG_MAX)
        {
            unsigned long long int t = intValue;
            output_.emit_literal(data, FT_UNSIGNED_LONG_LONG_INT, &t, sizeof(t));
        }
        else
        {
            output_.emit_invalid(data);
            return;
        }
    }
        break;
    case FT_FLOAT:
    {
        floatValue = PA2Decode_float(floatString);
        output_.emit_literal(data, type, &floatValue, sizeof(floatValue));
    }
        break;
    case FT_DOUBLE:
    {
        doubleValue = PA2Decode_double(floatString);
        output_.emit_literal(data, type, &doubleValue, sizeof(doubleValue));
    }
        break;
    case FT_LONG_DOUBLE:
    {
        longDoubleValue = PA2Decode_long_double(floatString);
        output_.emit_literal(data, type, &longDoubleValue, sizeof(longDoubleValue));
    }
        break;
    default:
    {
        output_.emit_invalid(data);
        return;
    }
        break;
    }
}

void PostTokenStream::emit_character_literal(const string &data)
{
    FlushAdjacentStringLiterals();
    DecodeInput_(data);
    codePointsCount_ = ReplaceEscapeSequences(codePoints_);

    if (codePointsCount_ > 3 && codePoints_[0] == '\'' || codePointsCount_ > 4)
    {
        output_.emit_invalid(data);
        cerr << "ERROR: multi code point character literals not supported: " << data << endl;
    }
    else if (codePointsCount_ <= 2 || codePointsCount_ == 3 && codePoints_[0] != '\'')
    {
        output_.emit_invalid(data);
        cerr << "ERROR: Empty character literal." << data << endl;
    }
    else
    {
        if (codePoints_[0] == '\'')
        {
            if (codePoints_[1] < 0x80)
            {
                char t = codePoints_[1];
                if (characterLiteralUdSuffix_.empty())
                {
                    output_.emit_literal(data, FT_CHAR, &t, 1);
                }
                else
                {
                    output_.emit_user_defined_literal_character(characterLiteralData_,
                                                                characterLiteralUdSuffix_,
                                                                FT_CHAR, &t, 1);
                }
            }
            else if (codePoints_[1] <= 0x10FFFF)
            {
                if (characterLiteralUdSuffix_.empty())
                {
                    output_.emit_literal(data, FT_INT, codePoints_ + 1, 4);
                }
                else
                {
                    output_.emit_user_defined_literal_character(characterLiteralData_,
                                                                characterLiteralUdSuffix_,
                                                                FT_INT, codePoints_ + 1, 4);
                }
            }
            else
            {
                output_.emit_invalid(data);
            }
        }
        else if (codePoints_[0] == 'L')
        {
            wchar_t t = data.c_str()[2];
            if (characterLiteralUdSuffix_.empty())
            {
                output_.emit_literal(data, FT_WCHAR_T, &codePoints_[2], 4);
            }
            else
            {
                output_.emit_user_defined_literal_character(characterLiteralData_,
                                                            characterLiteralUdSuffix_,
                                                            FT_WCHAR_T, &codePoints_[2], 4);
            }
        }
        else if (codePoints_[0] == 'u')
        {
            char16_t* utf16Data = new char16_t[codePointsCount_ * 2 + 1];
            int i = 0;
            int j = 0;
            while (i < codePointsCount_)
            {
                j += UTF16Encode(codePoints_[i], utf16Data + j);
                i++;
            }
            if (j == 4)
            {
                if (characterLiteralUdSuffix_.empty())
                {
                    output_.emit_literal(data, FT_CHAR16_T, &utf16Data[2], 2);
                }
                else
                {
                    output_.emit_user_defined_literal_character(characterLiteralData_,
                                                                characterLiteralUdSuffix_,
                                                                FT_CHAR16_T, &utf16Data[2], 2);
                }
            }
            else
            {
                output_.emit_invalid(data);
            }
            delete [] utf16Data;
        }
        else if (codePoints_[0] == 'U')
        {
            if (characterLiteralUdSuffix_.empty())
            {
                output_.emit_literal(data, FT_CHAR32_T, &codePoints_[2], 4);
            }
            else
            {
                output_.emit_user_defined_literal_character(characterLiteralData_,
                                                            characterLiteralUdSuffix_,
                                                            FT_CHAR32_T, &codePoints_[2], 4);
            }
        }
    }
}

void PostTokenStream::emit_user_defined_character_literal(const string &data)
{
    FlushAdjacentStringLiterals();
    DecodeInput_(data);
    int i = data.size() - 1;
    while (data[i] != '\'')
    {
        i--;
    }
    if (data[i + 1] != '_')
    {
        output_.emit_invalid(data);
    }
    else
    {
        characterLiteralUdSuffix_ = data.substr(i + 1, data.size() - i);
        characterLiteralData_ = data;
        emit_character_literal(data.substr(0, i + 1));
        characterLiteralUdSuffix_ = "";
        characterLiteralData_ = "";
    }
}

void PostTokenStream::emit_string_literal(const string &data)
{
    DecodeInput_(data);
    StringLiteralRecord r;
    r.data = data;
    bool rawString = false;
    if (MatchPrefix("u8", codePoints_) == 2)
    {
        rawString = codePoints_[2] == 'R';
        r.typePrefix = "u8";
    }
    else if (MatchPrefix("u", codePoints_) == 1)
    {
        rawString = codePoints_[1] == 'R';
        r.typePrefix = "u";
    }
    else if (MatchPrefix("U", codePoints_) == 1)
    {
        rawString = codePoints_[1] == 'R';
        r.typePrefix = "U";
    }
    else if (MatchPrefix("L", codePoints_) == 1)
    {
        rawString = codePoints_[1] == 'R';
        r.typePrefix = "L";
    }
    else
    {
        rawString = codePoints_[0] == 'R';
        r.typePrefix = "";
    }

    if (!rawString)
    {
        codePointsCount_ = ReplaceEscapeSequences(codePoints_);
    }
    int* begin = codePoints_;
    int* end = codePoints_ + codePointsCount_ - 1;

    if (rawString)
    {
        while (begin[0] != '(')
        {
            begin++;
        }
        begin++;
        while (end[0] != ')')
        {
            end--;
        }
        end--;
    }
    else
    {
        while (begin[0] != '"')
        {
            begin++;
        }
        begin++;
        while (end[0] != '"')
        {
            end--;
        }
        end--;
    }

    r.codePoints.insert(r.codePoints.end(), begin, end + 1);
    PushStringRecord_(r);
}

void PostTokenStream::emit_user_defined_string_literal(const string &data)
{
    DecodeInput_(data);
    int i = data.size() - 1;
    while (data[i] != '"')
    {
        i--;
    }
    emit_string_literal(data.substr(0, i + 1));
    StringLiteralRecord& r = stringLiterals_.back();
    r.udSuffix = data.substr(i + 1, data.size() - i);
    r.data = data;
}

void PostTokenStream::emit_preprocessing_op_or_punc(const string &data)
{
    FlushAdjacentStringLiterals();
    if (StringToTokenTypeMap.find(data) != StringToTokenTypeMap.end())
    {
        output_.emit_simple(data, StringToTokenTypeMap.find(data)->second);
    }
    else
    {
        output_.emit_invalid(data);
    }
}

void PostTokenStream::emit_non_whitespace_char(const string &data)
{
    FlushAdjacentStringLiterals();
    output_.emit_invalid(data);
}

void PostTokenStream::FlushAdjacentStringLiterals()
{
    if (stringLiterals_.empty())
    {
        return;
    }

    EFundamentalType commonType = FT_VOID;
    string commonTypePrefix = "";
    string commonUdSuffix = "";
    bool invalid = false;

    for (int i = 0; i < stringLiterals_.size(); i++)
    {
        StringLiteralRecord& l = stringLiterals_[i];
        if (!l.typePrefix.empty())
        {
            if (!commonTypePrefix.empty() && commonTypePrefix != l.typePrefix)
            {
                invalid = true;
                break;
            }
            commonTypePrefix = l.typePrefix;
        }

        if (!l.udSuffix.empty())
        {
            if (!commonUdSuffix.empty() && commonUdSuffix != l.udSuffix)
            {
                invalid = true;
                break;
            }
            if (l.udSuffix[0] != '_')
            {
                invalid = true;
                break;
            }
            commonUdSuffix = l.udSuffix;
        }
    }

    string commonData = "";
    for (int i = 0; i < stringLiterals_.size(); i++)
    {
        commonData += stringLiterals_[i].data;
        if (i != stringLiterals_.size() - 1)
        {
            commonData += ' ';
        }
    }

    if (commonTypePrefix.empty())
    {
        commonType = FT_CHAR;
    }
    else if (commonTypePrefix == "u8")
    {
        commonType = FT_CHAR;
    }
    else if (commonTypePrefix == "u")
    {
        commonType = FT_CHAR16_T;
    }
    else if (commonTypePrefix == "U")
    {
        commonType = FT_CHAR32_T;
    }
    else if (commonTypePrefix == "L")
    {
        commonType = FT_WCHAR_T;
    }

    if (invalid)
    {
        output_.emit_invalid(commonData);
    }
    else
    {
        vector<int> codePoints;
        for (int i = 0; i < stringLiterals_.size(); i++)
        {
            StringLiteralRecord& l = stringLiterals_[i];
            codePoints.insert(codePoints.end(), l.codePoints.begin(), l.codePoints.end());
        }

        void* commonDataPtr = NULL;
        int j = 0;
        int typeSize = 0;

        switch (commonType)
        {
        case FT_CHAR:
        {
            unsigned char* data = new unsigned char [codePoints.size() * 4 + 1];
            int i = 0;
            while (i < codePoints.size())
            {
                j += UTF8Encode(codePoints[i], data + j);
                i++;
            }
            data[j] = 0;
            j++;
            commonDataPtr = data;
            typeSize = 1;
        }
            break;

        case FT_CHAR16_T:
        {
            char16_t* data = new char16_t [codePoints.size() * 2 + 1];
            int i = 0;
            while (i < codePoints.size())
            {
                j += UTF16Encode(codePoints[i], data + j);
                i++;
            }
            data[j] = 0;
            j++;
            commonDataPtr = data;
            typeSize = 2;
        }
            break;

        case FT_CHAR32_T: case FT_WCHAR_T:
        {
            codePoints.push_back(0);
            commonDataPtr = codePoints.data();
            j = codePoints.size();
            typeSize = 4;
        }
            break;
        }

        if (!commonUdSuffix.empty())
        {
            output_.emit_user_defined_literal_string_array(commonData,
                                                           commonUdSuffix,
                                                           j,
                                                           commonType,
                                                           commonDataPtr,
                                                           j * typeSize);
        }
        else
        {
            output_.emit_literal_array(commonData, j, commonType, commonDataPtr, j * typeSize);
        }

        if (commonType == FT_CHAR || commonType == FT_CHAR16_T)
        {
            delete [] commonDataPtr;
        }
    }

    stringLiterals_.clear();
}
