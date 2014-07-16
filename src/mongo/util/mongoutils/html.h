// @file html.h

/*    Copyright 2014 MongoDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <sstream>

namespace mongo {

    namespace html {

        inline std::string _end() { return "</body></html>"; }
        inline std::string _table() { return "</table>\n\n"; }
        inline std::string _tr() { return "</tr>\n"; }

        inline std::string tr() { return "<tr>"; }
        inline std::string tr(const std::string& a, const std::string& b) {
            std::stringstream ss;
            ss << "<tr><td>" << a << "</td><td>" << b << "</td></tr>\n";
            return ss.str();
        }
        template <class T>
        inline std::string td(T x) {
            std::stringstream ss;
            ss << "<td>" << x << "</td>";
            return ss.str();
        }
        inline std::string td(const std::string& x) {
            return "<td>" + x + "</td>";
        }
        inline std::string th(const std::string& x) {
            return "<th>" + x + "</th>";
        }

        inline void tablecell( std::stringstream& ss , bool b ) {
            ss << "<td>" << (b ? "<b>X</b>" : "") << "</td>";
        }

        template< typename T>
        inline void tablecell( std::stringstream& ss , const T& t ) {
            ss << "<td>" << t << "</td>";
        }

        inline std::string table(const char *headers[] = 0, bool border = true) {
            std::stringstream ss;
            ss << "\n<table "
               << (border?"border=1 ":"")
               << "cellpadding=2 cellspacing=0>\n";
            if( headers ) {
                ss << "<tr>";
                while( *headers ) {
                    ss << "<th>" << *headers << "</th>";
                    headers++;
                }
                ss << "</tr>\n";
            }
            return ss.str();
        }

        inline std::string start(const std::string& title) {
            std::stringstream ss;
            ss << "<html><head>\n<title>";
            ss << title;
            ss << "</title>\n";

            ss << "<style type=\"text/css\" media=\"screen\">"
               "body { font-family: helvetica, arial, san-serif }\n"
               "table { border-collapse:collapse; border-color:#999; margin-top:.5em }\n"
               "th { background-color:#bbb; color:#000 }\n"
               "td,th { padding:.25em }\n"
               "</style>\n";

            ss << "</head>\n<body>\n";
            return ss.str();
        }

        inline std::string red(const std::string& contentHtml, bool color=true) {
            if( !color ) return contentHtml;
            std::stringstream ss;
            ss << "<span style=\"color:#A00;\">" << contentHtml << "</span>";
            return ss.str();
        }
        inline std::string grey(const std::string& contentHtml, bool color=true) {
            if( !color ) return contentHtml;
            std::stringstream ss;
            ss << "<span style=\"color:#888;\">" << contentHtml << "</span>";
            return ss.str();
        }
        inline std::string blue(const std::string& contentHtml, bool color=true) {
            if( !color ) return contentHtml;
            std::stringstream ss;
            ss << "<span style=\"color:#00A;\">" << contentHtml << "</span>";
            return ss.str();
        }
        inline std::string yellow(const std::string& contentHtml, bool color=true) {
            if( !color ) return contentHtml;
            std::stringstream ss;
            ss << "<span style=\"color:#A80;\">" << contentHtml << "</span>";
            return ss.str();
        }
        inline std::string green(const std::string& contentHtml, bool color=true) {
            if( !color ) return contentHtml;
            std::stringstream ss;
            ss << "<span style=\"color:#0A0;\">" << contentHtml << "</span>";
            return ss.str();
        }

        inline std::string p(const std::string& contentHtml) {
            std::stringstream ss;
            ss << "<p>" << contentHtml << "</p>\n";
            return ss.str();
        }

        inline std::string h2(const std::string& contentHtml) {
            std::stringstream ss;
            ss << "<h2>" << contentHtml << "</h2>\n";
            return ss.str();
        }

        /* does NOT escape the strings. */
        inline std::string a(const std::string& href,
                        const std::string& title="",
                        const std::string& contentHtml = "") {
            std::stringstream ss;
            ss << "<a";
            if( !href.empty() ) ss << " href=\"" << href << '"';
            if( !title.empty() ) ss << " title=\"" << title << '"';
            ss << '>';
            if( !contentHtml.empty() ) {
                ss << contentHtml << "</a>";
            }
            return ss.str();
        }

        /* escape for HTML display */
        inline std::string escape(const std::string& data) {
            std::string buffer;
            buffer.reserve( data.size() );
            for( size_t pos = 0; pos != data.size(); ++pos ) {
                switch( data[pos] ) {
                    case '&':
                        buffer.append( "&amp;" );
                        break;
                    case '\"':
                        buffer.append( "&quot;" );
                        break;
                    case '\'':
                        buffer.append( "&apos;" );
                        break;
                    case '<':
                        buffer.append( "&lt;" );
                        break;
                    case '>':
                        buffer.append( "&gt;" );
                        break;
                    default:
                        buffer.append( 1, data[pos] );
                        break;
                }
            }
            return buffer;
        }

    }

}
