#pragma once

#include "IPPTokenStream.hpp"
#include "DebugPostTokenOutputStream.hpp"

class PostTokenStream : public IPPTokenStream
{
public:
    PostTokenStream(DebugPostTokenOutputStream& output);

    virtual void emit_whitespace_sequence();
    virtual void emit_new_line();
    virtual void emit_header_name(const string& data);
    virtual void emit_identifier(const string& data);
    virtual void emit_pp_number(const string& data);
    virtual void emit_character_literal(const string& data);
    virtual void emit_user_defined_character_literal(const string& data);
    virtual void emit_string_literal(const string& data);
    virtual void emit_user_defined_string_literal(const string& data);
    virtual void emit_preprocessing_op_or_punc(const string& data);
    virtual void emit_non_whitespace_char(const string& data);
    virtual void emit_eof();

    virtual ~PostTokenStream();

 private:
    struct StringLiteralRecord
    {
        string data;
        string typePrefix;
        vector<int> codePoints;
        string udSuffix;
    };

    DebugPostTokenOutputStream& output_;
    int* codePoints_;
    int codePointsCount_;
    int codePointsAllocated_;
    vector<StringLiteralRecord> stringLiterals_;
    string characterLiteralUdSuffix_;
    string characterLiteralData_;

    void DecodeInput_(const string& data);
    void PushStringRecord_(const StringLiteralRecord& record);
    void FlushAdjacentStringLiterals();
};
