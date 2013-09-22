// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <memory>
#include <cstring>
#include <cstdint>
#include <climits>
#include <map>

using namespace std;

#include "Constants.hpp"
#include "Utils.hpp"
#include "DebugPostTokenOutputStream.hpp"
#include "PostTokenStream.hpp"
#include "Tokenizer.hpp"

int main()
{
	// TODO:
	// 1. apply your code from PA1 to produce `preprocessing-tokens`
	// 2. "post-tokenize" the `preprocessing-tokens` as described in PA2
	// 3. write them out in the PA2 output format specifed

	// You may optionally use the above starter code.
	//
	// In particular there is the DebugPostTokenOutputStream class which helps form the
	// correct output format:

    try
    {
        ostringstream oss;
        oss << cin.rdbuf();

        string input = oss.str();

        int* transformedInput = new int[input.size() + 1];
        for (size_t i = 0; i < input.size(); i++)
        {
            transformedInput[i] = static_cast<unsigned char>(input[i]);
        }
        transformedInput[input.size()] = EndOfFile;

        DebugPostTokenOutputStream output;
        PostTokenStream postTokenizer(output);

        PPTokenizer tokenizer(transformedInput, postTokenizer);
        tokenizer.process();
        delete [] transformedInput;
    }
    catch (PostTokenError& e)
    {
        cerr << "ERROR: " << e.what() << endl;
    }
    catch (exception& e)
    {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }

//	output.emit_invalid("foo");
//	output.emit_simple("auto", KW_AUTO);

//	u16string bar = u"bar";
//	output.emit_literal_array("u\"bar\"", bar.size()+1, FT_CHAR16_T, bar.data(), bar.size() * 2 + 2);

//	output.emit_user_defined_literal_integer("123_ud1", "ud1", "123");
}
