#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#include "IPPTokenStream.hpp"
#include "DebugPPTokenStream.hpp"

#include "Tokenizer.hpp"

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        ostringstream oss;
        oss << cin.rdbuf();

        string input = oss.str();

        DebugPPTokenStream output;

        int* transformedInput = new int[input.size() + 1];
        for (size_t i = 0; i < input.size(); i++)
        {
            transformedInput[i] = static_cast<unsigned char>(input[i]);
        }
        transformedInput[input.size()] = EndOfFile;

        PPTokenizer tokenizer(transformedInput, output);
        tokenizer.process();
        delete [] transformedInput;
    }
    catch (exception& e)
    {
        cerr << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

