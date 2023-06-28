#pragma once
#include "element.h"
#include <iostream>
#include <stack>
#include <utility>
#include <vector>

using std::pair;
using std::stack;
using std::string;
using std::vector;

class HTMLParser {
  bool isGenerator;

public:
  DOMElement *root;
  HTMLParser() { isGenerator = false; }
  bool create(const char *str, bool strictMode = false);
  DOMElement *documentSelector(const char *query) {
    if (!isGenerator)
      return nullptr;
    return root->documentSelector(query);
  }
  vector<DOMElement *> documentSelectorAll(const char *query) {
    if (!isGenerator)
      return vector<DOMElement *>();
    return root->documentSelectorAll(query);
  };
  ~HTMLParser();
};
