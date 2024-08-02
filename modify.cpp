#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include <cctype>
#include <memory>

// Token types for new statements
enum TokenType {
    NUMBER,
    IDENTIFIER,
    ASSIGN,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    PRINT,
    INPUT,
    WHILE,
    IF,
    ELSE,
    LEFT_PAREN,
    RIGHT_PAREN,
    END,
    INVALID
};

struct Token {
    TokenType type;
    std::string value;
};

class Tokenizer {
public:
    Tokenizer(const std::string& source) : source(source), position(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (position < source.size()) {
            char current = source[position];
            if (isspace(current)) {
                position++;
                continue;
            }
            if (isdigit(current)) {
                tokens.push_back(tokenizeNumber());
            } else if (isalpha(current)) {
                tokens.push_back(tokenizeIdentifier());
            } else {
                switch (current) {
                    case '+': tokens.push_back({PLUS, "+"}); position++; break;
                    case '-': tokens.push_back({MINUS, "-"}); position++; break;
                    case '*': tokens.push_back({MULTIPLY, "*"}); position++; break;
                    case '/': tokens.push_back({DIVIDE, "/"}); position++; break;
                    case '=': tokens.push_back({ASSIGN, "="}); position++; break;
                    case '(': tokens.push_back({LEFT_PAREN, "("}); position++; break;
                    case ')': tokens.push_back({RIGHT_PAREN, ")"}); position++; break;
                    default: tokens.push_back({INVALID, std::string(1, current)}); position++; break;
                }
            }
        }
        tokens.push_back({END, ""});
        return tokens;
    }

private:
    Token tokenizeNumber() {
        std::string number;
        while (position < source.size() && isdigit(source[position])) {
            number += source[position++];
        }
        return {NUMBER, number};
    }

    Token tokenizeIdentifier() {
        std::string identifier;
        while (position < source.size() && isalnum(source[position])) {
            identifier += source[position++];
        }
        if (identifier == "PRINT") {
            return {PRINT, identifier};
        } else if (identifier == "INPUT") {
            return {INPUT, identifier};
        } else if (identifier == "WHILE") {
            return {WHILE, identifier};
        } else if (identifier == "IF") {
            return {IF, identifier};
        } else if (identifier == "ELSE") {
            return {ELSE, identifier};
        }
        return {IDENTIFIER, identifier};
    }

    std::string source;
    size_t position;
};

// Abstract Syntax Tree nodes
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual int evaluate(std::unordered_map<std::string, int>& variables) = 0;
};

struct NumberNode : public ASTNode {
    int value;
    explicit NumberNode(int value) : value(value) {}
    int evaluate(std::unordered_map<std::string, int>&) override {
        return value;
    }
};

struct VariableNode : public ASTNode {
    std::string name;
    explicit VariableNode(const std::string& name) : name(name) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        return variables[name];
    }
};

struct BinaryOpNode : public ASTNode {
    std::unique_ptr<ASTNode> left, right;
    TokenType op;
    BinaryOpNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right, TokenType op)
        : left(std::move(left)), right(std::move(right)), op(op) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        int leftVal = left->evaluate(variables);
        int rightVal = right->evaluate(variables);
        switch (op) {
            case PLUS: return leftVal + rightVal;
            case MINUS: return leftVal - rightVal;
            case MULTIPLY: return leftVal * rightVal;
            case DIVIDE: return leftVal / rightVal;
            default: return 0;
        }
    }
};

struct AssignmentNode : public ASTNode {
    std::string variable;
    std::unique_ptr<ASTNode> expression;
    AssignmentNode(const std::string& variable, std::unique_ptr<ASTNode> expression)
        : variable(variable), expression(std::move(expression)) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        int value = expression->evaluate(variables);
        variables[variable] = value;
        return value;
    }
};

struct PrintNode : public ASTNode {
    std::unique_ptr<ASTNode> expression;
    explicit PrintNode(std::unique_ptr<ASTNode> expression) : expression(std::move(expression)) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        int value = expression->evaluate(variables);
        std::cout << value << std::endl;
        return value;
    }
};

struct InputNode : public ASTNode {
    std::string variable;
    explicit InputNode(const std::string& variable) : variable(variable) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        int value;
        std::cout << "Enter value for " << variable << ": ";
        std::cin >> value;
        variables[variable] = value;
        return value;
    }
};

struct IfElseNode : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch;
    IfElseNode(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> thenBranch, std::unique_ptr<ASTNode> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        if (condition->evaluate(variables)) {
            return thenBranch->evaluate(variables);
        } else if (elseBranch) {
            return elseBranch->evaluate(variables);
        }
        return 0;
    }
};

struct WhileNode : public ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    WhileNode(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    int evaluate(std::unordered_map<std::string, int>& variables) override {
        while (condition->evaluate(variables)) {
            body->evaluate(variables);
        }
        return 0;
    }
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), position(0) {}

    std::unique_ptr<ASTNode> parse() {
        return parseStatement();
    }

private:
    std::unique_ptr<ASTNode> parseStatement() {
        if (tokens[position].type == PRINT) {
            position++;
            auto expr = parseExpression();
            return std::make_unique<PrintNode>(std::move(expr));
        } else if (tokens[position].type == INPUT) {
            position++;
            if (tokens[position].type == IDENTIFIER) {
                std::string varName = tokens[position].value;
                position++;
                return std::make_unique<InputNode>(varName);
            }
        } else if (tokens[position].type == IF) {
            position++;
            auto condition = parseExpression();
            if (tokens[position].type == LEFT_PAREN) {
                position++;
                auto thenBranch = parseStatement();
                std::unique_ptr<ASTNode> elseBranch = nullptr;
                if (tokens[position].type == ELSE) {
                    position++;
                    elseBranch = parseStatement();
                }
                return std::make_unique<IfElseNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
            }
        } else if (tokens[position].type == WHILE) {
            position++;
            auto condition = parseExpression();
            if (tokens[position].type == LEFT_PAREN) {
                position++;
                auto body = parseStatement();
                return std::make_unique<WhileNode>(std::move(condition), std::move(body));
            }
        } else if (tokens[position].type == IDENTIFIER) {
            std::string varName = tokens[position].value;
            position++;
            if (tokens[position].type == ASSIGN) {
                position++;
                auto expr = parseExpression();
                return std::make_unique<AssignmentNode>(varName, std::move(expr));
            }
        }
        return nullptr;
    }

    std::unique_ptr<ASTNode> parseExpression() {
        auto left = parseTerm();
        while (position < tokens.size() && (tokens[position].type == PLUS || tokens[position].type == MINUS)) {
            TokenType op = tokens[position].type;
            position++;
            auto right = parseTerm();
            left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
        }
        return left;
    }

    std::unique_ptr<ASTNode> parseTerm() {
        auto left = parseFactor();
        while (position < tokens.size() && (tokens[position].type == MULTIPLY || tokens[position].type == DIVIDE)) {
            TokenType op = tokens[position].type;
            position++;
            auto right = parseFactor();
            left = std::make_unique<BinaryOpNode>(std::move(left), std::move(right), op);
        }
        return left;
    }

    std::unique_ptr<ASTNode> parseFactor() {
        Token current = tokens[position];
        if (current.type == NUMBER) {
            position++;
            return std::make_unique<NumberNode>(std::stoi(current.value));
        } else if (current.type == IDENTIFIER) {
            position++;
            return std::make_unique<VariableNode>(current.value);
        } else if (current.type == LEFT_PAREN) {
            position++;
            auto expr = parseExpression();
            if (tokens[position].type == RIGHT_PAREN) {
                position++;
                return expr;
            }
        }
        return nullptr;
    }

    const std::vector<Token>& tokens;
    size_t position;
};

int main() {
    std::unordered_map<std::string, int> variables;
    std::string input;
    std::cout << "BASIC Interpreter\nEnter exit to quit.\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "EXIT") break;

        Tokenizer tokenizer(input);
        std::vector<Token> tokens = tokenizer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<ASTNode> ast = parser.parse();
        if (ast) {
            ast->evaluate(variables);
        } else {
            std::cout << "Syntax error!" << std::endl;
        }
    }
    return 0;
}
