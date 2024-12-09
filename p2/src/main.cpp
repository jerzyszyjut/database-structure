#include "Tree.hpp"
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <array>

#define COUNT 10

int main(void)
{
  sbd::Tree tree;
  std::array<std::int32_t, COUNT> numbers;
  // std::random_device rd;
  // std::mt19937 gen(rd());
  // gen.seed(1);
  // std::uniform_int_distribution<> dis(1, 100);

  for (auto i = 0; i < COUNT; ++i)
  {
    // numbers[i] = dis(gen);
    numbers[i] = i;
  }

  for (auto i = COUNT - 1; i > 0; --i)
  {
    tree.insert(numbers[i], numbers[i]);
  }

  tree.insert(numbers[0], numbers[0]);

  tree.createDotFile("tree.dot");
  system("dot -Tpng tree.dot > wynik.ps; okular wynik.ps");

  // sbd::Tree tree2;
  // for (auto i = 0; i < COUNT - 1; ++i)
  // {
  //   tree2.insert(numbers[i], numbers[i]);
  // }
  // tree2.insert(numbers[COUNT - 1], numbers[COUNT - 1]);
  // tree2.createDotFile("tree2.dot");
  // system("dot -Tpng tree2.dot > wynik2.ps; okular wynik2.ps");

  // // Random shuffle array
  // std::random_device rd;
  // std::mt19937 g(rd());
  // g.seed(193064);
  // std::shuffle(numbers.begin(), numbers.end(), g);

  // sbd::Tree tree3;
  // for (auto i = 0; i < COUNT - 1; ++i)
  // {
  //   tree3.insert(numbers[i], numbers[i]);
  // }
  // tree3.insert(numbers[COUNT - 1], numbers[COUNT - 1]);
  // tree3.createDotFile("tree3.dot");
  // system("dot -Tpng tree3.dot > wynik3.ps; okular wynik3.ps");

  return 0;
}