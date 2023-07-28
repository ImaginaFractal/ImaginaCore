#include <iostream>
#include <string_view>
#include <vector>

bool IsWhitespace(char c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool IsDigit(char c) {
	return c >= '0' && c <= '9';
}

bool IsLetterOrUnderscore(char c) {
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsAlphaNumeric(char c) {
	return IsLetterOrUnderscore(c) || IsDigit(c);
}

std::string_view TrimWhitespace(std::string_view string) {
	size_t begin = 0;
	size_t end = string.size();

	while (begin < end && IsWhitespace(string[begin])) begin++;
	while (begin < end && IsWhitespace(string[end - 1])) end--;

	return string.substr(begin, end - begin);
}

struct Identifier {
	std::string_view Type;
	std::string_view Name;
};

Identifier ParseDeclaration(std::string_view declaration) {
	Identifier result;

	declaration = TrimWhitespace(declaration);
	size_t nameBegin = declaration.size();
	while (nameBegin > 0 && IsAlphaNumeric(declaration[nameBegin - 1])) nameBegin--;

	result.Type = declaration.substr(0, nameBegin);
	result.Name = declaration.substr(nameBegin);

	result.Type = TrimWhitespace(result.Type);

	if (result.Type.empty() || result.Name.empty() || !IsLetterOrUnderscore(result.Name[0])) throw std::invalid_argument("declaration");

	return result;
}

struct Function {
	std::string_view Type;
	std::string_view Name;
	std::vector<Identifier> Parameters;
};

Function ParseFunction(std::string_view declaration) {
	declaration = TrimWhitespace(declaration);

	if (declaration.empty() || declaration.back() != ')') throw std::invalid_argument("declaration");

	size_t parameterListBegin = declaration.find('(');
	if (parameterListBegin == declaration.npos) throw std::invalid_argument("declaration");

	std::string_view parameterList = declaration.substr(parameterListBegin + 1, declaration.size() - parameterListBegin - 2);
	declaration = declaration.substr(0, parameterListBegin);

	Identifier identifier = ParseDeclaration(declaration);

	Function result;
	result.Name = identifier.Name;
	result.Type = identifier.Type;

	parameterList = TrimWhitespace(parameterList);
	if (parameterList.size() == 0) return result; 

	size_t begin = 0;
	for (size_t i = 0; i < parameterList.size(); i++) {
		if (parameterList[i] == ',') {
			result.Parameters.push_back(ParseDeclaration(parameterList.substr(begin, i - begin)));
			begin = i + 1;
		}
	}
 	result.Parameters.push_back(ParseDeclaration(parameterList.substr(begin, parameterList.size() - begin)));

	return result;
}

std::string_view source = R"(Interface {
	float func(int a, float b);
	void func2();
};
)";


int main() {
	size_t begin = 0, i = 1;

	if (begin >= source.size() || !IsLetterOrUnderscore(source[begin])) return 1;
	while (i < source.size() && IsAlphaNumeric(source[i])) i++;

	std::string_view name = source.substr(begin, i - begin);
	begin = i;

	while (i < source.size() && IsWhitespace(source[i])) i++;
	if (i >= source.size() || source[i] != '{') return 1;
	i++;
	
	std::vector<Function> functions;

	while (true) {
		begin = i;
		while (i < source.size() && IsWhitespace(source[i])) i++;
		if (i >= source.size()) return 1;
		if (source[i] == '}') break;

		while (i < source.size() && source[i] != ';' && source[i] != '}') i++;
		if (i >= source.size() || source[i] == '}') return 1;

		functions.push_back(ParseFunction(source.substr(begin, i - begin)));
		i++;
	}

	std::cout << name << '\n';
	for (const Function &function : functions) {
		std::cout << function.Type << '\t';
		std::cout << function.Name << '\n';

		for (const Identifier &identifier : function.Parameters) {
			std::cout << '\t' << identifier.Type << '\t';
			std::cout << '\t' << identifier.Name << '\n';
		}
	}

	return 0;
}