#pragma once
#include "parser.h"
#include <cstring>
#include <iostream>
#include <string>

// #define Log
bool HTMLParser::create(const char *str, bool strictMode) {
  root = new DOMElement("document");
  char c;
  int i = 0;
  int statement = 0; // FSM
  stack<DOMElement *> st;
  st.push(root);
  DOMElement *elm;
  char tagName[65535] = {0};
  char attrName[65535] = {0};
  char attrValue[65535] = {0};
  char temp[65535] = {0};
  int isSingleQuote = -1;
  int tagIndex = 0, attrIndex = 0, valueIndex = 0;
  bool isSelfClosedTag = false;
  bool isScriptTag = false;
  vector<pair<string, string>> attrs;
  while ((c = str[i++]) != '\0') {
    if (statement == 0) {
      if (c == '<') {
        isSelfClosedTag = false;
        attrs.clear();
        statement = 1;
      }
    } else if (statement == 1) {
      if (isScriptTag && c != '/') {
        statement = 0;
        continue;
      }
      if (c == '!') {
        statement = 10;
      } else if (c == '/') {
        statement = 12;
        tagIndex = 0;
      } else {
        tagIndex = 0;
        tagName[tagIndex++] = c;
        statement = 2;
      }
    } else if (statement == 2) {
      if ((c == ' ' || c == '\n') || c == '/' || c == '>') {
        tagName[tagIndex] = '\0';
        // Create a new tag
        if (strcmp(tagName, "script") == 0) {
          isScriptTag = true;
        }
#ifdef Log
        std::cout << tagName << std::endl;
#endif
        elm = new DOMElement(tagName);

        if ((c == ' ' || c == '\n')) {
          statement = 3;
        } else if (c == '/') {
          statement = 8;
        } else if (c == '>') {
          statement = 9;
          i--;
        }
      } else {
        tagName[tagIndex++] = c;
      }
    } else if (statement == 3) {
      if ((c == ' ' || c == '\n')) {
        continue;
      } else if (c == '/') {
        statement = 8;
      } else if (c == '>') {
        statement = 9;
        i--;
      } else {
        statement = 4;
        attrIndex = 0;
        attrName[attrIndex++] = c;
      }

    } else if (statement == 4) {
      if (c == '/' || c == '>' || (c == ' ' || c == '\n') || c == '=') {
        attrName[attrIndex] = '\0';
        if ((c == ' ' || c == '\n')) {
          statement = 5;
        } else if (c == '/') {
          statement = 8;
        } else if (c == '>') {
          statement = 9;
          i--;
        } else if (c == '=') {
          statement = 6;
        }
      } else {
        attrName[attrIndex++] = c;
      }
    } else if (statement == 5) {
      if ((c == ' ' || c == '\n'))
        continue;
      else if (c == '=') {
        statement = 6;
      } else {
        // add a new attribute (default value) for tag
        statement = 4;
        attrIndex = 0;
        valueIndex = 0;
        attrValue[valueIndex] = '\0';
        i--;
#ifdef Log
        std::cout << attrName << ":" << attrValue << std::endl;
#endif
        elm->attrs.push_back(pair<string, string>(attrName, attrValue));
      }
    } else if (statement == 6) {
      if ((c == ' ' || c == '\n'))
        continue;
      else {
        valueIndex = 0;
        statement = 7;
        if (c == '\'') {
          isSingleQuote = 1;
        } else if (c == '"') {
          isSingleQuote = 0;
        } else {
          isSingleQuote = -1;
          attrValue[valueIndex++] = c;
        }
      }
    } else if (statement == 7) {
      if ((isSingleQuote == 1 && c == '\'') ||
          (isSingleQuote == 0 && c == '"') ||
          (isSingleQuote == -1 && ((c == ' ' || c == '\n') || c == '>'))) {
        if (isSingleQuote == -1 && c == '>') {
          statement = 9;
          i--;
        } else {
          statement = 3;
        }
        // add new attribute for tag
        attrValue[valueIndex] = '\0';
#ifdef Log
        std::cout << attrName << ":" << attrValue << std::endl;
#endif
        if (strcmp(attrName, "class") == 0) {
          char tempClassName[65535];
          int tempIndex = 0;
          for (int t = 0; t <= valueIndex; t++) {
            if (attrValue[t] == ' ' || t == valueIndex) {
              if (tempIndex != 0) {
                tempClassName[tempIndex++] = '\0';
                elm->className.push_back(string(tempClassName));
                tempIndex = 0;
              }
            } else {
              tempClassName[tempIndex++] = attrValue[t];
            }
          }
        } else if (strcmp(attrName, "id") == 0) {
          elm->idName = attrValue;
        }
        elm->attrs.push_back(*new pair<string, string>(attrName, attrValue));
        continue;
      }
      attrValue[valueIndex++] = c;
    } else if (statement == 8) {
      isSelfClosedTag = true;
      if (c == '>') {
        statement = 9;
        i--;
      } else {
        if (strictMode)
          return false;
      }
    } else if (statement == 9) {
      // push stack, record first character position of innerHTML
      if (isSelfClosedTag) {
        elm->innerHTML = "";
        st.top()->children.push_back(elm);
      } else {
        elm->startPosition = i;
#ifdef Log
        // std::cout << "Tag: " << tagName << " start position: " << str[i] <<
        // "("
        //           << i << ")" << std::endl;
#endif
      }
      if (!isSelfClosedTag) {
        st.top()->children.push_back(elm);
        st.push(elm);
      }
      statement = 0;
    } else if (statement == 10) {
      // doctype/describe tag is not processed
      if (c == '>') {
        statement = 11;
        i--;
      }
    } else if (statement == 11) {
      statement = 0;
    } else if (statement == 12) {
      if (c == ' ' || c == '\n' || c == '>') {
        tagName[tagIndex] = '\0';
        if (isScriptTag) {
          if (strcmp(tagName, "script") == 0) {
            isScriptTag = false;
          } else {
            statement = 0; // not match </script> tag
            continue;
          }
        }
        if ((c == ' ' || c == '\n')) {
          statement = 13;
        } else {
          statement = 14;
          i--;
        }
        continue;
      }
      tagName[tagIndex++] = c;
    } else if (statement == 13) {
      if ((c == ' ' || c == '\n'))
        continue;
      else if (c == '>') {
        i--;
        statement = 14;
      } else {
        if (strictMode)
          return false;
      }
    } else if (statement == 14) { // pop stack, calculate innerHTML value
      bool isFound = false;
      while (!st.empty()) {
#ifdef Log
        std::cout << "Compare:" << st.top()->name << " | " << tagName
                  << std::endl;
#endif
        DOMElement *e = st.top();
        if (e->name == tagName) {
          isFound = true;
          st.pop();
          // innerHTML
          const char *tempStr = str + e->startPosition;
          memcpy(temp, tempStr, i - 3 - strlen(tagName) - e->startPosition);
          temp[i - 3 - strlen(tagName) - e->startPosition] = '\0';
          e->innerHTML = string(temp);
#ifdef Log
          std::cout << "/" << tagName << std::endl;
          // std::cout << "Tag: " << tagName << " end position: " << str[i] <<
          // "("
          //           << i << ")" << std::endl;
          // std::cout << "innerHTML:" << e->innerHTML << std::endl;
#endif
          break;
        } else {
          if (strictMode)
            return false;
          st.pop();
        }
      }
      if (!isFound)
        return false;
      isScriptTag = false;
      statement = 0;
    }
  }
  isGenerator = true;
#ifdef Log
  queue<DOMElement *> q;
  q.push(root);
  while (!q.empty()) {
    DOMElement *e = q.front();
    q.pop();
    char className[65535];
    int tempIndex = 0;
    for (auto cls = e->className.begin(); cls != e->className.end(); ++cls) {
      for (auto i = cls->begin(); i != cls->end(); ++i) {
        className[tempIndex++] = *i;
      }
      className[tempIndex++] = ' ';
    }
    className[tempIndex] = '\0';
    std::cout << "[visualize]name: " << e->getTagName()
              << "; class=" << className << "; id=" << e->idName << std::endl;
    for (auto i = e->children.begin(); i != e->children.end(); ++i) {
      q.push(*i);
    }
  }
#endif
  return true;
}
HTMLParser::~HTMLParser() {
  stack<DOMElement *> st;
  st.push(root);
  while (!st.empty()) {
    DOMElement *elm = st.top();
    st.pop();
    for (int i = 0; i < elm->children.size(); i++) {
      st.push(elm->children[i]);
    }
    delete elm;
  }
}