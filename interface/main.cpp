#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>

class SyntaxError : std::exception {
	virtual char const *what() const {
		return "Syntax error";
	}
};

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
		stream << indentation << "\t{&T::" << function.Name << "}->std::convertible_to<"
			<< function.ReturnType << "(T:: *)(" << function.ParameterList << ")>;\n";
	}
	stream << indentation << "\trequires !std::is_same_v<T, " << name << ">;\n";
	stream << indentation << "};\n\n";

	// Wrappers
	stream << indentation << "template<" << ImplName << " T>\n";
	stream << indentation << "void _IIG_" << name << "_Release(void *instance) {\n";
	stream << indentation << "\tdelete (T *)instance;\n";
	stream << indentation << "}\n\n";

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

	// VTable
	stream << indentation << "struct " << VTableName << " {\n";
	stream << indentation << "\tvoid *reserved; // Must be zero\n";
	stream << indentation << "\tvoid (*Release)(void *instance);\n\n";

	for (const Function &function : functions) {
		stream << indentation << '\t' << function.ReturnType << "(*" << function.Name << ")(void *instance";
		if (!function.Parameters.empty()) stream << ", ";
		stream << function.ParameterList << ");\n";
	}
	stream << '\n';

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\tstatic " << VTableName << " OfType() {\n";
	stream << indentation << "\t\t" << VTableName << " result;\n";
	stream << indentation << "\t\tresult.Release" << " = _IIG_" << name << "_Release<T>;\n";
	for (const Function &function : functions) {
		stream << indentation << "\t\tresult." << function.Name << " = _IIG_" << name << '_' << function.Name << "<T>;\n";
	}
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

	stream << indentation << "\tvoid Release() {\n";
	stream << indentation << "\t\tvTable->Release(instance);\n";
	stream << indentation << "\t}\n";

	for (const Function &function : functions) {
		stream << '\n';
		stream << indentation << "\t" << function.ReturnType << function.Name << '(' << function.ParameterList << ") {\n";
		stream << indentation << "\t\treturn vTable->" << function.Name << "(instance";
		for (const Identifier &parameter : function.Parameters) {
			stream << ", " << parameter.Name;
		}
		stream << ");\n";
		stream << indentation << "\t}\n";
	}

	stream << indentation << "};";

}

struct Token {
	std::string_view content;
	size_t begin, end;
	bool eof;

	operator bool() { return !eof; }
};

class Tokenizer {
	std::string_view content;
	std::size_t lineBegin = 0;

public:
	std::size_t i = 0;

private:
	void SkipWhitespace() {
		while (i < content.size() && IsWhitespace(content[i])) {
			if (content[i] == '\r' || content[i] == '\n') lineBegin = i + 1;
			i++;
		}
	}

public:
	Tokenizer(std::string_view content) : content(content) {};

	bool Eof() { return i >= content.size(); }

	Token Get() {
		SkipWhitespace();
		Token token;
		token.begin = i;

		if (Eof()) {
			token.end = i;
			token.content = std::string_view();
			token.eof = true;
			return token;
		}
		token.eof = false;

		if (IsAlphaNumeric(content[i])) {
			while (i < content.size() && IsAlphaNumeric(content[i])) i++;

			token.end = i;

			token.content = content.substr(token.begin, token.end - token.begin);
			return token;
		}
		i++;
		token.end = i;
		token.content = content.substr(token.begin, 1);
		return token;
	}

	Token GetUntil(char delimiter) {
		SkipWhitespace();
		Token token;
		token.begin = i;

		if (Eof()) {
			token.end = i;
			token.content = std::string_view();
			token.eof = true;
			return token;
		}
		token.eof = false;

		while (i < content.size() && content[i] != delimiter) i++;

		token.end = i;

		token.content = content.substr(token.begin, token.end - token.begin);
		return token;
	}

	char Peek() {
		SkipWhitespace();
		if (Eof()) return '\0';

		return content[i];
	}

	std::string_view LineIndentation() {
		size_t indentationEnd = lineBegin;
		while (indentationEnd < content.size() && (content[indentationEnd] == ' ' || content[indentationEnd] == '\t')) indentationEnd++;
		return content.substr(lineBegin, indentationEnd - lineBegin);
	}
};

void ProcessInterface(std::ostream &output, Tokenizer &tokenizer) {
	std::string_view indentation = tokenizer.LineIndentation();

	Token token = tokenizer.Get();
	if (!token || !IsLetterOrUnderscore(token.content[0])) throw SyntaxError();

	std::string_view name = token.content;

	if (tokenizer.Get().content != "{") throw SyntaxError();

	std::vector<Function> functions;

	while (true) {
		if (tokenizer.Peek() == '}') break;
		token = tokenizer.GetUntil(';');
		if (tokenizer.Get().content != ";") throw SyntaxError();
		functions.push_back(ParseFunction(token.content));
	}
	tokenizer.Get();
	if (tokenizer.Get().content != ";") throw SyntaxError();

	GenerateCode(output, indentation, name, functions);
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

	Tokenizer tokenizer(source);

	size_t unprocessedBegin = 0;
	while (Token token = tokenizer.Get()) {
		if (token.content == "interface") {
			fstream << source.substr(unprocessedBegin, token.begin - unprocessedBegin);
			ProcessInterface(fstream, tokenizer);
			unprocessedBegin = tokenizer.i;
		}
	}

	fstream << source.substr(unprocessedBegin, tokenizer.i - unprocessedBegin);

	fstream.close();

	return 0;
}