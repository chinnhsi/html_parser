#pragma once
#include "element.h"
#include <iostream>
#include <optional>
#include <stack>
#include <utility>
#include <vector>
// #define Log

/*
#id
.class
tag
tag#id
#id .class
#id>.class
#id~.class
#id+.class
*/
vector<QueryStep> DOMElement::parseQueryStr(const char *query) {
  int i = 0, statement = 0, strIndex = 0;
  char c;
  char name[1024];
  DOMType t;
  DOMAction action = SEARCH;
  vector<QueryStep> v;
  int index = -1;
  while ((c = query[i++]) != '\0') {
    if (statement == 0) {
      strIndex = 0;
      if (c == '#') {
        t = ID;
      } else if (c == '.') {
        t = CLASS;
      } else {
        t = TAG;
        name[strIndex++] = c;
      }
      v.push_back(QueryStep());
      index++;
      statement = 1;
    } else if (statement == 1) {
      if (c == '#' || c == '.' || c == ' ' || c == '>' || c == '~' ||
          c == '+') {
        v[index].action = action;
        name[strIndex] = '\0';
        v[index].name.push_back(QueryDom(t, string(name)));
        strIndex = 0;
        if (c == '#' || c == '.') {
          if (c == '#') {
            t = ID;
          } else {
            t = CLASS;
          }
        } else if (c == ' ') {
          action = INCLUDE;
          statement = 0;
        } else if (c == '>') {
          action = PARENT;
          statement = 0;
        } else if (c == '~') {
          action = NEARBY;
          statement = 0;
        } else if (c == '+') {
          action = CLOSE;
          statement = 0;
        }
      } else {
        name[strIndex++] = c;
      }
    }
  }
  v[index].action = action;
  name[strIndex] = '\0';
  v[index].name.push_back(QueryDom(t, string(name)));
  return v;
}

DOMElement *DOMElement::documentSelector(const char *query) {
  vector<QueryStep> v = parseQueryStr(query);
  auto r = selector(v);
  if (r.size() == 0) {
    return nullptr;
  } else {
    return r[0];
  }
}
vector<DOMElement *> DOMElement::documentSelectorAll(const char *query) {
  vector<QueryStep> v = parseQueryStr(query);
  auto r = selector(v);
  return r;
}

vector<DOMElement *> DOMElement::selector(vector<QueryStep> &steps) {

#ifdef Log
  for (auto step = steps.begin(); step != steps.end(); ++step) {
    std::cout << "step ===== action: " << step->action << std::endl;
    for (auto d = step->name.begin(); d != step->name.end(); d++) {
      std::cout << "type: " << d->type << "; name: " << d->name << std::endl;
    }
  }
#endif
  vector<pair<DOMElement *, DOMElement *>> vec;
  vector<pair<DOMElement *, DOMElement *>> lastvec;
  vec.push_back(pair<DOMElement *, DOMElement *>(this, this));
  for (auto step = steps.begin(); step != steps.end(); ++step) {

#ifdef Log
    std::cout << "============ " << std::endl;
#endif
    lastvec = vec;
    vec = vector<pair<DOMElement *, DOMElement *>>();
    for (auto i = lastvec.begin(); i != lastvec.end(); ++i) {
      DOMElement *elm = i->first, *last = i->second;
#ifdef Log
      std::cout << "[Now Find]selected: " << elm->getTagName()
                << "; parent: " << last->getTagName() << std::endl;
#endif
      DOMAction &act = step->action;
      queue<DOMElement *> q = queue<DOMElement *>(); // BFS
      vector<pair<DOMElement *, DOMElement *>> r;
      if (act == NEARBY || act == CLOSE) {
        for (auto i = last->children.begin(); i != last->children.end(); ++i) {
          q.push(*i);
          r = finder(*step, q, last, elm);
#ifdef Log
          std::cout << (*i)->getTagName() << std::endl;
#endif
        }
      } else {
        for (auto i = elm->children.begin(); i != elm->children.end(); ++i) {
          q.push(*i);
#ifdef Log
          std::cout << (*i)->getTagName() << std::endl;
#endif
          r = finder(*step, q, elm, elm);
        }
      }

      for (auto t = r.begin(); t != r.end(); ++t) {
#ifdef Log
        std::cout << "[match]selected: " << t->first->getTagName()
                  << "; parent: " << t->second->getTagName() << std::endl;
#endif
        vec.push_back(*t);
      }
    }
  }
  vector<DOMElement *> ret;
  for (auto t = vec.begin(); t != vec.end(); ++t) {
    ret.push_back(t->first);
#ifdef Log
    std::cout << "[return]selected: " << t->first->getTagName() << std::endl;
#endif
  }
  return ret;
}
vector<pair<DOMElement *, DOMElement *>>
DOMElement::finder(QueryStep &step, queue<DOMElement *> q, DOMElement *parent,
                   DOMElement *prev) {
  DOMAction &act = step.action;
  vector<pair<DOMElement *, DOMElement *>> ret =
      vector<pair<DOMElement *, DOMElement *>>();
  DOMElement *last = parent, *elm = parent;
  bool nextFound = false;
  while (!q.empty()) {
    DOMElement &e = *(q.front());
    q.pop();
    if (!nextFound && (act == NEARBY || act == CLOSE)) {

#ifdef Log
      std::cout << "[found compare]queue front element name: " << e.name
                << "; isEqual:" << (&e == prev ? "true" : "false") << std::endl;
#endif
      if (&e == prev)
        nextFound = true;
      continue;
    }
    bool f = true;
    for (int b = 0; b < step.name.size(); b++) {
      const QueryDom &d = step.name[b];

#ifdef Log
      char className[65535];
      int tempIndex = 0;
      for (auto cls = e.className.begin(); cls != e.className.end(); ++cls) {
        for (auto i = cls->begin(); i != cls->end(); ++i) {
          className[tempIndex++] = *i;
        }
        className[tempIndex++] = ' ';
      }
      className[tempIndex] = '\0';
      std::cout << "[compare]search type: " << d.type
                << "; search name: " << d.name << "; elm name: " << e.name
                << "; elm class: " << className << "; elm id: " << e.idName
                << std::endl;
#endif
      if ((d.type == TAG && d.name == e.name) ||
          (d.type == ID && d.name == e.idName)) {
        continue;
      } else {
        if (d.type == CLASS) {
          bool isMatched = false;
          for (auto cls = e.className.begin(); cls != e.className.end();
               ++cls) {
            if ((*cls) == d.name) {
              isMatched = true;
              break;
            }
          }
          if (isMatched)
            continue;
        }
        f = false;
        break;
      }
    }
    if (f) {
      ret.push_back(pair<DOMElement *, DOMElement *>(&e, last));
    } else {
      if (act == INCLUDE || act == SEARCH) {
        for (auto i = e.children.begin(); i != e.children.end(); ++i) {
          q.push(*i);

#ifdef Log
          std::cout << "[push]name: " << (*i)->getTagName() << std::endl;
#endif
          last = elm;
          elm = &e;
        }
      } else if (act == CLOSE) {
        break;
      }
    }
  }
  return ret;
}