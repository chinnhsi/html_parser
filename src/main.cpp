#pragma once
#include "parser.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <vector>
using std::ifstream;

int main(int, char **) {
  HTMLParser p;
  ifstream ifs("./testcase/test.html", std::ios::in);
  if (!ifs.is_open()) {
    std::cout << "ERROR" << std::endl;
    return 0;
  }
  std::string s(std::istreambuf_iterator<char>{ifs},
                std::istreambuf_iterator<char>{});

  std::cout << "==========================Success" << std::endl;
  p.create(s.c_str());
  std::vector<DOMElement *> d =
      p.documentSelectorAll(".bili-video-card .bili-video-card__info--author");
      // p.documentSelectorAll("div#test.test ul>li~li~li");

  if (d.size() == 0) {
    std::cout << "[result]"
              << "NOT FOUND" << std::endl;
  } else {
    for (auto i = d.begin(); i != d.end(); ++i) {
      std::cout << "[result]" << (*i)->getInnerHTML() << std::endl;
    }
  }
  /*
  expected result:
  [result]熊出没BoonieBears
  [result]全球不知道
  [result]害羞7
  [result]青铜树海
  [result]甜甜怜怜子
  [result]是个维维啊
  [result]不愚人的愚人杰
  [result]不正常动物RC
  [result]冯牡丹无Emoji版
  [result]SaxyBar
  */
  return 0;
}
