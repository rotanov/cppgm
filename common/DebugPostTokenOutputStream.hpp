#pragma once

#include <iostream>

#include "Constants.hpp"
#include "Utils.hpp"

// DebugPostTokenOutputStream: helper class to produce PA2 output format
struct DebugPostTokenOutputStream
{
    // output: invalid <source>
    void emit_invalid(const string& source)
    {
        cout << "invalid " << source << endl;
    }

    // output: simple <source> <token_type>
    void emit_simple(const string& source, ETokenType token_type)
    {
        cout << "simple " << source << " " << TokenTypeToStringMap.at(token_type) << endl;
    }

    // output: identifier <source>
    void emit_identifier(const string& source)
    {
        cout << "identifier " << source << endl;
    }

    // output: literal <source> <type> <hexdump(data,nbytes)>
    void emit_literal(const string& source, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "literal " << source << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: literal <source> array of <num_elements> <type> <hexdump(data,nbytes)>
    void emit_literal_array(const string& source, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "literal " << source << " array of " << num_elements << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> character <type> <hexdump(data,nbytes)>
    void emit_user_defined_literal_character(const string& source, const string& ud_suffix, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " character " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> string array of <num_elements> <type> <hexdump(data, nbytes)>
    void emit_user_defined_literal_string_array(const string& source, const string& ud_suffix, size_t num_elements, EFundamentalType type, const void* data, size_t nbytes)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " string array of " << num_elements << " " << FundamentalTypeToStringMap.at(type) << " " << HexDump(data, nbytes) << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> <prefix>
    void emit_user_defined_literal_integer(const string& source, const string& ud_suffix, const string& prefix)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " integer " << prefix << endl;
    }

    // output: user-defined-literal <source> <ud_suffix> <prefix>
    void emit_user_defined_literal_floating(const string& source, const string& ud_suffix, const string& prefix)
    {
        cout << "user-defined-literal " << source << " " << ud_suffix << " floating " << prefix << endl;
    }

    // output : eof
    void emit_eof()
    {
        cout << "eof" << endl;
    }
};
