#pragma once

// Tokenizer
class PPTokenizer
{
public:
	IPPTokenStream& output;

    PPTokenizer(int* source, IPPTokenStream& output)
		: output(output)
        , source_(source)
        , pos(0)
        , state(NORMAL)
        , inputSize_(0)
        , transformed_(NULL)
        , translations_(NULL)
	{}

    enum State
    {
        NORMAL,
        COMMENT,
        INLINE_COMMENT,
        WHITESPACE,
        MATCHING_IDENTIFIER,
        FINISHED,
        MATCHING_CHARACTER_LITERAL,
        MATCHING_PP_NUMBER,
        MATCHING_S_CHAR_SEQUENCE,
        MATCHING_RAW_STRING_D_SEQUENCE,
        MATCHING_RAW_STRING_R_SEQUENCE,
        MATCHING_UD_STRING_LITERAL,
        MATCHING_UD_CHARACTER_LITERAL,
    };

    State state;
    int inputSize_;
    int* transformed_;
    int* translations_;

    void Decode()
    {
        int codePoint = 0;
        int* i = source_;
        int* j = source_;
        while (i[0] != EndOfFile)
        {
            i += UTF8Decode(i, codePoint);
            j[0] = codePoint;
            j++;
        }
        j[0] = EndOfFile;
        inputSize_ = j - source_ + 1;
        transformed_ = new int[inputSize_];
        translations_ = new int[inputSize_];
        for (int k = 0; k < inputSize_; k++)
        {
            translations_[k] = -1;
        }
    }

    void ProcessTrigraphs()
    {
        int* i = source_;
        int* j = transformed_;
        while (i[0] != EndOfFile)
        {
            translations_[j - transformed_] = i - source_;
            if (MatchPrefix("??", i) == 2)
            {
                if (TrigraphReplacement.find(i[2]) != TrigraphReplacement.end())
                {

                    j[0] = TrigraphReplacement.find(i[2])->second;
                    i += 3;
                    j++;
                    continue;
                }
            }
            j[0] = i[0];
            i++;
            j++;
        }

        translations_[j - transformed_] = i - source_;
        j[0] = EndOfFile;
    }

    void ProcessUCN()
    {
        int* i = transformed_;
        int* j = transformed_;
        while (i[0] != EndOfFile)
        {
            translations_[j - transformed_] += i - j;
//------------------------------------------------------------------------------
            if (MatchPrefix("\\u", i) == 2)
            {
                int quadLow;
                if (MatchHexQuad(i + 2, quadLow) != 0)
                {
                    j[0] = quadLow;
                    i += 6;
                    j++;
                    continue;
                }
            }
//------------------------------------------------------------------------------
            else if (MatchPrefix("\\U", i) == 2)
            {
                int quadHigh;
                int quadLow;
                if (MatchHexQuad(i + 2, quadHigh) != 0 &&
                    MatchHexQuad(i + 6, quadLow) != 0)
                {
                    quadHigh = quadHigh << 16;
                    quadLow |= quadHigh;

                    j[0] = quadLow;
                    i += 10;
                    j++;
                    continue;
                }
            }
            j[0] = i[0];
            i++;
            j++;
        }

        translations_[j - transformed_] += i - j;
        j[0] = EndOfFile;
    }

    void ProcessSplicing()
    {
        int* i = transformed_;
        int* j = transformed_;
        while (i[0] != EndOfFile)
        {
            if (MatchPrefix("\\\n", i) == 2)
            {
                i += 2;
                continue;
            }
            translations_[j - transformed_ - (i - j)] = translations_[j - transformed_];
            j[0] = i[0];
            i++;
            j++;
        }

        translations_[j - transformed_] += i - j;
        j[0] = EndOfFile;
    }

    void process()
	{
		// TODO:  Your code goes here.

		// 1. do translation features
		// 2. tokenize resulting stream
		// 3. call an output.emit_* function for each token.

		// TIP: Reference implementation is about 1000 lines of code.
		// It is a state machine with about 50 states, most of which
		// are simple transitions of the operators.

        Decode();
        ProcessTrigraphs();
        ProcessUCN();
        ProcessSplicing();

        headerNameState = 0;

        while (state != FINISHED)
        {
            int* where = transformed_ + pos;
            int c = where[0];

            if (state == MATCHING_RAW_STRING_D_SEQUENCE
             || state == MATCHING_RAW_STRING_R_SEQUENCE)
            {
                where = source_ + sourcePos;
                c = where[0];
            }

            //------------------------------------------------------------------------------
//            if (state != INSIDE_RAW_STRING_LITERAL)
//            {
//                if (MatchPrefix("\\\n", where) == 2)
//                {
//                    pos += 2;
//                    continue;
//                }
//            }

            switch (state)
            {
////////////////////////////////////////////////////////////////////////////////
            case NORMAL:
//------------------------------------------------------------------------------
                if (IsWhiteSpace(c))
                {
                    state = WHITESPACE;
                    pos++;
                }
//------------------------------------------------------------------------------
                else if (MatchPrefix("//", where) == 2)
                {
                    pos += 2;
                    state = COMMENT;
                }
//------------------------------------------------------------------------------
                else if (MatchPrefix("/*", where) == 2)
                {
                    state = INLINE_COMMENT;
                    pos += 2;
                }
//------------------------------------------------------------------------------
                else if (c == '\n')
                {
                    output.emit_new_line();
                    headerNameState = 0;
                    pos++;
                }
//------------------------------------------------------------------------------
                else if (c == '\'')
                {
                    outStart = pos;
                    pos++;
                    state = MATCHING_CHARACTER_LITERAL;
                }
//------------------------------------------------------------------------------
                else if (IsDigit(c))
                {
                    outStart = pos;
                    pos++;
                    state = MATCHING_PP_NUMBER;
                }
//------------------------------------------------------------------------------
                else if (c == '.' && IsDigit((where + 1)[0]))
                {
                    outStart = pos;
                    pos += 2;
                    state = MATCHING_PP_NUMBER;
                }
//------------------------------------------------------------------------------
                else if (c == '"')
                {
                    outStart = pos;
                    if (headerNameState == 2)
                    {
                        int j = pos + 1;
                        while (true)
                        {
                            if  (transformed_[j] == EndOfFile || transformed_[j] == '\n')
                            {
                                throw logic_error("Unterminated header name.");
                            }
                            else
                            {
                                if (transformed_[j] == '"')
                                {
                                    output.emit_header_name(UTF8CodePointToString(transformed_, pos, j));
                                    pos = j + 1;
                                    state = NORMAL;
                                    break;
                                }
                            }
                            j++;
                        }
                    }
                    else
                    {
                        pos++;
                        state = MATCHING_S_CHAR_SEQUENCE;
                    }
                }
//------------------------------------------------------------------------------
                else if (IsNonDigit(c) && !IsInAnnexE2(c))
                {
                    outStart = pos;

                    int matchedForCharacterLiteral = MatchPrefixes("'|u'|U'|L'", where);
                    int matchedForStringLiteral = MatchPrefixes("u8\"|u\"|U\"|L\"", where);
                    int matchedForRawStringLiteral = MatchPrefixes("u8R\"|uR\"|UR\"|LR\"|R\"", where);
                    if (matchedForCharacterLiteral > 0)
                    {
                        pos += matchedForCharacterLiteral;
                        state = MATCHING_CHARACTER_LITERAL;
                    }
                    else if (matchedForStringLiteral > 0)
                    {
                        pos += matchedForStringLiteral;
                        state = MATCHING_S_CHAR_SEQUENCE;
                    }
                    else if (matchedForRawStringLiteral > 0)
                    {
                        sourcePos = translations_[pos + matchedForRawStringLiteral - 1] + 1;
                        pos += matchedForRawStringLiteral;
                        outEnd = pos - 1;
                        dPrefixBegin = sourcePos;
                        state = MATCHING_RAW_STRING_D_SEQUENCE;
                    }
                    else
                    {
                        pos++;
                        state = MATCHING_IDENTIFIER;
                    }
                }
//------------------------------------------------------------------------------
                else if (c == EndOfFile)
                {
                    state = FINISHED;
                }
//------------------------------------------------------------------------------
                else
                {
                    int spaceTillNotEof = 1;
                    while (transformed_[pos + spaceTillNotEof] != EndOfFile && spaceTillNotEof < 4)
                    {
                        spaceTillNotEof++;
                    }

                    bool opOrPuncFound = false;

                    for (int i = spaceTillNotEof; i > 0; i--)
                    {
                        string s = UTF8CodePointToString(transformed_, pos, pos + i - 1);
                        if (Punctuation[i - 1].count(s) > 0)
                        {
                            if (headerNameState == 0 && (s == "#" || s == "%:"))
                            {
                                headerNameState = 1;
                            }
                            if (headerNameState == 2)
                            {
                                if (s == "<")
                                {
                                    int j = pos + 1;
                                    while (true)
                                    {
                                        if  (transformed_[j] == EndOfFile || transformed_[j] == '\n')
                                        {
                                            throw logic_error("Unterminated header name.");
                                        }
                                        else
                                        {
                                            if (s == "<" && transformed_[j] == '>')
                                            {
                                                output.emit_header_name(UTF8CodePointToString(transformed_, pos, j));
                                                pos = j + 1;
                                                opOrPuncFound = true;
                                                state = NORMAL;
                                                break;
                                            }
                                        }
                                        j++;
                                    }
                                    if (opOrPuncFound)
                                    {
                                        break;
                                    }
                                }
                            }
                            if (s == "<::")
                            {
                                if (spaceTillNotEof == 3 || (spaceTillNotEof > 3 && where[3] != ':' && where[3] != '>'))
                                {
                                    output.emit_preprocessing_op_or_punc("<");
                                    pos += 1;
                                }
                                else
                                {
                                    output.emit_preprocessing_op_or_punc("<:");
                                    pos += 2;
                                }
                            }
                            else
                            {
                                output.emit_preprocessing_op_or_punc(s);
                                pos += i;
                            }
                            opOrPuncFound = true;
                            break;
                        }
                    }

                    if (opOrPuncFound)
                    {
                        break;
                    }

                    output.emit_non_whitespace_char(UTF8CodePointToString(c));
                    pos++;
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case COMMENT:
//------------------------------------------------------------------------------
                if (c == '\n')
                {
                    output.emit_whitespace_sequence();
                    output.emit_new_line();
                    headerNameState = 0;
                    pos++;
                    state = NORMAL;
                }
//------------------------------------------------------------------------------
                else if (c == EndOfFile)
                {
                    output.emit_whitespace_sequence();
                    state = FINISHED;
                }
//------------------------------------------------------------------------------
                else
                {
                    pos++;
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case INLINE_COMMENT:
//------------------------------------------------------------------------------
                if (MatchPrefix("*/", where) == 2)
                {
                    output.emit_whitespace_sequence();
                    pos += 2;
                    state = NORMAL;
                }
//------------------------------------------------------------------------------
                else if (c == EndOfFile)
                {
                    throw logic_error("Partial inline comment.");
                }
//------------------------------------------------------------------------------
                else
                {
                    pos++;
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case WHITESPACE:
//------------------------------------------------------------------------------
                if (c == ' ' || c == '\t')
                {
                    pos++;
                }
//------------------------------------------------------------------------------
                else if (MatchPrefix("/*", where) == 2 || MatchPrefix("//", where) == 2)
                {
                    state = NORMAL;
                }
//------------------------------------------------------------------------------
                else if (c == '\n')
                {
                    output.emit_whitespace_sequence();
                    output.emit_new_line();
                    headerNameState = 0;
                    pos++;
                    state = NORMAL;
                }
//------------------------------------------------------------------------------
                else
                {
                    output.emit_whitespace_sequence();
                    state = NORMAL;
                    if (c == EndOfFile)
                    {
                        state = FINISHED;
                    }
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_IDENTIFIER: case MATCHING_UD_STRING_LITERAL: case MATCHING_UD_CHARACTER_LITERAL:
//------------------------------------------------------------------------------
                if (IsDigit(c) || IsNonDigit(c))
                {
                    pos++;
                }
//------------------------------------------------------------------------------
                else
                {
                    string identifier = UTF8CodePointToString(transformed_, outStart, pos - 1);

                    if (state == MATCHING_UD_STRING_LITERAL)
                    {
                        output.emit_user_defined_string_literal(identifier);
                    }
                    else if (state == MATCHING_UD_CHARACTER_LITERAL)
                    {
                        output.emit_user_defined_character_literal(identifier);
                    }
                    else
                    {
                        if (Digraph_IdentifierLike_Operators.count(identifier) != 0)
                        {
                            output.emit_preprocessing_op_or_punc(identifier);
                        }
                        else
                        {
                            output.emit_identifier(identifier);
                            if (headerNameState == 1 && identifier == "include")
                            {
                                headerNameState = 2;
                            }
                            else
                            {
                                headerNameState = 3;
                            }
                        }
                    }
                    state = NORMAL;
                    if (c == EndOfFile)
                    {
                        state = FINISHED;
                    }
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_CHARACTER_LITERAL:
//------------------------------------------------------------------------------
                // ignoring escape sequences and stuff for now
                if (c == '\'')
                {
                    if (IsNonDigit(where[1]))
                    {
                        state = MATCHING_UD_CHARACTER_LITERAL;
                        pos += 2;
                    }
                    else
                    {
                        output.emit_character_literal(UTF8CodePointToString(transformed_, outStart, pos));
                        state = NORMAL;
                        pos++;
                    }
                }
//------------------------------------------------------------------------------
                else if (c == '\n')
                {
                    throw logic_error("Newling in character literal");
                }
//------------------------------------------------------------------------------
                else if (c == '\\')
                {
                    int escapeSequenceMatched = MatchEscapeSequence(where + 1);
                    if (escapeSequenceMatched == 0)
                    {
                        throw logic_error("Invalid escape sequence.");
                    }
                    else
                    {
                        pos += escapeSequenceMatched + 1;
                    }
                }
//------------------------------------------------------------------------------
                else
                {
                    if (c == EndOfFile)
                    {
                        state = FINISHED;
                    }
                    else
                    {
                        pos++;
                    }
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_PP_NUMBER:
            {
                int matched = MatchPrefixes("e-|e+|E-|E+", where);
//------------------------------------------------------------------------------
                if (IsDigit(c) || IsNonDigit(c) || matched > 0 || c == '.')
                {
                    matched = matched == 0 ? 1 : matched;
                    pos += matched;
                }
//------------------------------------------------------------------------------
                else
                {
                    output.emit_pp_number(UTF8CodePointToString(transformed_, outStart, pos - 1));
                    if (c == EndOfFile)
                    {
                        state = FINISHED;
                    }
                    else
                    {
                        state = NORMAL;
                    }
                }
            }
            break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_S_CHAR_SEQUENCE:
//------------------------------------------------------------------------------
                if (c == '"')
                {
                    if (IsNonDigit(where[1]))
                    {
                        state = MATCHING_UD_STRING_LITERAL;
                        pos += 2;
                    }
                    else
                    {
                        output.emit_string_literal(UTF8CodePointToString(transformed_, outStart, pos));
                        state = NORMAL;
                        pos++;
                    }
                }
                else if (c == '\n')
                {
                    throw logic_error("Newline in string literal");
                }
                else if (c == '\\')
                {
                    int escapeSequenceMatched = MatchEscapeSequence(where + 1);
                    if (escapeSequenceMatched == 0)
                    {
                        throw logic_error("Invalid escape sequence.");
                    }
                    else
                    {
                        pos += escapeSequenceMatched + 1;
                    }
                }
                else if (c == EndOfFile)
                {
                    throw logic_error("Unfinished string literal");
                }
                else
                {
                    pos++;
                }
                break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_RAW_STRING_D_SEQUENCE:
                if (IsDChar(c))
                {
                    sourcePos++;
                }
//------------------------------------------------------------------------------
                else if (c == '(')
                {
                    dPrefixEnd = sourcePos - 1;
                    sourcePos++;
                    state = MATCHING_RAW_STRING_R_SEQUENCE;
                }
//------------------------------------------------------------------------------
                else
                {
                    throw logic_error("Invalid raw string.");
                }
//------------------------------------------------------------------------------
            break;
////////////////////////////////////////////////////////////////////////////////
            case MATCHING_RAW_STRING_R_SEQUENCE:
//------------------------------------------------------------------------------
                if (c == ')')
                {
                    if (MatchSubsequence(source_, sourcePos + 1, dPrefixBegin, dPrefixEnd))
                    {
                        if (where[dPrefixEnd - dPrefixBegin + 2] == '\"')
                        {
                            // enconding prefix + R"
                            string str0 = UTF8CodePointToString(transformed_, outStart, outEnd);
                            string str1 = UTF8CodePointToString(source_, dPrefixBegin, sourcePos + dPrefixEnd - dPrefixBegin + 2);
                            output.emit_string_literal(str0 + str1);
                            while (translations_[pos] < sourcePos + dPrefixEnd - dPrefixBegin + 2)
                            {
                                pos++;
                            }
                            pos++;
                            state = NORMAL;
                        }
                        else
                        {
                            sourcePos += dPrefixEnd - dPrefixEnd + 1;
                        }
                    }
                    else
                    {
                        sourcePos++;
                    }
                }
                else if (c == EndOfFile)
                {
                    throw logic_error("Invalid raw string.");
                }
                else
                {
                    sourcePos++;
                }
                break;
//////////////////////////////////////////////////////////////////////////////////
//            case MATCHING_PP_NUMBER:
////------------------------------------------------------------------------------

//                break;
            }
        }

        // I am deeply ashamed by this and headerNameState kludge below.
        if (inputSize_ > 1)
        {
            if (inputSize_ > 2)
            {
                if (*(transformed_ + pos - 2) == '\\')
                {
                    if (*(transformed_ + pos - 1) == '\n')
                    {
                        output.emit_new_line();
                    }
                }
                if (*(transformed_ + pos - 1) != '\n')
                {
                    output.emit_new_line();
                }
            }
            else
            {
                if (*(transformed_ + pos - 1) != '\n')
                {
                    output.emit_new_line();
                }
            }
        }

        output.emit_eof();
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

    // splits string by |, if we want to match | then we're at fail
    // returns true if at least one prefix matched
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

    // syntax:
    // [ \t].*(#|%:)[ \t].*include[ \t].*
    // int MatchRegex

private:
    int pos;    
    int outStart;
    // raw string specials:
    int* source_;
    int sourcePos;
    int outEnd;
    int dPrefixBegin;
    int dPrefixEnd;
    // cs kludge
    int headerNameState;
};