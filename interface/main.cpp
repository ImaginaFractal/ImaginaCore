#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>

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

void GenerateCode(std::ostream &stream, std::string_view indentation, std::string_view name, const std::vector<Function> &functions) {
	std::string ImplName = std::string(name) + "Impl";
	std::string VTableName = std::string(name) + "VTable";

	stream << "class " << name << ";\n\n";

	// Concept
	stream << indentation << "template<typename T>\n";
	stream << indentation << "concept " << ImplName << " = requires {\n";
	for (const Function &function : functions) {
		stream << indentation << "\t{&T::" << function.Name << "}->std::same_as<"
			<< function.ReturnType << "(T:: *)(" << function.ParameterList << ")>;\n";
	}
	stream << indentation << "\trequires !std::is_same_v<T, " << name << ">;\n";
	stream << indentation << "};\n\n";

	// Wrappers
	for (const Function &function : functions) {
		stream << indentation << "template<" << ImplName << " T>\n";
		stream << indentation << function.ReturnType << "_IIG_" << name << '_' << function.Name << "(void *instance";
		if (!function.Parameters.empty()) stream << ", ";
		stream << function.ParameterList << ") {\n";
		stream << indentation << "\treturn ((T *)instance)->T::" << function.Name << '(';
		if (!function.ParameterList.empty()) for (auto iterator = function.Parameters.begin();;) {
			stream << iterator->Name;
			if (++iterator == function.Parameters.end()) break;
			stream << ", ";
		}
		stream << ");\n";
		stream << indentation << "}\n\n";
	}

	stream << indentation << "template<" << ImplName << " T>\n";
	stream << indentation << "void _IIG_" << name << "_Release(void *instance) {\n";
	stream << indentation << "\tdelete (T *)instance;\n";
	stream << indentation << "}\n\n";

	// VTable
	stream << indentation << "struct " << VTableName << " {\n";
	stream << indentation << "\tvoid *reserved; // Must be zero\n";
	for (const Function &function : functions) {
		stream << indentation << '\t' << function.ReturnType << "(*" << function.Name << ")(void *instance";
		if (!function.Parameters.empty()) stream << ", ";
		stream << function.ParameterList << ");\n";
	}
	stream << '\n';

	stream << indentation << "\tvoid (*Release)(void *instance);\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\tstatic " << VTableName << " OfType() {\n";
	stream << indentation << "\t\t" << VTableName << " result;\n";
	for (const Function &function : functions) {
		stream << indentation << "\t\tresult." << function.Name << " = _IIG_" << name << '_' << function.Name << "<T>;\n";
	}
	stream << indentation << "\t\tresult.Release" << " = _IIG_" << name << "_Release<T>;\n";
	stream << indentation << "\t\treturn result;\n";
	stream << indentation << "\t}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\tstatic " << VTableName << " value;\n";
	stream << indentation << "};\n\n";

	// Interface
	stream << indentation << "class " << name << " {\n";
	stream << indentation << "\tvoid *instance;\n";
	stream << indentation << "\tconst " << VTableName << " *vTable;\n\n";

	stream << indentation << "public:\n";
	stream << indentation << '\t' << name << "() = default;\n";
	stream << indentation << '\t' << name << "(const " << name << " &) = default;\n";
	stream << indentation << '\t' << name << '(' << name << " &&) = default;\n\n";

	stream << indentation << '\t' << name << "(void *instance, const " << VTableName << " *vTable) : instance(instance), vTable(vTable) {}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << '\t' << name << "(T &instance) : instance(&instance), vTable(&" << VTableName << "::value<T>) {}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << '\t' << name << "(T *instance) : instance(instance), vTable(&" << VTableName << "::value<T>) {}\n\n";

	stream << indentation << "\t" << name << " &operator=(const " << name << " &) = default;\n";
	stream << indentation << "\t" << name << " &operator=(" << name << " &&) = default;\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\texplicit operator T *() { return (T *)instance; }\n\n";

	for (const Function &function : functions) {
		stream << indentation << "\t" << function.ReturnType << function.Name << '(' << function.ParameterList << ") {\n";
		stream << indentation << "\t\treturn vTable->" << function.Name << "(instance";
		for (const Identifier &parameter : function.Parameters) {
			stream << ", " << parameter.Name;
		}
		stream << ");\n";
		stream << indentation << "\t}\n\n";
	}

	stream << indentation << "\tvoid Release() {\n";
	stream << indentation << "\t\tvTable->Release(instance);\n";
	stream << indentation << "\t}\n";

	stream << indentation << "};";

}

size_t ProcessInterface(std::ostream &output, std::string_view source, size_t begin, std::string_view indentation) {
	while (begin < source.size() && IsWhitespace(source[begin])) begin++;

	size_t i = begin + 1;

	if (begin >= source.size() || !IsLetterOrUnderscore(source[begin])) throw std::invalid_argument("source");
	while (i < source.size() && IsAlphaNumeric(source[i])) i++;

	std::string_view name = source.substr(begin, i - begin);
	begin = i;

	while (i < source.size() && IsWhitespace(source[i])) i++;
	if (i >= source.size() || source[i] != '{') throw std::invalid_argument("source");
	i++;

	std::vector<Function> functions;

	while (true) {
		begin = i;
		while (i < source.size() && IsWhitespace(source[i])) i++;
		if (i >= source.size()) throw std::invalid_argument("source");
		if (source[i] == '}') break;

		while (i < source.size() && source[i] != ';' && source[i] != '}') i++;
		if (i >= source.size() || source[i] == '}') throw std::invalid_argument("source");

		functions.push_back(ParseFunction(source.substr(begin, i - begin)));
		i++;
	}
	i++;
	while (i < source.size() && IsWhitespace(source[i])) i++;
	if (i >= source.size() || source[i] != ';') throw std::invalid_argument("source");

	GenerateCode(output, indentation, name, functions);
	return i;
}

int main(int argc, char **argv) {
	if (argc < 3) return 1;

	std::ifstream sourceFile(argv[2], std::ios::in);
	sourceFile.seekg(0, std::ios::end);
	size_t size = sourceFile.tellg();
	char *data = (char *)malloc(size);
	sourceFile.seekg(0, std::ios::beg);
	sourceFile.read(data, size);
	size = sourceFile.gcount();

	std::string_view source(data, size);
	std::ofstream fstream(argv[1], std::ios::out);

	size_t lineBegin = 0;

	for (size_t i = 0; i < source.size(); i++) {
		if (source[i] == '\r' || source[i] == '\n') lineBegin = i + 1;

		constexpr size_t letterCount = sizeof("interface") / sizeof(char) - 1;
		if (source[i] == 'i' &&
			i + letterCount < source.size() &&
			IsWhitespace(source[i + letterCount]) &&
			source.substr(i, letterCount) == "interface") {
			i += letterCount + 1;
			size_t indentationEnd = lineBegin;
			while (source[indentationEnd] == ' ' || source[indentationEnd] == '\t') indentationEnd++;
			i = ProcessInterface(fstream, source, i, source.substr(lineBegin, indentationEnd - lineBegin));
			continue;
		}

		fstream.put(source[i]);
	}

	fstream.close();

	return 0;
}