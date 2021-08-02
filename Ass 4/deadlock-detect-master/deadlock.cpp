/**********************************************
 * Last Name:   Yang
 * First Name:  Jack
 * Student ID:  30062393
 * Course:      CPSC 457
 * Tutorial:    T05
 * Assignment:  4
 *
 * File name: deadlock.cpp
 *********************************************/
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_map>
#include <set>
#include <list>
#include <queue>
#include <iostream>

typedef std::vector<std::string> vs_t;

enum Edge
{
  Allocation,
  Request
};

// split string p_line into vector of strings (words)
// the delimiters are 1 or more whitespaces
vs_t split(const std::string &p_line)
{
  auto line = p_line + " ";
  vs_t res;
  bool in_str = false;
  std::string curr_word = "";
  for (auto c : line)
  {
    if (isspace(c))
    {
      if (in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else
    {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// read in a line from stdin and parse it
// parsed results are passed back via parameter references
// return true on success, false on EOF
// aborts program on bad input (should throw exception)
bool parseLine(std::string &proc_name, std::string &res_name, Edge &etype)
{
  char buff[4096];
  vs_t words;

  // read in a line and parse it into words
  while (1)
  { // loop to skip empty lines
    if (!fgets(buff, sizeof(buff), stdin))
      return false;
    words = split(buff);
    if (words.size() != 0)
      break;
  }

  if (words.size() != 3)
  {
    printf("Bad line: %s\n", buff);
    exit(-1); // exception would be nicer
  }

  proc_name = words[0];
  res_name = words[2];
  etype = Edge::Request;

  if (words[1] == "->")
  {
    return true;
  }
  if (words[1] == "<-")
  {
    etype = Edge::Allocation;
    return true;
  }

  printf("Bad line: %s\n", buff);
  exit(-1); // exception would be nicer
};

// utility class you can use to convert words to unique integers
// get(word) returns the same number given the same word
//           the numbers will start at 0, then 1, 2, ...
//
// example:
//   Word2Int w2i
//   w2i.get("hello") = 0
//   w2i.get("world") = 1
//   w2i.get("hello") = 0
//
// HINT: this "may" help you get a bit more performance
// in your cycle finding algorithm, since indexed arrays are faster
// than hash tables... if you don't know what that means,
// safely ignore this hint :)
struct Word2Int
{
  int get(const std::string &w)
  {
    auto f = myset.find(w);
    if (f == myset.end())
    {
      myset[w] = counter;
      counter++;
      return counter - 1;
    }
    return f->second;
  }

  // int to word
  std::string see(const int &w)
  {
    for (const auto &p : myset)
    {
      if (p.second == w)
        return p.first;
    }
    return NULL;
  }

private:
  int counter = 0;
  std::unordered_map<std::string, int> myset;
};

struct Graph
{

  // add your own members here
  // ...

  std::vector<std::vector<int>> adj_list;
  std::unordered_map<int, int> out_counts;
  // copy of out_counts
  std::unordered_map<int, int> tmp_out_counts;
  std::set<std::string> all_nodes;

  int visted_count;
  // queue to keep track of nodes with out degrees of 0
  std::queue<int> tmp_queue;
  // initialize 2 temp strings to keep track of current edge
  std::string tmp_node1, tmp_node2;
  Word2Int w2i;

  Graph()
  {
    // reimplement this (probably)
  }

  // add edge n1 -> n2
  void add(std::string n1, std::string n2)
  {
    all_nodes.insert(n1);
    all_nodes.insert(n2);
    int nodeint2 = w2i.get(n2);
    int nodeint1 = w2i.get(n1);
    if (nodeint2 < (int)adj_list.size())
      adj_list[nodeint2].push_back(nodeint1);
    else
      adj_list.push_back({nodeint1});
    out_counts[nodeint1]++;
    tmp_node1 = n1;
    tmp_node2 = n2;
  }

  // run cycle detection
  bool deadlock()
  {
    tmp_out_counts = out_counts;
    visted_count = 0;
    // clear the queue
    tmp_queue = std::queue<int>();

    /*
    printf("------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < (int)adj_list.size(); i++)
    {
      std::cout << "{" << i << ": ";
      for (int b = 0; b < (int)adj_list[i].size(); b++)
      {
        std::cout << adj_list[i].at(b) << ", ";
      }
      std::cout << "}\n";
    }
    */

    // add all nodes with 0 out degrees into queue
    for (const std::string &n : all_nodes)
    {
      if (tmp_out_counts[w2i.get(n)] == 0)
        tmp_queue.push(w2i.get(n));
    }

    // keep doing nodes with outdegree 0
    while (!tmp_queue.empty())
    {
      // pick a node with outdegree of 0 from queue
      int n;
      n = tmp_queue.front();
      tmp_queue.pop();

      visted_count++;
      // decrement out degree of all neighbouring incoming nodes
      if (n < (int)adj_list.size())
        for (const auto &s : adj_list[n])
        {
          if (--tmp_out_counts[s] == 0)
            tmp_queue.push(s);
        }
      else
        adj_list.push_back({});

      if (tmp_queue.empty() && visted_count != (int)all_nodes.size())
        return true;
    }

    if (visted_count != (int)all_nodes.size())
      return true;

    return false;
  }

  // report cycle
  void report()
  {
    if (tmp_queue.empty() && visted_count != (int)all_nodes.size())
    {
      std::string str_node1 = tmp_node1.erase(0, 2);
      std::string str_node2 = tmp_node2.erase(0, 2);
      printf("Deadlock on edge: %s -> %s", str_node1.c_str(), str_node2.c_str());
      printf("\nDeadlocked processes: ");
      for (const auto &p : tmp_out_counts)
      {
        if (p.second != 0)
        {
          std::string loopnode = w2i.see(p.first);
          if (loopnode.at(0) == 'p')
            std::cout << loopnode.erase(0, 2) << ", ";
        }
      }

      printf("\n");
    }
    else
    {
      printf("No Deadlock\n");
    }
  }
};

int main()
{

  Graph g;

  while (true)
  {
    std::string pname, rname;
    Edge e;
    if (!parseLine(pname, rname, e))
      break;

    // make adjacency list
    if (e == Edge::Request)
      g.add("p_" + pname, "r_" + rname);
    else
      g.add("r_" + rname, "p_" + pname);

    // run topological sort
    if (g.deadlock())
      break;
  }

  g.report();

  return 0;
}
