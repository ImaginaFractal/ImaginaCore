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

int main() {
	std::string_view declaration = "const int* func(int a, const float &b)";

	Function result = ParseFunction(declaration);

	std::cout << result.Type << '\t';
	std::cout << result.Name << '\n';

	for (const Identifier &identifier : result.Parameters) {
		std::cout << identifier.Type << '\t';
		std::cout << identifier.Name << '\n';
	}




	return 0;
}