#include "Tree.hpp"
#include <iostream>
#include <string>

#define COUNT 10

int main(void)
{
  sbd::Tree tree;
  std::array<std::int32_t, COUNT> numbers = {
      53, 30, 14, 3, 7, 20, 27, 40, 35, 45};
  // for (auto i = 0; i < COUNT; ++i)
  // {
  //   numbers[i] = i;
  // }

  for (auto i = COUNT - 1; i > 0; --i)
  {
    tree.insert(numbers[i], numbers[i]);
  }
  tree.insert(numbers[0], numbers[0]);

  tree.createDotFile("tree.dot");
  system("dot -Tpng tree.dot > wynik.ps; okular wynik.ps");

  sbd::Tree tree2;
  for (auto i = 0; i < COUNT - 1; ++i)
  {
    tree2.insert(numbers[i], numbers[i]);
  }
  tree2.insert(numbers[COUNT - 1], numbers[COUNT - 1]);
  tree2.createDotFile("tree2.dot");
  system("dot -Tpng tree2.dot > wynik2.ps; okular wynik2.ps");

  return 0;
}