/*
Copyright (c) 2018 K. S. Ernest (iFire) Lee

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. 
*/

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "thirdparty/catch2/catch.hpp"
#include "thirdparty/cpp-peglib/peglib.h"
#include <assert.h>
#include <iostream>

using namespace peg;
using namespace std;

SCENARIO("vectors can be sized and resized", "[vector]") {

	GIVEN("An ink grammar") {
		// clang-format off
		auto grammar = R"(
# ? (option, aka zero-or-one)
# lpeg ^-1
# * (aka Kleene star, zero-or-more) 
# lpeg ^0
# + (one-or-more)
# lpeg ^1
# a b (with spaces between them) is a 'sequence' expression
# lpeg *
# a / b is an 'ordered choice' expression
# lpeg +
# https://github.com/PhilippeSigaud/Pegged/wiki/PEG-Basics
# Ported from https://github.com/premek/pink/blob/master/pink/parser.lua

LINES <- TAG_GLOBAL* LINE*
LINE <- STMT / GATHER / PARA
STMT <- GLUE / DIVERT / KNOT / STITCH / OPTION / COMM

PARA <- TAG_ABOVE / TEXT TAG_END
TEXT <- (!TAG_END .)* _

TAG_END <- TAG
TAG_ABOVE <- TAG
TAG_GLOBAL <- TAG

HASH <- '#' _
TAG <- ~HASH TAG_CONTENT _
TAG_CONTENT <- (![\r\n] .)*

OPTION <- OPT_STARS OPT_ANS _
OPT_DIV_CONT <- (!']' .)*
OPT_ANS_WITH_DIV <- _ '[' _ OPT_DIV_CONT _ ']' / _ OPT_ANS_CONTENT _
OPT_ANS_WITHOUT_DIV <- _
OPT_ANS <- OPT_ANS_WITH_DIV / OPT_ANS_WITHOUT_DIV
OPT_ANS_CONTENT <- (![\r\n] .)*

OPT_STARS <- OPT_STAR OPT_STAR* OPT_ANS OPT_CONT _
~OPT_STAR <- _ '*'
OPT_CONT <- (![\r\n] .)*

GATHER <- GATHER_MARKS
GATHER_MARKS <- GATHER_MARK _ GATHER_MARK* _ GATHER_CONTENT
~GATHER_MARK <- _ '-'
GATHER_CONTENT <- (![\r\n] .)*

GLUE <- _ '<>' _

DIVERT <- DIVERT_END / DIVERT_JUMP

DIVERT_SYM <- '->' _
DIVERT_END <- DIVERT_SYM 'END' _
DIVERT_JUMP <- DIVERT_SYM _ ADDR _

KNOT <- _ ('=' '='+ ) _ ID _ ('=')* _
STITCH <- '=' _ ID _ ('=')* _

COMM <- COMM_OL / COMM_ML / TODO
TODO <- _ 'TODO:' ' '* TODO_CONTENT
TODO_CONTENT <- (![\r\n] .)*
COMM_OL <- _ '//' ' '* COMM_OL_CONTENT
COMM_OL_CONTENT <- (![\r\n] .)*
COMM_ML <- _ '/*' [ \r\n]* COMM_ML_CONTENT
COMM_ML_CONTENT <- (!'*/' .)*
ADDR <- ID ('.' ID)?
ID <- ([a-zA-Z]+ / '_') ([a-zA-Z0-9] / '_')*
~NL <- [\r\n]*
~_ <- [ \t\r\n]*
EOF <- !.
)";
					 
		parser parser;
		// clang-format on
		std::stringstream out;
		parser.log = [&out](size_t line, size_t col, const string &msg) {
			out << line << ":" << col << ": " << msg << "\n";
		};

		auto ok = parser.load_grammar(grammar);
		assert(ok);

		parser = parser.enable_ast();

		WHEN("given an ink file") {
			shared_ptr<peg::Ast> ast;
			if (parser.parse(R"(TODO: This is a todo.)", ast)) {
				ast = peg::AstOptimizer(true).optimize(ast);
				out << peg::ast_to_s(ast);
			}
			THEN("return ast") {
				REQUIRE(ast->name == "TODO_CONTENT");
				REQUIRE(ast->token == "This is a todo.");
			}
		}

		//// (3) Setup actions
		//parser["Additive"] = [](const SemanticValues& sv) {
		//    switch (sv.choice()) {
		//    case 0:  // "Multitive '+' Additive"
		//        return sv[0].get<int>() + sv[1].get<int>();
		//    default: // "Multitive"
		//        return sv[0].get<int>();
		//    }
		//};

		//parser["Multitive"] = [](const SemanticValues& sv) {
		//    switch (sv.choice()) {
		//    case 0:  // "Primary '*' Multitive"
		//        return sv[0].get<int>() * sv[1].get<int>();
		//    default: // "Primary"
		//        return sv[0].get<int>();
		//    }
		//};

		//parser["Number"] = [](const SemanticValues& sv) {
		//    return stoi(sv.token(), nullptr, 10);
		//};

		// (4) Parse
		parser.enable_packrat_parsing(); // Enable packrat parsing.

		//int val;
		//parser.parse(" (1 + 2) * 3 ", val);

		//assert(val == 9);
	}
}
