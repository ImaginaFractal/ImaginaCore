#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

using std::operator""sv;

class SyntaxError : public std::exception {
public:
	virtual const char *what() const noexcept override {
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
	size_t begin = string.find_first_not_of(" \t\r\n"sv);
	if (begin == std::string_view::npos) return std::string_view();

	size_t end = string.find_last_not_of(" \t\r\n"sv) + 1;

	return string.substr(begin, end - begin);
}

struct Identifier {
	std::string_view Type;
	std::string_view Name;
	std::string_view Value;
	bool IsIncompleteInterface;
};

struct Function {
	std::string_view ReturnType;
	std::string_view Name;
	//std::string_view ParameterList;
	std::vector<Identifier> Parameters;
};

struct Interface {
	std::string_view Name;
	std::vector<Interface *> Bases;
	std::vector<Function> Functions;
};

std::unordered_map<std::string_view, Interface> Interfaces;
std::unordered_set<std::string_view> IncompleteInterfaces;

Identifier ParseIdentifier(std::string_view declaration) {
	Identifier result;

	size_t equalSignPos = declaration.find('=');
	if (equalSignPos != std::string_view::npos) {
		result.Value = TrimWhitespace(declaration.substr(equalSignPos + 1));
		declaration = declaration.substr(0, equalSignPos);
	} else {
		result.Value = std::string_view();
	}

	declaration = TrimWhitespace(declaration);

	size_t nameBegin = declaration.size();
	while (nameBegin > 0 && IsAlphaNumeric(declaration[nameBegin - 1])) nameBegin--;

	result.Type = declaration.substr(0, nameBegin);
	result.Name = declaration.substr(nameBegin);

	//result.Type = TrimWhitespace(result.Type);

	if (result.Type.empty() || result.Name.empty() || !IsLetterOrUnderscore(result.Name[0])) throw std::invalid_argument("declaration");

	result.IsIncompleteInterface = IncompleteInterfaces.find(TrimWhitespace(result.Type)) != IncompleteInterfaces.end();

	return result;
}

Function ParseFunction(std::string_view declaration) {
	declaration = TrimWhitespace(declaration);

	if (declaration.empty() || declaration.back() != ')') throw std::invalid_argument("declaration");

	size_t parameterListBegin = declaration.find('(');
	if (parameterListBegin == declaration.npos) throw std::invalid_argument("declaration");

	std::string_view parameterList = declaration.substr(parameterListBegin + 1, declaration.size() - parameterListBegin - 2);
	declaration = declaration.substr(0, parameterListBegin);

	Identifier identifier = ParseIdentifier(declaration);

	Function result;
	result.Name = identifier.Name;
	result.ReturnType = identifier.Type;

	parameterList = TrimWhitespace(parameterList);
	//result.ParameterList = parameterList;
	if (parameterList.empty()) return result;

	size_t begin = 0;
	for (size_t i = 0; i < parameterList.size(); i++) {
		if (parameterList[i] == ',') {
			result.Parameters.push_back(ParseIdentifier(parameterList.substr(begin, i - begin)));
			begin = i + 1;
		}
	}
 	result.Parameters.push_back(ParseIdentifier(parameterList.substr(begin, parameterList.size() - begin)));

	return result;
}

void GenerateSecondaryUpcasts(std::ostream &stream, std::string_view indentation, std::string_view vTable, const Interface &interface, std::unordered_set<std::string_view> &visitedBases) {
	bool inserted = visitedBases.insert(interface.Name).second;
	if (!inserted) return; // Already visited
	const std::vector<Interface *> &bases = interface.Bases;

	if (!bases.empty()) {
		GenerateSecondaryUpcasts(stream, indentation, vTable, *bases.front(), visitedBases);

		std::string vTableString(vTable);
		vTableString += '.';
		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			vTableString.resize(vTable.size() + 1);
			vTableString += base->Name;
			vTableString += "vTable";
			GenerateSecondaryUpcasts(stream, indentation, vTableString, *base, visitedBases);
		}
	}

	stream << indentation << "\toperator " << interface.Name << "() { return " << interface.Name << "(instance, &vTable->" << vTable << "); }\n";
}

void GenerateUpcasts(std::ostream &stream, std::string_view indentation, const Interface &interface, std::unordered_set<std::string_view> &visitedBases) {
	bool inserted = visitedBases.insert(interface.Name).second;
	if (!inserted) return; // Already visited
	const std::vector<Interface *> &bases = interface.Bases;

	if (!bases.empty()) {
		GenerateUpcasts(stream, indentation, *bases.front(), visitedBases);

		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			GenerateSecondaryUpcasts(stream, indentation, std::string(base->Name) + "vTable", *base, visitedBases);
		}
	}

	stream << indentation << "\toperator " << interface.Name << "() { return " << interface.Name << "(instance, vTable); }\n";
}

void GenerateMemberFunctions(std::ostream &stream, std::string_view indentation, std::string_view vTable, const Interface &interface, std::unordered_set<std::string_view> &visitedBases) {
	bool inserted = visitedBases.insert(interface.Name).second;
	if (!inserted) return; // Already visited
	const std::vector<Interface *> &bases = interface.Bases;

	if (!bases.empty()) {
		GenerateMemberFunctions(stream, indentation, vTable, *bases.front(), visitedBases);

		std::string vTableString(vTable);
		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			vTableString.resize(vTable.size());
			vTableString += base->Name;
			vTableString += "vTable.";
			GenerateMemberFunctions(stream, indentation, vTableString, *base, visitedBases);
		}
	}
	for (const Function &function : interface.Functions) {
		stream << indentation << "\t" << function.ReturnType << function.Name << '(';// << function.ParameterList << ") {\n";
		if (!function.Parameters.empty()) for (auto iterator = function.Parameters.begin();;) {
			if (iterator->IsIncompleteInterface) {
				stream << "const " << iterator->Type << "&";
			} else {
				stream << iterator->Type;
			}
			stream << iterator->Name;
			if (!iterator->Value.empty()) {
				stream << " = " << iterator->Value;
			}
			if (++iterator == function.Parameters.end()) break;
			stream << ", ";
		}
		stream << ") {\n";
		stream << indentation << "\t\treturn vTable->" << vTable << function.Name << "(instance";
		for (const Identifier &parameter : function.Parameters) {
			stream << ", ";
			if (parameter.IsIncompleteInterface) stream << "(IAny &)";
			stream << parameter.Name;
		}
		stream << ");\n";
		stream << indentation << "\t}\n\n";
	}
}

void GenerateCode(std::ostream &stream, std::string_view indentation, const Interface &interface) { //, std::string_view name, const std::vector<Function> &functions) {
	std::string_view name = interface.Name;
	const std::vector<Function> &functions = interface.Functions;
	std::string ImplName = std::string(name) + "Impl";
	std::string VTableName = std::string(name) + "VTable";
	bool isDerived = !interface.Bases.empty();
	const std::vector<Interface *> &bases = interface.Bases;
	//std::string_view firstBase = isDerived ? interface.DirectBases.front() : std::string_view();

	stream << "class " << name << ";\n\n";

	// Concept
	stream << indentation << "template<typename T>\n";
	stream << indentation << "concept " << ImplName << " = !Imagina::Interface<T>";
	for (Interface *base : interface.Bases) {
		stream << " && " << base->Name << "Impl<T>";
	}
	if (!functions.empty()) {
		stream << " && requires {\n";
		for (const Function &function : functions) {
			stream << indentation << "\tstatic_cast<" << function.ReturnType << "(T:: *)(";

			if (!function.Parameters.empty()) {
				stream << function.Parameters.front().Type << function.Parameters.front().Name;
				for (auto iterator = function.Parameters.begin() + 1; iterator != function.Parameters.end(); ++iterator) {
					const Identifier &parameter = *iterator;
					stream << ", " << parameter.Type << parameter.Name;
				}
			}
			stream << ")>(&T::" << function.Name << ");\n";
		}
		stream << indentation << '}';
	}
	stream << ";\n\n";

	// Wrappers
	stream << indentation << "template<" << ImplName << " T>\n";
	stream << indentation << "void _IIG_" << name << "_Release(void *instance) {\n";
	stream << indentation << "\tdelete (T *)instance;\n";
	stream << indentation << "}\n\n";

	for (const Function &function : functions) {
		stream << indentation << "template<" << ImplName << " T>\n";
		stream << indentation << function.ReturnType << "_IIG_" << name << '_' << function.Name << "(void *instance";
		for (const Identifier &parameter : function.Parameters) {
			stream << ", ";
			if (parameter.IsIncompleteInterface) {
				stream << "IAny ";
			} else {
				stream << parameter.Type;
			}
			stream << parameter.Name;
		}
		stream << ") {\n";
		stream << indentation << "\treturn ((T *)instance)->T::" << function.Name << '(';
		if (!function.Parameters.empty()) for (auto iterator = function.Parameters.begin();;) {
			if (iterator->IsIncompleteInterface) {
				stream << "(const " << iterator->Type << "&)";
			}
			stream << iterator->Name;
			if (++iterator == function.Parameters.end()) break;
			stream << ", ";
		}
		stream << ");\n";
		stream << indentation << "}\n\n";
	}

	// VTable
	stream << indentation << "struct " << VTableName;
	if (isDerived) {
		stream << " : " << bases.front()->Name << "VTable {\n";

		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			stream << indentation << '\t' << base->Name << "VTable " << base->Name << "vTable;\n";
		}
	} else {
		stream << " {\n";
		stream << indentation << "\tvoid *reserved = nullptr; // Must be null\n";
		stream << indentation << "\tvoid (*Release)(void *instance);\n\n";
	}

	for (const Function &function : functions) {
		stream << indentation << '\t' << function.ReturnType << "(*" << function.Name << ")(void *instance";
		//if (!function.Parameters.empty()) stream << ", ";
		//stream << function.ParameterList << ");\n";
		for (const Identifier &parameter : function.Parameters) {
			stream << ", ";
			if (parameter.IsIncompleteInterface) {
				stream << "IAny ";
			} else {
				stream << parameter.Type;
			}
			stream << parameter.Name;
		}
		stream << ");\n";
	}
	stream << '\n';

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\tstatic constexpr " << VTableName << " OfType(void (*release)(void *instance) = _IIG_" << name << "_Release<T>) {\n";
	stream << indentation << "\t\t" << VTableName << " result;\n";
	if (isDerived) {
		stream << indentation << "\t\t(" << bases.front()->Name << "VTable &)result = " << bases.front()->Name << "VTable::OfType<T>(release);\n";

		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			stream << indentation << "\t\tresult." << base->Name << "vTable = " << base->Name << "VTable::OfType<T>(release);\n";
		}
	} else {
		stream << indentation << "\t\tresult.Release = release;\n";
	}
	for (const Function &function : functions) {
		stream << indentation << "\t\tresult." << function.Name << " = _IIG_" << name << '_' << function.Name << "<T>;\n";
	}
	stream << '\n';
	stream << indentation << "\t\treturn result;\n";
	stream << indentation << "\t}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\tstatic const " << VTableName << " value;\n";
	stream << indentation << "};\n\n";

	// Interface
	stream << indentation << "class " << name << " final {\n";
	stream << indentation << "\tvoid *instance;\n";
	stream << indentation << "\tconst " << VTableName << " *vTable;\n\n";

	stream << indentation << "public:\n";
	stream << indentation << '\t' << name << "() = default;\n";
	stream << indentation << '\t' << name << "(const " << name << " &) = default;\n";
	stream << indentation << '\t' << name << '(' << name << " &&) = default;\n";
	stream << indentation << '\t' << name << "(std::nullptr_t) : instance(nullptr), vTable(nullptr) {}\n\n";

	stream << indentation << '\t' << name << "(void *instance, const " << VTableName << " *vTable) : instance(instance), vTable(vTable) {}\n";
	stream << indentation << "\texplicit " << name << "(IAny any) : instance(any.instance), vTable((" << VTableName << " *)any.vTable) {}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << '\t' << name << "(T &instance) : instance(&instance), vTable(&" << VTableName << "::value<T>) {}\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << '\t' << name << "(T *instance) : instance(instance), vTable(&" << VTableName << "::value<T>) {}\n\n";

	stream << indentation << '\t' << name << " &operator=(const " << name << " &) = default;\n";
	stream << indentation << '\t' << name << " &operator=(" << name << " &&) = default;\n\n";

	stream << indentation << "\t" << name << " &operator=(std::nullptr_t) { instance = nullptr; vTable = nullptr; return *this; }\n";
	stream << indentation << "\tbool operator==(const " << name << " x) const { return instance == x.instance; }\n";
	stream << indentation << "\tbool operator!=(const " << name << " x) const { return instance != x.instance; }\n\n";

	stream << indentation << "\toperator IAny() { return IAny(instance, vTable); }\n";
	stream << indentation << "\toperator bool() { return instance != nullptr; }\n\n";

	stream << indentation << "\ttemplate<" << ImplName << " T>\n";
	stream << indentation << "\texplicit operator T *() { return (T *)instance; }\n\n";

	if (isDerived) {
		std::unordered_set<std::string_view> visitedBases;
		GenerateUpcasts(stream, indentation, *bases.front(), visitedBases);
		
		for (auto iterator = bases.begin() + 1; iterator != bases.end(); ++iterator) {
			Interface *base = *iterator;
			GenerateSecondaryUpcasts(stream, indentation, std::string(base->Name) + "vTable", *base, visitedBases);
		}

		stream << '\n';
	}

	stream << indentation << "\tvoid Release() {\n";
	stream << indentation << "\t\tif (!instance) return;\n";
	stream << indentation << "\t\tvTable->Release(instance);\n";
	stream << indentation << "\t\t*this = nullptr;\n";
	stream << indentation << "\t}\n\n";

	{
		std::unordered_set<std::string_view> visitedBases;
		GenerateMemberFunctions(stream, indentation, "", interface, visitedBases);
	}

	stream << indentation << "\tusing _IIG_IsInterface = void;\n";
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
	void SkipWhitespaceAndComments() {
		while (i < content.size()) switch (content[i]) {
			case '\r':
			case '\n':
				lineBegin = i + 1;
				[[fallthrough]];
			case ' ':
			case '\t':
				i++;
				continue;
			case '/': {
				if (i + 1 < content.size() && content[i + 1] == '/') {
					i += 2;
					while (i < content.size() && content[i] != '\r' && content[i] != '\n') i++;
					continue;
				} else {
					return;
				}
			}
			case '#': {
				i++;
				while (i < content.size() && content[i] != '\r' && content[i] != '\n') i++;
				continue;
			}
			default:
				return;
		}
	}

public:
	Tokenizer(std::string_view content) : content(content) {};

	bool Eof() { return i >= content.size(); }

	Token Get() {
		SkipWhitespaceAndComments();
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
		SkipWhitespaceAndComments();
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
		SkipWhitespaceAndComments();
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

	Interface interface;
	interface.Name = token.content;

	token = tokenizer.Get();
	if (token.content == ";") {
		output << "class " << interface.Name << ";";
		if (Interfaces.find(interface.Name) == Interfaces.end()) IncompleteInterfaces.insert(interface.Name);
		return;
	}
	if (token.content == ":") do {
		token = tokenizer.Get();
		if (!token || !IsLetterOrUnderscore(token.content[0])) throw SyntaxError();
		auto iterator = Interfaces.find(token.content);
		if (iterator == Interfaces.end()) throw SyntaxError(); // FIXME: Exception type

		interface.Bases.push_back(&iterator->second);

		token = tokenizer.Get();
	} while (token.content == ",");

	if (token.content != "{") throw SyntaxError();

	while (true) {
		if (tokenizer.Peek() == '}') break;
		token = tokenizer.GetUntil(';');
		if (tokenizer.Get().content != ";") throw SyntaxError();
		interface.Functions.push_back(ParseFunction(token.content));
	}
	tokenizer.Get();
	if (tokenizer.Get().content != ";") throw SyntaxError();

	Interfaces.emplace(interface.Name, interface);
	IncompleteInterfaces.erase(interface.Name);

	GenerateCode(output, indentation, interface);
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

	std::string_view sourceFileName = argv[2];
	size_t lastSlash = sourceFileName.find_last_of("/\\");
	if (lastSlash != sourceFileName.npos) sourceFileName = sourceFileName.substr(lastSlash + 1);

	fstream << "#pragma once\n";
	fstream << "// This file is automatically generated. To make modifications, please edit interfaces/" << sourceFileName << "\n\n";
	fstream << "#include <concepts>\n";
	fstream << "#include <Imagina/interface>\n\n";

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