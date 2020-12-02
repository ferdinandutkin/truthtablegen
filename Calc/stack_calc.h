#include "Calc.h"

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;



class expr_result {
	friend class expression_stack;

	bool result;
	std::wstring expr;

public:
	expr_result(std::wstring varible, bool value) : expr{ varible }, result{ value } {

	}

	expr_result(bool value) : expr{ value ? L"true" : L"false" }, result{ value } {

	}

	expr_result operator & (expr_result r) {
		return { expr + L" ∧ " + r.expr, result && r.result };

	}

	expr_result operator | (expr_result r) {
		return { expr + L" ∨ " + r.expr, result || r.result };

	}


	expr_result implication(expr_result r) {
		return { expr + L" ⇒ " + r.expr, !result || r.result };
	}

	expr_result operator ==(expr_result r) {
		return { expr + L" ⇔ " + r.expr, result == r.result };
	}

	expr_result operator ^ (expr_result r) {
		return { expr + L" ⊕ " + r.expr, result != r.result };
	}

	expr_result operator ! () {
		return { L"¬" + expr, !result };

	}

	operator std::wstring() {
		return expr;
	}

	operator bool() {
		return result;
	}
};


class expression_stack {
public:

	using T = expr_result;
	using unary_operation = std::function<T(T)>;
	using binary_operation = std::function<T(T, T)>;
	using operations = std::variant<unary_operation, binary_operation>;

	std::function<void(T)> on_eval = [](T) {};
	//òóò äîëæíî áûòü áîëüøå íóëÿ à òî ñêîáêè ñòàíîâÿòñÿ îïåðàòîðàìè
	std::unordered_map <std::wstring, uint16_t> priority = { {L"!", 5}, {L"&", 4}, {L"|", 3}, {L"^", 3}, {L">", 2}, {L"=", 1 } };

	std::unordered_map<std::wstring, operations> op_dictionary = {
	   {L"&", std::mem_fn(&T::operator&) },
	   {L"|", std::mem_fn(&T::operator|) },
	   {L"!", std::mem_fn(&T::operator!)},
	   {L"^", std::mem_fn(&T::operator^)},
	   {L">", std::mem_fn(&T::implication)},
	   {L"=", std::mem_fn(&T::operator==)},

	};

	std::unordered_map<std::wstring, bool> to_bool = { {L"true", true}, {L"false", false}, {L"1", true}, {L"0", false} };

	std::unordered_map<std::wstring, bool> variable_values;

	expression_stack(std::unordered_map<std::wstring, bool> variable_values) : variable_values{ variable_values } {

	}

	expression_stack() : variable_values{} {

	}




	std::stack<T> operands;
	std::stack<std::wstring> operators;




	void eval_top() {
		//íóæíà ëè ýòà ïðîâåðêà?
		if (operators.top() == L"(") {
			operators.pop();
			return;
		}

		std::visit(
			overload{
				[this](const unary_operation& operation) {

				T b = operands.top();
				operands.pop();

				T res = operation(b);
				on_eval(res);

				operands.push(res);
			},
				[this](const binary_operation& operation) {

				T r = operands.top();
				operands.pop();

				T l = operands.top();
				operands.pop();

				T res = operation(l, r);
				on_eval(res);

				operands.push(res);
			}
			}, op_dictionary[operators.top()]
				);
		operators.pop();


	}
	T force_eval() {
		while (not operators.empty()) {
			eval_top();
		}
		return operands.top();
	}

	void push(bool variable) {
		operands.push(variable);
	}
	void push(std::wstring token) {
		//std::cout << token << " ";
		if (variable_values.contains(token)) {
			if (not operators.empty() and
				operators.top() == L"(") {
				operands.push({ L"(" + token, variable_values[token] });
			}
			else
				operands.push({ token, variable_values[token] });
			
		}
		else if (to_bool.contains(token)) {
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

		else if (token == L")") {
			//добавим к токену
			operands.top().expr += L")";
			 

			//
			do {
				eval_top();
			} while (operators.top() != L"(");
			operators.pop();
		}
		else if (token == L"(") {

			operators.push(token);

		}
	}

};



class truth_table_gen {

	std::vector<std::wstring> variables;
	std::vector<std::wstring> expression;
	expression_stack stack;
public:
	truth_table_gen(std::wstring expression) {
		for (auto r_token : expression) {
			if (r_token != ' ') {
				if (stack.op_dictionary.contains(std::wstring(1, r_token))) {
					std::wstring op;
					op.push_back(r_token);
					this->expression.push_back(op);
				}
				else if (stack.to_bool.contains(std::wstring(1, r_token)) or r_token == L')' or r_token == L'(') {
					std::wstring var;
					var.push_back(r_token);
					this->expression.push_back(var);
				}

				else {
					std::wstring var;
					var.push_back(r_token);
					this->expression.push_back(var);
					if (std::find(variables.begin(), variables.end(), var) == variables.end()) {
						this->variables.push_back(var);
					}


				}
			}

		}
	}

	std::vector<std::vector<std::wstring>> generate() {
		//
		if (variables.empty()) {
			//expression_stack stack;
			for (auto&& token : expression) {
				stack.push(token);
			}
			return  { {stack.force_eval() ? L"1" : L"0"} };
		}
		std::vector<std::vector<std::wstring>> ret;
		//


		ret.push_back(variables); //èìåíà ïåðåìåííûûõ
	 
		 

		for (uint32_t i = 0; i < 2 << (variables.size() - 1); i++) {
			std::unordered_map<std::wstring, bool> varibale_values;

			std::vector<std::wstring> row;

			 

			for (int j = 0; auto && var : variables) {
				varibale_values[var] = (i >> (variables.size() - 1 - j)) & 1;

				row.push_back(varibale_values[var] ? L"1" : L"0");

				if (j >= variables.size()) break;
				j++;
			}


			expression_stack stack{ varibale_values };
			stack.on_eval = [&](expr_result r) {
				if (i == 0) {
					ret[0].push_back(r.operator std::wstring());
				}

				row.push_back(r.operator bool() ? L"1" : L"0");
			};
			for (auto&& token : expression) {
				if (varibale_values.contains(token)) {
					stack.push(token);
				}
				else {
					stack.push(token);
				}
			}
			stack.force_eval();
			ret.push_back(row);



		}

		return ret;

	}

};

