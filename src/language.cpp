#include "language.h"
#include "log.h"
#include "rapidxml.hpp"
#include "stringutils.hpp"

using namespace rapidxml;
using namespace stringutils;

Language::Language(const std::string& filename) : isValid(false), caseSensitive(false) {
#if defined(_WIN32)
    std::string platform = "win";
#elif defined(__APPLE__)
    std::string platform = "mac";
#elif defined(__linux__)
    std::string platform = "linux";
#endif
    std::string xmlstr = read(filename);
    if ( xmlstr == "" ) {
        Log::Write("Could not find language file.");
        return;
    }

    xml_document<> doc;
    try {
        doc.parse<0>((char*)xmlstr.c_str());
    } catch ( parse_error parse ) {
        Log::Write(parse.what());
    }

    xml_node<>* languageNode = doc.first_node("language");
    if ( !languageNode ) {
        Log::Write("Could not parse language file.");
        return;
    }

    if ( languageNode->first_attribute("case_sensitive") )
        caseSensitive = std::string(languageNode->first_attribute("case_sensitive")->value()) == "true";

    keywords = languageNode->first_node("keywords")->value();
    if ( languageNode->first_node("multilinecomments") != NULL ) {
        multiLineBegin = replace(languageNode->first_node("multilinecomments")->first_attribute("begin")->value(), "&quot", "\\\"");
        multiLineEnd = replace(languageNode->first_node("multilinecomments")->first_attribute("end")->value(), "&quot", "\\\"");
    }

    xml_node<>* ruleNode = languageNode->first_node("rule");
    while ( ruleNode ) {
        SyntaxRule rule;
        rule.regex = replace(ruleNode->first_attribute("regex")->value(), "&quot", "\\\"");
        rule.id = ruleNode->first_attribute("id")->value();
        rules.push_back(rule);
        ruleNode = ruleNode->next_sibling("rule");
    }

    xml_node<>* commandsNode = languageNode->first_node("commands");
    if ( commandsNode ) {
        // Vector of command names
        std::vector<std::string> commands;
        commands.push_back("compile");
        commands.push_back("build");
        commands.push_back("run");
        commands.push_back("debug");

        // Vector with the destination lists
        std::vector<std::vector<std::string>*> dest;
        dest.push_back(&compileCmds);
        dest.push_back(&buildCmds);
        dest.push_back(&runCmds);
        dest.push_back(&debugCmds);

        // Get all commands
        for ( unsigned int i = 0; i < commands.size(); i++ ) {
            // Get command string
            std::string commandStr = "";
            if ( commandsNode->first_node((commands[i] + ":" + platform).c_str()) ) {
                commandStr = replace(replace(commandsNode->first_node((commands[i] + ":" + platform).c_str())->value(), "\r\n", "\n"), "\r", "\n");
            } else if ( commandsNode->first_node(commands[i].c_str()) ) {
                commandStr = replace(replace(commandsNode->first_node(commands[i].c_str())->value(), "\r\n", "\n"), "\r", "\n");
            }

            // Add to list of commands
            if ( commandStr != "" ) {
                std::vector<std::string> commandLines = split(commandStr, '\n');
                for ( std::vector<std::string>::const_iterator it = commandLines.begin(); it != commandLines.end(); it++ ) {
                    std::string command = trim(*it);
                    if ( command != "" ) {
                        Log::Write("Adding " + commands[i] + " command '" + command + "'");
                        dest[i]->push_back(command);
                    }
                }
            }
        }
    }

    isValid = true;
}
