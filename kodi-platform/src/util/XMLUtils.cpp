/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "XMLUtils.h"
#include <p8-platform/util/StringUtils.h>

bool XMLUtils::GetHex(const TiXmlNode* pRootNode, const char* strTag, uint32_t& hexValue)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  sscanf(pNode->FirstChild()->Value(), "%x", (uint32_t*) &hexValue );
  return true;
}


bool XMLUtils::GetUInt(const TiXmlNode* pRootNode, const char* strTag, uint32_t& uintValue)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  uintValue = atol(pNode->FirstChild()->Value());
  return true;
}

bool XMLUtils::GetLong(const TiXmlNode* pRootNode, const char* strTag, long& lLongValue)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  lLongValue = atol(pNode->FirstChild()->Value());
  return true;
}

bool XMLUtils::GetInt(const TiXmlNode* pRootNode, const char* strTag, int& iIntValue)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  iIntValue = atoi(pNode->FirstChild()->Value());
  return true;
}

bool XMLUtils::GetInt(const TiXmlNode* pRootNode, const char* strTag, int &value, const int min, const int max)
{
  if (GetInt(pRootNode, strTag, value))
  {
    if (value < min) value = min;
    if (value > max) value = max;
    return true;
  }
  return false;
}

bool XMLUtils::GetDouble(const TiXmlNode *root, const char *tag, double &value)
{
  const TiXmlNode* node = root->FirstChild(tag);
  if (!node || !node->FirstChild())
    return false;
  value = atof(node->FirstChild()->Value());
  return true;
}

bool XMLUtils::GetFloat(const TiXmlNode* pRootNode, const char* strTag, float& value)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  value = (float)atof(pNode->FirstChild()->Value());
  return true;
}

bool XMLUtils::GetFloat(const TiXmlNode* pRootElement, const char *tagName, float& fValue, const float fMin, const float fMax)
{
  if (GetFloat(pRootElement, tagName, fValue))
  { // check range
    if (fValue < fMin) fValue = fMin;
    if (fValue > fMax) fValue = fMax;
    return true;
  }
  return false;
}

bool XMLUtils::GetBoolean(const TiXmlNode* pRootNode, const char* strTag, bool& bBoolValue)
{
  const TiXmlNode* pNode = pRootNode->FirstChild(strTag );
  if (!pNode || !pNode->FirstChild())
    return false;
  std::string strEnabled = pNode->FirstChild()->Value();
  StringUtils::ToLower(strEnabled);
  if (strEnabled == "off" || strEnabled == "no" || strEnabled == "disabled" || strEnabled == "false" || strEnabled == "0" )
    bBoolValue = false;
  else
  {
    bBoolValue = true;
    if (strEnabled != "on" && strEnabled != "yes" && strEnabled != "enabled" && strEnabled != "true")
      return false; // invalid bool switch - it's probably some other string.
  }
  return true;
}

bool XMLUtils::GetString(const TiXmlNode* pRootNode, const char* strTag, std::string& strStringValue)
{
  const TiXmlElement* pElement = pRootNode->FirstChildElement(strTag );
  if (!pElement)
    return false;
  const TiXmlNode* pNode = pElement->FirstChild();
  if (pNode != NULL)
  {
    strStringValue = pNode->Value();
    return true;
  }
  strStringValue.clear();
  return false;
}

bool XMLUtils::HasChild(const TiXmlNode* pRootNode, const char* strTag)
{
  const TiXmlElement* pElement = pRootNode->FirstChildElement(strTag);
  if (!pElement)
    return false;
  const TiXmlNode* pNode = pElement->FirstChild();
  return (pNode != NULL);
}

/*!
  Returns true if the encoding of the document is other then UTF-8.
  /param strEncoding Returns the encoding of the document. Empty if UTF-8
*/
bool XMLUtils::GetEncoding(const TiXmlDocument* pDoc, std::string& strEncoding)
{
  const TiXmlNode* pNode=NULL;
  while ((pNode=pDoc->IterateChildren(pNode)) && pNode->Type()!=TiXmlNode::TINYXML_DECLARATION) {}
  if (!pNode)
    return false;
  const TiXmlDeclaration* pDecl=pNode->ToDeclaration();
  if (!pDecl)
    return false;
  strEncoding=pDecl->Encoding();
  if (StringUtils::EqualsNoCase(strEncoding, "UTF-8") || StringUtils::EqualsNoCase(strEncoding, "UTF8"))
    strEncoding.clear();

  StringUtils::ToUpper(strEncoding);
  return !strEncoding.empty(); // Other encoding then UTF8?
}

/*!
  Returns true if the encoding of the document is specified as as UTF-8
  /param strXML The XML file (embedded in a string) to check.
*/
bool XMLUtils::HasUTF8Declaration(const std::string &strXML)
{
  std::string test = strXML;
  StringUtils::ToLower(test);
  // test for the encoding="utf-8" string
  if (test.find("encoding=\"utf-8\"") != std::string::npos)
    return true;
  // TODO: test for plain UTF8 here?
  return false;
}

bool XMLUtils::GetPath(const TiXmlNode* pRootNode, const char* strTag, std::string& strStringValue)
{
  const TiXmlElement* pElement = pRootNode->FirstChildElement(strTag);
  if (!pElement)
    return false;

  const TiXmlNode* pNode = pElement->FirstChild();
  if (pNode != NULL)
  {
    strStringValue = pNode->Value();
    return true;
  }
  strStringValue.clear();
  return false;
}

void XMLUtils::SetStringArray(TiXmlNode* pRootNode, const char *strTag, const std::vector<std::string>& arrayValue)
{
  for (unsigned int i = 0; i < arrayValue.size(); i++)
    SetString(pRootNode, strTag, arrayValue.at(i));
}

void XMLUtils::SetString(TiXmlNode* pRootNode, const char *strTag, const std::string& strValue)
{
  TiXmlElement newElement(strTag);
  TiXmlNode *pNewNode = pRootNode->InsertEndChild(newElement);
  if (pNewNode)
  {
    TiXmlText value(strValue);
    pNewNode->InsertEndChild(value);
  }
}

void XMLUtils::SetInt(TiXmlNode* pRootNode, const char *strTag, int value)
{
  auto strValue = StringUtils::Format("%i", value);
  SetString(pRootNode, strTag, strValue);
}

void XMLUtils::SetLong(TiXmlNode* pRootNode, const char *strTag, long value)
{
  auto strValue = StringUtils::Format("%l", value);
  SetString(pRootNode, strTag, strValue);
}

void XMLUtils::SetFloat(TiXmlNode* pRootNode, const char *strTag, float value)
{
  auto strValue = StringUtils::Format("%f", value);
  SetString(pRootNode, strTag, strValue);
}

void XMLUtils::SetBoolean(TiXmlNode* pRootNode, const char *strTag, bool value)
{
  SetString(pRootNode, strTag, value ? "true" : "false");
}

void XMLUtils::SetHex(TiXmlNode* pRootNode, const char *strTag, uint32_t value)
{
  auto strValue = StringUtils::Format("%x", value);
  SetString(pRootNode, strTag, strValue);
}

void XMLUtils::SetPath(TiXmlNode* pRootNode, const char *strTag, const std::string& strValue)
{
  TiXmlElement newElement(strTag);
  newElement.SetAttribute("pathversion", path_version);
  TiXmlNode *pNewNode = pRootNode->InsertEndChild(newElement);
  if (pNewNode)
  {
    TiXmlText value(strValue);
    pNewNode->InsertEndChild(value);
  }
}
