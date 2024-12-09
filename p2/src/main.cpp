#include "Tree.hpp"
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <array>

#define COUNT 1000

int main(void)
{
  sbd::Tree tree;
  std::array<float, COUNT> numbers;
  std::random_device rd;
  std::mt19937 gen(rd());
  gen.seed(1);
  std::uniform_real_distribution<> dis(1.0, 100.0);

  for (auto i = 0; i < COUNT; ++i)
  {
    numbers[i] = dis(gen);
    // numbers[i] = i;
  }

  // for (auto i = COUNT - 1; i > 0; --i)
  // {
  //   tree.insert(numbers[i], i);
  // }

  // tree.createDotFile("tree.dot");
  // system("dot -Tpng tree.dot > wynik.ps; okular wynik.ps");

  // tree.insert(numbers[0], 0);

  // tree.createDotFile("tree.dot");
  // system("dot -Tpng tree.dot > wynik.ps; okular wynik.ps");

  // sbd::Tree tree2;
  // for (auto i = 0; i < COUNT - 1; ++i)
  // {
  //   tree2.insert(numbers[i], numbers[i]);
  // }

  // tree2.createDotFile("tree2.dot");
  // system("dot -Tpng tree2.dot > wynik2.ps; okular wynik2.ps");

  // tree2.insert(numbers[COUNT - 1], numbers[COUNT - 1]);
  // tree2.createDotFile("tree2.dot");
  // system("dot -Tpng tree2.dot > wynik2.ps; okular wynik2.ps");

  // Random shuffle array
  std::mt19937 g(rd());
  g.seed(193064);
  std::shuffle(numbers.begin(), numbers.end(), g);

  sbd::Tree tree3;
  for (auto i = 0; i < COUNT - 1; ++i)
  {
    tree3.insert(numbers[i], numbers[i]);
  }
  tree3.insert(numbers[COUNT - 1], numbers[COUNT - 1]);
  tree3.createDotFile("tree3.dot");
  system("dot -Tpng tree3.dot > wynik3.ps; okular wynik3.ps");

  return 0;
}