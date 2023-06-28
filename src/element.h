#pragma once
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

using std::optional;
using std::pair;
using std::queue;
using std::stack;
using std::string;
using std::vector;

enum DOMType { TAG, ID, CLASS };
enum DOMAction { SEARCH, INCLUDE, PARENT, NEARBY, CLOSE };
class QueryDom {
public:
  DOMType type;
  string name;
  QueryDom(DOMType type, string name) : type(type), name(name) {}
};
class QueryStep {
public:
  DOMAction action;
  vector<QueryDom> name;
};

class DOMElement {
  friend class HTMLParser;
  string name;
  string innerHTML;
  vector<string> className;
  string idName = "";
  vector<pair<string, string>> attrs;
  vector<DOMElement *> children;
  DOMElement(const char *name) : name(name){};
  int startPosition;
  vector<QueryStep> parseQueryStr(const char *query);
  vector<DOMElement *> selector(vector<QueryStep> &steps);
  vector<pair<DOMElement *, DOMElement *>> finder(QueryStep &step,
                                                  queue<DOMElement *> q,
                                                  DOMElement *elm = nullptr,
                                                  DOMElement *prev = nullptr);

public:
  DOMElement *documentSelector(const char *query);
  vector<DOMElement *> documentSelectorAll(const char *query);
  const char *getInnerHTML() { return innerHTML.c_str(); };
  const char *getAttribute(const char *key) {
    for (auto i = attrs.begin(); i != attrs.end(); ++i) {
      if (strcmp(key, i->first.c_str()) == 0) {
        return i->second.c_str();
      }
    }
    return nullptr;
  };
  const char *getTagName() { return name.c_str(); };
};
