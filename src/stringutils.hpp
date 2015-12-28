#ifndef __STRINGUTILS_HPP__
#define __STRINGUTILS_HPP__

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace stringutils {

std::string replace( std::string str, std::string find, std::string replace );

inline int intValue( std::string str ) {
    int ret = 0;
    if ( str.length() > 2  &&  str[0] == '0'  &&  str[1] == 'x' ) {
        str = stringutils::replace(str, "0x", "");
        sscanf(str.c_str(), "%x", &ret);
    } else {
        sscanf(str.c_str(), "%i", &ret);
    }
    return ret;
}

inline double floatValue( std::string str )	{
    return atof(str.c_str());
}

inline std::string fromInt( int value )	{
    char str[32];
    sprintf( str, "%i", value );
    return std::string( str );
}

inline std::string fromFloat( float value )	{
    char str[32];
    sprintf( str, "%f", value );
    return std::string( str );
}

inline std::string hexFromInt( int value ) {
    char str[32];
    sprintf( str, "%x", value );
    return std::string( str );
}

inline size_t find( std::string str, const std::string& find ) {
    if ( str.find( find ) != std::string::npos )
        return str.find( find );
    else
        return -1;
}

inline std::string replace( std::string str, std::string find, std::string replace ) {
    std::string retstr = str;
    while ( retstr.find( find ) != std::string::npos )
        retstr.replace( retstr.find( find ), find.length(), replace );
    return retstr;
}

inline std::string stripExt( std::string file ) {
    return file.substr( 0, file.find( '.' ) );
}

inline std::string stripPath( std::string file )  {
    file = replace( file, "\\", "/" );
    return file.substr( file.rfind( '/' ) + 1, file.length() - file.rfind( '/' ) - 1 );
}

inline std::string extractExt( std::string file ) {
    return file.substr( 0, file.rfind( '.' ) + 1 );
}

inline std::string extractPath( std::string file ) {
    file = replace( file, "\\", "/" );
    return file.substr( 0, file.rfind( '/' ) + 1 );
}

inline std::vector<std::string> split( std::string str, char delim ) {
    if ( ( str.length() == 0 ) || ( delim == '\0' ) )
        return std::vector<std::string>();

    // Count strings
    int count = 1;
    for ( unsigned int i = 0; i < str.length(); i++ ) {
        char c = str[i];
        if ( c == delim )
            count++;
    }

    // Fill vector with strings
    std::vector<std::string> vector(count);
    std::string txt = "";
    count = 0;
    for ( unsigned int i = 0; i < str.length(); i++ ) {
        char c = str[i];
        if ( c == delim ) {
            vector[count] = txt;
            count++;
            txt = "";
        } else
            txt += c;
    }
    vector[count] = txt;
    return vector;
}

inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

inline std::string trim(std::string s) {
    return ltrim(rtrim(s));
}

inline std::string toLowerCase(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

inline std::string toUpperCase(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return out;
}

inline std::string leftString(const std::string& s, int count) {
    return s.substr(0, count);
}

inline std::string rightString(const std::string& s, int count) {
    return s.substr(s.length() - count, count);
}

inline std::string read(std::string file) {
  std::ifstream istream(file.c_str(), std::ios_base::in | std::ios_base::binary);
  std::stringstream sstream;
  sstream << istream.rdbuf();
  istream.close();
  return sstream.str();
}

inline void write(std::string file, std::string text, bool append = false) {
    // Open file to write or append
    FILE* f = fopen(file.c_str(), (append == 1) ? "a" : "w");

    // If it could not be opened, return
    if ( f == NULL ) return;

    // Write string
    fwrite(text.c_str(), sizeof(char), text.length(), f);

    // Close file
    fclose(f);
}

}

#endif	// __STRINGUTILS_HPP__
