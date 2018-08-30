
#include "thirdparty/cpp-peglib/peglib.h"
#include "thirdparty/catch2/catch.hpp"
#include <assert.h>
#include <iostream>

using namespace peg;
using namespace std;

int main(void) {
    // (2) Make a parser
    auto grammar = R"(
        # Grammar for Calculator...
        Additive    <- Multitive '+' Additive / Multitive
        Multitive   <- Primary '*' Multitive / Primary
        Primary     <- '(' Additive ')' / Number
        Number      <- < [0-9]+ >
        %whitespace <- [ \t]*
    )";

    parser parser;

    parser.log = [](size_t line, size_t col, const string& msg) {
        cerr << line << ":" << col << ": " << msg << "\n";
    };

    auto ok = parser.load_grammar(grammar);
    assert(ok);

    // (3) Setup actions
    parser["Additive"] = [](const SemanticValues& sv) {
        switch (sv.choice()) {
        case 0:  // "Multitive '+' Additive"
            return sv[0].get<int>() + sv[1].get<int>();
        default: // "Multitive"
            return sv[0].get<int>();
        }
    };

    parser["Multitive"] = [](const SemanticValues& sv) {
        switch (sv.choice()) {
        case 0:  // "Primary '*' Multitive"
            return sv[0].get<int>() * sv[1].get<int>();
        default: // "Primary"
            return sv[0].get<int>();
        }
    };

    parser["Number"] = [](const SemanticValues& sv) {
        return stoi(sv.token(), nullptr, 10);
    };

    // (4) Parse
    parser.enable_packrat_parsing(); // Enable packrat parsing.

    int val;
    parser.parse(" (1 + 2) * 3 ", val);

    assert(val == 9);
	printf("%i", val);
}