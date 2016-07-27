/*
 * File:   XMLPacket.cpp
 * Author: ahmed
 *
 * Created on 12 juin 2011, 19:32
 */

#include "StdAfx.h"
#include "XMLPacket.h"

namespace PushFramework
{
XMLPacket::~XMLPacket()
{
    
}

XMLPacket::XMLPacket(int typeId)
{
    xmlRoot = XMLNode::createXMLTopNode("root");

    std::stringstream ss;
    ss << typeId;

    xmlRoot.addAttribute("typeId", ss.str().c_str());
    isXmlFormat = true;
}

XMLPacket::XMLPacket( const std::string& data )
{
    isXmlFormat = false;
    this->data = data;
}

XMLPacket::XMLPacket()
{
    // waiting for decode.
    isXmlFormat = false;
}


XMLNode& XMLPacket::getRoot()
{
    return xmlRoot;
}

bool XMLPacket::Encode()
{
    if (!isXmlFormat)
        return true;

    if (xmlRoot.isEmpty())
        return false;

    int nSize;
    char *pString = xmlRoot.createXMLString(0, &nSize);
    data = pString;

    delete [] pString;
    return true;
}

bool XMLPacket::Decode( char* stringBuffer )
{
    xmlRoot = XMLNode::parseString(stringBuffer, "root");
    isXmlFormat = true;
    return !xmlRoot.isEmpty();
}

const char* XMLPacket::getArgumentAsText( const char* argName )
{
    if (xmlRoot.isEmpty())
        return "";
    XMLNode xChild = xmlRoot.getChildNode(argName);
    if (xChild.isEmpty())
    {
        return "";
    }
    const char* val = xChild.getAttribute("val");
    if (val == NULL)
        return "";
    return val;
}

int XMLPacket::getArgumentAsInt( const char* argName )
{
    return atoi(getArgumentAsText(argName));
}
bool XMLPacket::getArgumentAsBool( const char* argName )
{
    return getArgumentAsInt(argName) == 1;
}
void XMLPacket::setArgumentAsText( const char* argName, const char* val )
{
    if (xmlRoot.isEmpty())
        return;
    xmlRoot.addChild(argName).addAttribute("val", val);
}

void XMLPacket::setArgumentAsInt( const char* argName, int val )
{
    std::stringstream ss;
    ss << val;
    setArgumentAsText(argName, ss.str().c_str());
}

void XMLPacket::setArgumentAsBool( const char* argName, bool val )
{
    setArgumentAsText(argName, val ? "1" : "0");
}

std::string& XMLPacket::getData()
{
    return data;
}

int XMLPacket::getTypeId()
{
    if(xmlRoot.isEmpty())
        return 0;
    const char* val = xmlRoot.getAttribute("typeId");
    if(val)
        return atoi(val);
    return 0;
}

}












