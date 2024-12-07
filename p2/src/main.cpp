#include "Tree.hpp"
#include <iostream>
#include <string>

int main(void)
{
  sbd::Tree tree;
  for (auto i = 1; i < 48; ++i)
  {
    tree.insert(i, i);
  }
  tree.createDotFile("tree.dot");
  system("dot -Tpng tree.dot > wynik.ps; okular wynik.ps");

  return 0;
}