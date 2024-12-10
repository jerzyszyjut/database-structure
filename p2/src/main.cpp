#include "Tree.hpp"
#include "DebugController.hpp"
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include <array>

#define COUNT 50

int main(void)
{
  sbd::Tree tree;
  sbd::DebugController &debugController = sbd::DebugController::getInstance();
  debugController.setTree(&tree);
  debugController.menu();

  return 0;
}