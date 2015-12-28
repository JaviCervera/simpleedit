#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>
#include <vector>

class Language {
public:
    struct SyntaxRule {
        std::string regex;
        std::string id;
    };

    Language(const std::string& filename);
    bool IsValid() const { return isValid; }
    bool IsCaseSensitive() const { return caseSensitive; }
    const std::string& Keywords() const { return keywords; }
    const std::string& MultiLineBegin() const { return multiLineBegin; }
    const std::string& MultiLineEnd() const { return multiLineEnd; }
    const std::vector<SyntaxRule>& Rules() const { return rules; }
    const std::vector<std::string>& CompileCommands() const { return compileCmds; }
    const std::vector<std::string>& BuildCommands() const { return buildCmds; }
    const std::vector<std::string>& RunCommands() const { return runCmds; }
    const std::vector<std::string>& DebugCommands() const { return debugCmds; }
private:
    bool isValid;
    bool caseSensitive;
    std::string keywords;
    std::string multiLineBegin;
    std::string multiLineEnd;
    std::vector<SyntaxRule> rules;
    std::vector<std::string> compileCmds;
    std::vector<std::string> buildCmds;
    std::vector<std::string> runCmds;
    std::vector<std::string> debugCmds;
};

#endif // LANGUAGE_H
