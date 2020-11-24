#pragma once

#include <stack>
#include <string>
#include <functional>
#include <variant>
#include <unordered_map>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

template <typename T>
class expression_stack {

};

template<>
class expression_stack<bool> {
public:
	using unary_operation = std::function<bool(bool)>;
	using binary_operation = std::function<bool(bool, bool)>;
	using operations = std::variant<unary_operation, binary_operation>;

	//тут должно быть больше нуля а то скобки становятся операторами
	std::unordered_map <std::string, uint16_t> priority = { {"!", 5}, {"&", 4}, {"|", 3}, {"^", 3}, {">", 2}, {"=", 1 } };

	std::unordered_map<std::string, operations> op_dictionary = {
	   {"&", [](bool l, bool r) {return l && r; } },
	   {"|", [](bool l, bool r) {return l || r; } },
	   {"!", [](bool b) {return !b; }},
	   {"^", [](bool l, bool r) {return l != r; }},
	   {">", [](bool l, bool r) {return !l || r; }},
	   {"=", [](bool l, bool r) {return  l == r; }},

	};

	std::unordered_map<std::string, bool> to_bool = { {"true", true}, {"false", false}, {"1", true}, {"0", false} };

	expression_stack() {

	}




	std::stack <bool> operands;
	std::stack<std::string> operators;


	

	void eval_top() {
//нужна ли эта проверка?
		if (operators.top() == "(") {
			operators.pop();
			return;
		}
		
		std::visit(
			overload{
				[this](const unary_operation& operation) {

				bool b = operands.top();
				operands.pop();

				operands.push(operation(b));
			},
				[this](const binary_operation& operation) {

				bool r = operands.top();
				operands.pop();

				bool l = operands.top();
				operands.pop();

				operands.push(operation(l, r));
			}
			}, op_dictionary[operators.top()]
				);
		operators.pop();


	}
	bool force_eval() {
		while (not operators.empty()) {
			eval_top();
		}
		return operands.top();
	}

	void push(bool variable) {
		operands.push(variable);
	}
	void push(std::string token) {
		//std::cout << token << " ";
		if (to_bool.contains(token)) {
			//std::cout << "variable " << std::endl;
			operands.push(to_bool[token]);
		}
		else if (op_dictionary.contains(token)) {
			if (operators.empty()) {
				operators.push(token);
				//	std::cout << "empty " <<  std::endl;

			}

			else if (priority[token] > priority[operators.top()]) {
				//std::cout << "priority higher " << std::endl;
				operators.push(token);
			}
			else {
				//std::cout << "priority lower " << std::endl;
				if (token == operators.top() && operators.size() > operands.size() && op_dictionary[token].index() == 0) {
					operators.pop();
				}
				else {
					eval_top();
					push(token);

				}

			}


		}

		else if (token == ")") {


			do {
				eval_top();
			} while (operators.top() != "(");
			operators.pop();
		}
		else if (token == "(") {

			operators.push(token);

		}
	}

};



class truth_table_gen {

	std::vector<std::string> variables;
	std::vector<std::string> expression;
	expression_stack<bool> stack;
public:
	truth_table_gen(std::string expression) {
		for (auto r_token : expression) {
			if (r_token != ' ') {
				if (stack.op_dictionary.contains(std::string(1, r_token))) {
					std::string op;
					op.push_back(r_token);
					this->expression.push_back(op);
				}
				else if (stack.to_bool.contains(std::string(1, r_token)) or r_token == ')' or r_token == '(') {
					std::string var;
					var.push_back(r_token);
					this->expression.push_back(var);
				}

				else {
					std::string var;
					var.push_back(r_token);
					this->expression.push_back(var);
					if (std::find(variables.begin(), variables.end(), var) == variables.end()) {
						this->variables.push_back(var);
					}


				}
			}

		}
	}

	std::vector<std::vector<std::string>> generate() {
		//
		if (variables.empty()) {
			expression_stack<bool> stack;
			for (auto&& token : expression) {
				stack.push(token);
			}
			return  { {stack.force_eval() ? "1" : "0"} };
		}
		std::vector<std::vector<std::string>> ret;
		//


		ret.push_back(variables); //имена переменныых
		ret[0].push_back("f");
		ret[0].insert(ret[0].begin(), "");

		for (uint32_t i = 0; i < 2 << (variables.size() - 1); i++) {
			std::unordered_map<std::string, bool> varibale_values;

			std::vector<std::string> row;

			row.push_back(std::to_string(i));

			for (int j = 0; auto && var : variables) {
				varibale_values[var] = (i >> (variables.size() - 1 - j)) & 1;

				row.push_back(varibale_values[var] ? "1" : "0");

				if (j >= variables.size()) break;
				j++;
			}

			expression_stack<bool> stack;
			for (auto&& token : expression) {
				if (varibale_values.contains(token)) {
					stack.push(varibale_values[token]);
				}
				else {
					stack.push(token);
				}
			}
			row.push_back(stack.force_eval() ? "1" : "0");
			ret.push_back(row);



		}

		return ret;

	}

};
