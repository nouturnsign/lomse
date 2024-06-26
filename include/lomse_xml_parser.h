//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-present, Lomse Developers
//
// Licensed under the MIT license.
//
// See LICENSE and NOTICE.md files in the root directory of this source tree.
//---------------------------------------------------------------------------------------

#ifndef __LOMSE_XML_PARSER_H__
#define __LOMSE_XML_PARSER_H__

#include "lomse_parser.h"
#include "lomse_internal_model.h"

#include <string>
using namespace std;

#include "pugixml/pugiconfig.hpp"
#include "pugixml/pugixml.hpp"
using namespace pugi;


namespace lomse
{

//forward declarations and definitions
typedef pugi::xml_document          XmlDocument;
typedef pugi::xml_attribute         XmlAttribute;


//---------------------------------------------------------------------------------------
class XmlNode
{
protected:
    pugi::xml_node  m_node;

    friend class XmlParser;
    XmlNode(pugi::xml_node node) : m_node(node) {}

public:
    XmlNode() {}
    XmlNode(const XmlNode* node) : m_node(node->m_node) {}

    string name() { return string(m_node.name()); }
    string value();
    XmlAttribute attribute(const string& name) {
        return m_node.attribute(name.c_str());
    }
    int type();

    enum {
		k_node_null = 0,	// Empty (null) node handle
		k_node_document,	// A document tree's absolute root
		k_node_element,		// Element tag, e.g., '<node/>'
		k_node_pcdata,		// Plain character data, e.g., 'text'
		k_node_cdata,		// Character data, e.g., '<![CDATA[text]]>'
		k_node_comment,		// Comment tag, e.g., '<!-- text -->'
		k_node_pi,			// Processing instruction, e.g., '<?name?>'
		k_node_declaration,	// Document declaration, e.g., '<?xml version="1.0"?>'
		k_node_doctype,		// Document type declaration, e.g., '<!DOCTYPE doc>'
        k_node_unknown
    };

    //XmlNode helper methods
    inline bool is_null() { return m_node.type() == pugi::xml_node_type::node_null; }
    //inline bool is_null() { return ! m_node; }      //TODO: is this valid? It seems to work!
	///get child with the specified name
    inline XmlNode child(const string& name) {
        return XmlNode( m_node.child(name.c_str()));
    }
    inline XmlNode first_child() { return XmlNode( m_node.first_child() ); }
    inline XmlNode next_sibling() { return XmlNode( m_node.next_sibling() ); }
    inline bool has_attribute(const string& name)
    {
        return m_node.attribute(name.c_str()) != nullptr;
    }
	///get value of attribute with the specified name
    inline string attribute_value(const string& name)
    {
        XmlAttribute attr = m_node.attribute(name.c_str());
        return string( attr.value() );
    }

    ptrdiff_t offset();

protected:
    string normalize_value(string str);

};

//---------------------------------------------------------------------------------------
class XmlParser : public Parser
{
private:
    XmlDocument m_doc;
    XmlNode m_root;
    string m_encoding;         //e.g., "utf-8"
    string m_errorMsg;
    int m_errorOffset;
    vector<ptrdiff_t> m_offsetData;     // offset -> line mapping
    bool m_fOffsetDataReady;
    string m_filename;

public:
    XmlParser(ostream& reporter=cout);
    ~XmlParser();

    void parse_file(const std::string& filename, bool fErrorMsg = true) override;
    void parse_text(const std::string& sourceText) override;
    void parse_cstring(char* sourceText);
    void parse_buffer(const void* buffer, size_t size);

    inline const string& get_error() { return m_errorMsg; }
    inline const string& get_encoding() { return m_encoding; }
    inline XmlNode* get_tree_root() { return &m_root; }
    int get_line_number(XmlNode* node);

protected:
    void parse_char_string(char* string);
    void find_root();
    bool build_offset_data(const char* file);
    std::pair<int, int> get_location(ptrdiff_t offset);

};


} //namespace lomse

#endif    //__LOMSE_XML_PARSER_H__
