#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cctype>
#include <sstream>
#include <iomanip>

using namespace std;

enum TokenType { NUMBER, VARIABLE, OPERATOR, LPREN, RPREN };

struct Token {
    TokenType type;
    string value;
};

string preprocess(string s) {
    string res = "";
    for (size_t i = 0; i < s.length(); i++) {
        if (isspace(s[i])) continue;
        if (s[i] == '*' && i + 1 < s.length() && s[i+1] == '*') {
            res += '^';
            i++;
            continue;
        }
        res += s[i];
    }
    string fixed = "";
    for (size_t i = 0; i < res.length(); i++) {
        fixed += res[i];
        if (i + 1 < res.length()) {
            if ((isdigit(res[i]) || res[i] == ')' || res[i] == 'n') && (res[i+1] == 'n' || res[i+1] == '(')) {
                fixed += '*';
            }
        }
    }
    return fixed;
}

vector<Token> tokenize(const string& s) {
    vector<Token> tokens;
    size_t i = 0;
    while (i < s.length()) {
        if (s[i] == 'n') {
            tokens.push_back({VARIABLE, "n"});
            i++;
        } else if (s[i] == '(') {
            tokens.push_back({LPREN, "("});
            i++;
        } else if (s[i] == ')') {
            tokens.push_back({RPREN, ")"});
            i++;
        } else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' || s[i] == '^') {
            tokens.push_back({OPERATOR, string(1, s[i])});
            i++;
        } else if (isdigit(s[i]) || s[i] == '.') {
            string num = "";
            while (i < s.length() && (isdigit(s[i]) || s[i] == '.')) {
                num += s[i];
                i++;
            }
            tokens.push_back({NUMBER, num});
        } else {
            i++;
        }
    }
    return tokens;
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

vector<Token> shuntingYard(const vector<Token>& tokens) {
    vector<Token> output;
    vector<Token> stack;
    for (const auto& token : tokens) {
        if (token.type == NUMBER || token.type == VARIABLE) {
            output.push_back(token);
        } else if (token.type == OPERATOR) {
            while (!stack.empty() && stack.back().type == OPERATOR &&
                   ((token.value != "^" && precedence(stack.back().value[0]) >= precedence(token.value[0])) ||
                    (token.value == "^" && precedence(stack.back().value[0]) > precedence(token.value[0])))) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(token);
        } else if (token.type == LPREN) {
            stack.push_back(token);
        } else if (token.type == RPREN) {
            while (!stack.empty() && stack.back().type != LPREN) {
                output.push_back(stack.back());
                stack.pop_back();
            }
            if (!stack.empty()) stack.pop_back();
        }
    }
    while (!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }
    return output;
}

double evaluateRPN(const vector<Token>& rpn, double nVal) {
    vector<double> stack;
    for (const auto& token : rpn) {
        if (token.type == NUMBER) {
            stack.push_back(stod(token.value));
        } else if (token.type == VARIABLE) {
            stack.push_back(nVal);
        } else if (token.type == OPERATOR) {
            if (stack.size() < 2) return 0;
            double b = stack.back(); stack.pop_back();
            double a = stack.back(); stack.pop_back();
            if (token.value == "+") stack.push_back(a + b);
            else if (token.value == "-") stack.push_back(a - b);
            else if (token.value == "*") stack.push_back(a * b);
            else if (token.value == "/") stack.push_back(b == 0 ? 0 : a / b);
            else if (token.value == "^") stack.push_back(pow(a, b));
        }
    }
    return stack.empty() ? 0 : stack.back();
}

int main() {
    string input;
    if (!getline(cin, input)) return 0;

    string clean = preprocess(input);
    vector<Token> tokens = tokenize(clean);
    vector<Token> rpn = shuntingYard(tokens);

    double n1 = 10000.0;
    double n2 = 100000.0;
    double n3 = 1000000.0;

    double v1 = evaluateRPN(rpn, n1);
    double v2 = evaluateRPN(rpn, n2);
    double v3 = evaluateRPN(rpn, n3);

    if (isnan(v3) || isinf(v3) || v3 > 1e15) {
        cout << "oo" << endl;
    } else if (v3 < -1e15) {
        cout << "-oo" << endl;
    } else {
        double d1 = abs(v2 - v1);
        double d2 = abs(v3 - v2);

        if (d2 < d1 && d2 < 0.1) {
            if (abs(v3) < 1e-4) {
                cout << 0 << endl;
            } else {
                cout << fixed << setprecision(20) << v3 << endl;
            }
        } else if (v3 > v2 && v2 > v1 && (v3 - v1) > 10.0) {
            cout << "oo" << endl;
        } else {
            cout << fixed << setprecision(20) << v3 << endl;
        }
    }

    return 0;
}
