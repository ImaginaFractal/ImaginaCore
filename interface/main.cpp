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

	//result.Type = TrimWhitespace(result.Type);

	if (result.Type.empty() || result.Name.empty() || !IsLetterOrUnderscore(result.Name[0])) throw std::invalid_argument("declaration");

	return result;
}

struct Function {
	std::string_view ReturnType;
	std::string_view Name;
	std::string_view ParameterList;
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
	result.ReturnType = identifier.Type;

	parameterList = TrimWhitespace(parameterList);
	result.ParameterList = parameterList;
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
	int &func3(float *a, const int &b);
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

	std::string ImplName = std::string(name) + "Impl";
	std::string VTableName = std::string(name) + "VTable";

	// Concept
	std::cout << "template<typename T>\nconcept " << ImplName << " = requires {\n";
	for (const Function &function : functions) {
		std::cout << "\t{&T::" << function.Name << "}->std::same_as<";
		std::cout << function.ReturnType << "(T:: *)(" << function.ParameterList << ")>;\n";
	}
	std::cout << "\trequires !std::is_same_v<T, " << name << ">;\n};\n\n";

	// Wrappers
	for (const Function &function : functions) {
		std::cout << "template<" << ImplName << " T>\n";
		std::cout << function.ReturnType << "_IIG_" << name << '_' << function.Name << "(void *instance";
		if (!function.Parameters.empty()) std::cout << ", ";
		std::cout << function.ParameterList << ") {\n";
		std::cout << "\treturn ((T *)instance)->T::" << function.Name << '(';
		if (!function.ParameterList.empty()) for (auto iterator = function.Parameters.begin();;) {
			std::cout << iterator->Name;
			if (++iterator == function.Parameters.end()) break;
			std::cout << ", ";
		}
		std::cout << ");\n}\n\n";
	}

	// VTable
	std::cout << "struct " << VTableName << " {\n";
	std::cout << "\tvoid *reserved; // Must be zero\n";
	for (const Function &function : functions) {
		std::cout << '\t' << function.ReturnType << "(*" << function.Name << ")(void *instance";
		if (!function.Parameters.empty()) std::cout << ", ";
		std::cout << function.ParameterList << ");\n";
	}

	std::cout << "\n\ttemplate<" << ImplName << " T>\n";
	std::cout << "\tstatic " << VTableName << " OfType() {\n";
	std::cout << "\t\t" << VTableName << " result;\n";
	for (const Function &function : functions) {
		std::cout << "\t\tresult." << function.Name << " = _IIG_" << name << '_' << function.Name << "<T>;\n";
	}
	std::cout << "\t\treturn result;\n\t}\n\n";

	std::cout << "\ttemplate<" << ImplName << " T>\n";
	std::cout << "\tstatic " << VTableName << " value;\n};\n\n";

	// Interface
	std::cout << "class " << name << " {\n";
	std::cout << "\tvoid *instance;\n";
	std::cout << "\tconst " << VTableName << " *vTable;\n";

	std::cout << "\npublic:\n";
	std::cout << '\t' << name << "() = default;\n";
	std::cout << '\t' << name << "(const " << name << " &) = default;\n";
	std::cout << '\t' << name << '(' << name << " &&) = default;\n";

	std::cout << "\n\ttemplate<" << ImplName << " T>\n";
	std::cout << '\t' << name << "(T &instance) : instance(&instance), vTable(&" << VTableName << "::value<T>) {}\n\n";

	std::cout << "\t" << name << " &operator=(const " << name << " &) = default;\n";
	std::cout << "\t" << name << " &operator=(" << name << " &&) = default;\n";

	std::cout << "\n\ttemplate<" << ImplName << " T>\n";
	std::cout << "\texplicit operator T *() { return (T *)instance; }\n";

	for (const Function &function : functions) {
		std::cout << "\n\t" << function.ReturnType << function.Name << '(' << function.ParameterList << ") {\n";
		std::cout << "\t\treturn vTable->" << function.Name << "(instance";
		for (const Identifier &parameter : function.Parameters) {
			std::cout << ", " << parameter.Name;
		}
		std::cout << ");\n\t}\n";
	}

	std::cout << "};";

	return 0;
}