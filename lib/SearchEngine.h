/*
  This file is part of f1x.
  Copyright (C) 2016  Sergey Mechtaev, Gao Xiang, Abhik Roychoudhury

  f1x is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <vector>
#include "RepairUtil.h"
#include "Project.h"
#include "Runtime.h"


struct SearchStatistics {
  uint explorationCounter;
  uint executionCounter;
};


class SearchEngine {
 public:
  SearchEngine(const std::vector<std::string> &tests,
               TestingFramework &tester,
               Runtime &runtime,
               const Config &cfg,
               std::shared_ptr<std::unordered_map<uint, std::unordered_set<F1XID>>> groupable,
               std::unordered_map<Location, std::vector<int>> relatedTestIndexes);

  uint findNext(const std::vector<SearchSpaceElement> &searchSpace, uint indexFrom);
  SearchStatistics getStatistics();

 private:
 
  void changeSensitivity(std::vector<int> &testOrder, int index);
  std::vector<std::string> tests;
  TestingFramework tester;
  Runtime runtime;
  Config cfg;
  SearchStatistics stat;
  uint progress;
  std::shared_ptr<std::unordered_map<uint, std::unordered_set<F1XID>>> groupable;
  std::unordered_set<F1XID> failing;
  std::unordered_map<std::string, std::unordered_set<F1XID>> passing;
  std::unordered_map<Location, std::vector<int>> relatedTestIndexes;
};
