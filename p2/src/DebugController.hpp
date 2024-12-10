#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include "Counter.hpp"
#include "Tree.hpp"
#include "constants.hpp"

namespace sbd
{
  class DebugController
  {
  private:
    bool printAfterEachAction = true;
    Tree *tree;

  public:
    DebugController() = default;
    ~DebugController() = default;
    DebugController(const DebugController &) = delete;
    DebugController &operator=(const DebugController &) = delete;

    static DebugController &getInstance()
    {
      static DebugController instance;
      return instance;
    }

    void setTree(Tree *tree)
    {
      this->tree = tree;
    }

    void setPrintAfterEachAction(bool printAfterEachAction)
    {
      this->printAfterEachAction = printAfterEachAction;
    }

    bool getPrintAfterEachAction() const
    {
      return printAfterEachAction;
    }

    void insert()
    {
      float key;
      std::int32_t address;
      std::cout << "Enter key: ";
      std::cin >> key;
      std::cout << "Enter address: ";
      std::cin >> address;
      insert(key, address);
    }

    void insert(float key, std::int32_t address)
    {
      tree->insert(key, address);
      breakpoint();
    }

    void insertRandom()
    {
      std::int32_t randomCount;
      std::cout << "Enter count: ";
      std::cin >> randomCount;
      insertRandom(randomCount);
    }

    void insertRandom(std::int32_t randomCount)
    {
      std::random_device rd;
      std::mt19937 gen(rd());
      gen.seed(193064);
      std::uniform_real_distribution<> dis(1.0, 100.0);
      for (auto i = 0; i < randomCount; ++i)
      {
        tree->insert(floorf(dis(gen) * 10) / 10, i);
      }
      breakpoint();
    }

    void search()
    {
      float key;
      std::cout << "Enter key: ";
      std::cin >> key;
      search(key);
    }

    void search(float key)
    {
      std::int32_t result = tree->search(key);
      std::cout << "Result: " << result << std::endl;
    }

    void printCounters()
    {
      std::cout << "Reads: " << sbd::Counters::getInstance().getReadCounter() << std::endl;
      std::cout << "Writes: " << sbd::Counters::getInstance().getWriteCounter() << std::endl;
    }

    void executeInstructionsFromFile()
    {
      std::string filename;
      std::cout << "Enter filename: ";
      std::cin >> filename;

      std::ifstream file(filename);
      if (!file.is_open())
      {
        std::cout << "File not found" << std::endl;
        return;
      }

      std::string line;
      while (std::getline(file, line))
      {
        std::istringstream iss(line);
        std::string instruction;
        iss >> instruction;
        if (instruction == "i")
        {
          float key;
          std::int32_t address;
          iss >> key >> address;
          insert(key, address);
        }
        else if (instruction == "s")
        {
          float key;
          iss >> key;
          search(key);
        }
        else if (instruction == "r")
        {
          std::int32_t count;
          iss >> count;
          insertRandom(count);
        }
        else if (instruction == "p")
        {
          tree->createDotFile(TREE_DOT_FILE_NAME);
          std::string command = "dot -Tpng " + std::string(TREE_DOT_FILE_NAME) + " > wynik.ps; okular wynik.ps";
          system(command.c_str());
        }
        else if (instruction == "d")
        {
          printAfterEachAction = !printAfterEachAction;
          std::cout << "Print after each action: " << (printAfterEachAction ? "true" : "false") << std::endl;
        }
        else if (instruction == "c")
        {
          printCounters();
        }
        else if (instruction == "q")
        {
          break;
        }
      }
    }

    void menu()
    {
      std::cout << "i - insert" << std::endl;
      std::cout << "r - insert random" << std::endl;
      std::cout << "s - search" << std::endl;
      std::cout << "p - print tree" << std::endl;
      std::cout << "d - toggle print after each action" << std::endl;
      std::cout << "c - print counters" << std::endl;
      std::cout << "f - execute instructions from file" << std::endl;
      std::cout << "q - quit" << std::endl;

      char option;
      while (true)
      {
        std::cout << "Enter option: ";
        std::cin >> option;
        switch (option)
        {
        case 'i':
          insert();
          break;
        case 'r':
          insertRandom();
          break;
        case 's':
          search();
          break;
        case 'p':
        {
          tree->createDotFile(TREE_DOT_FILE_NAME);
          std::string command = "dot -Tpng " + std::string(TREE_DOT_FILE_NAME) + " > wynik.ps; okular wynik.ps";
          system(command.c_str());
        }
        break;
        case 'd':
          printAfterEachAction = !printAfterEachAction;
          std::cout << "Print after each action: " << (printAfterEachAction ? "true" : "false") << std::endl;
          break;
        case 'c':
          printCounters();
          break;
        case 'f':
          executeInstructionsFromFile();
          break;
        case 'q':
          return;
        default:
          std::cout << "Invalid option" << std::endl;
          break;
        }
      }
    }

    void breakpoint()
    {
      if (printAfterEachAction)
      {
        sbd::Counters::getInstance().disable();
        tree->createDotFile(TREE_DOT_FILE_NAME);
        std::string command = "dot -Tpng " + std::string(TREE_DOT_FILE_NAME) + " > wynik.ps; okular wynik.ps";
        system(command.c_str());
        sbd::Counters::getInstance().enable();
      }
    }
  };
}