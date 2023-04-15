/*

program         ::= statement+
statement       ::= PRINT expression
expression      ::= term (PLUS | MINUS) term)*
term            ::= factor ((MUL | DIV) factor)*
factor          ::= INTEGER | LPAR expression RPAR

*/

/*

PRINT -> "print"
PLUS -> "+"
MINUS -> "-"
MUL -> "*"
DIV -> "/"
LPAR -> "("
RPAR -> ")"
INTEGER -> [0-9]+

*/

/*

print 4 + 2

*/

#import <iostream>
#import <string>
#import <vector>

enum class TokenType {
    PRINT,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LPAR,
    RPAR,
    INTEGER,
};

struct Token {
    TokenType type;
    std::string lexeme;
};

#define log(x) std::cout << x << std::endl;

///
//
// Lexer
//
///

// convert string to lowercase
std::string lower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
    [](unsigned char c){ return std::tolower(c); });
    return str;
}

// convert string to uppercase
std::string upper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
    [](unsigned char c){ return std::toupper(c); });
    return str;
}

// tokenize the source code
std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t currentCharIndex = 0;
    while (currentCharIndex < source.size()) {
        char ch = source[currentCharIndex];
        switch (ch) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                currentCharIndex++;
                break;
            case '+':
                tokens.push_back({TokenType::PLUS, "+"});
                currentCharIndex++;
                break;
            case '-':
                tokens.push_back({TokenType::MINUS, "-"});
                currentCharIndex++;
                break;
            case '*':
                tokens.push_back({TokenType::MUL, "*"});
                currentCharIndex++;
                break;
            case '/':
                tokens.push_back({TokenType::DIV, "/"});
                currentCharIndex++;
                break;
            case '(':
                tokens.push_back({TokenType::LPAR, "("});
                currentCharIndex++;
                break;
            case ')':
                tokens.push_back({TokenType::RPAR, ")"});
                currentCharIndex++;
                break;
            default:
                if (isdigit(ch)) {
                    size_t start = currentCharIndex;
                    while (isdigit(source[currentCharIndex]) && currentCharIndex < source.size()) {
                        currentCharIndex++;
                    }
                    std::string lexeme = source.substr(start, currentCharIndex - start);
                    tokens.push_back({TokenType::INTEGER, lexeme});
                } else if (isalpha(ch)) {
                    size_t start = currentCharIndex;
                    while (isalnum(source[currentCharIndex]) && currentCharIndex < source.size()) {
                        currentCharIndex++;
                    }
                    std::string lexeme = source.substr(start, currentCharIndex - start);
                    if (lexeme == "print" || lexeme == "PRINT") {
                        tokens.push_back({TokenType::PRINT, upper(lexeme)});
                    } else {
                        throw std::runtime_error("Unexpected identifier");
                    }
                }
        }
    }

    return tokens;
}

///
//
// Parser
//
///

struct AST {
    Token token;
    std::vector<AST> children;
};

AST parse_expression(const std::vector<Token>& tokens, size_t& currentTokenIndex);
AST parse_statement(const std::vector<Token>& tokens, size_t& currentTokenIndex);
AST parse_term(const std::vector<Token>& tokens, size_t& currentTokenIndex);
AST parse_factor(const std::vector<Token>& tokens, size_t& currentTokenIndex);

// parse : program ::= statement+
AST parse(const std::vector<Token>& tokens) {
    AST tree;
    size_t currentTokenIndex = 0;
    while (currentTokenIndex < tokens.size()) {
        AST statement = parse_statement(tokens, currentTokenIndex);
        tree.children.push_back(statement);
    }
    return tree;
}

// parse : statement ::= PRINT expression
AST parse_statement(const std::vector<Token>& tokens, size_t& currentTokenIndex) {
    AST statement;
    statement.token = tokens[currentTokenIndex];
    currentTokenIndex++;
    statement.children.push_back(parse_expression(tokens, currentTokenIndex));
    return statement;
}

// parse : expression ::= term (PLUS | MINUS) term)*
AST parse_expression(const std::vector<Token>& tokens, size_t& currentTokenIndex) {
    AST expression;
    expression.token = tokens[currentTokenIndex];
    // currentTokenIndex++;
    AST term = parse_term(tokens, currentTokenIndex);
    expression.children.push_back(term);
    while (currentTokenIndex < tokens.size()) {
        Token token = tokens[currentTokenIndex];
        if (token.type == TokenType::PLUS || token.type == TokenType::MINUS) {
            AST op;
            op.token = token;
            expression.children.push_back(op);
            currentTokenIndex++;
            term = parse_term(tokens, currentTokenIndex);
            expression.children.push_back(term);
        } else {
            break;
        }
    }
    return expression;
}

// parse : term ::= factor ((MUL | DIV) factor)*
AST parse_term(const std::vector<Token>& tokens, size_t& currentTokenIndex) {
    AST term;
    AST factor = parse_factor(tokens, currentTokenIndex);
    term.children.push_back(factor);
    while (currentTokenIndex < tokens.size()) {
        Token token = tokens[currentTokenIndex];
        if (token.type == TokenType::MUL || token.type == TokenType::DIV) {
            AST op;
            op.token = token;
            term.children.push_back(op);
            currentTokenIndex++;
            factor = parse_factor(tokens, currentTokenIndex);
            term.children.push_back(factor);
        } else {
            break;
        }
    }
    return term;
}

// parse : factor ::= INTEGER | LPAR expression RPAR
AST parse_factor(const std::vector<Token>& tokens, size_t& currentTokenIndex) {
    Token token = tokens[currentTokenIndex];
    AST expression;
    switch (token.type) {
        case TokenType::INTEGER:
            currentTokenIndex++;
            return AST{token};
        case TokenType::LPAR:
            currentTokenIndex++;
            expression = parse_expression(tokens, currentTokenIndex);
            if (tokens[currentTokenIndex].type != TokenType::RPAR) {
                throw std::runtime_error("Expected ')'");
            }
            currentTokenIndex++;
            return expression;
        default:
            throw std::runtime_error("Unexpected token");
    }
}

///
//
// Interpreter
//
///

void interpret(AST tree) {
    // TODO
}


///
//
// Main
//
///

int main() {
    std::string source = "print 4 + 2";
    std::vector<Token> tokens = tokenize(source);
    // for (Token token : tokens) {
    //  std::cout << token.lexeme << std::endl;
    // }
    AST statements = parse(tokens);
    interpret(statements);
}

