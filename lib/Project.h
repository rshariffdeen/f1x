/*
  This file is part of f1x.
  Copyright (C) 2016  Sergey Mechtaev, Abhik Roychoudhury

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

#include <boost/filesystem.hpp>
#include "Config.h"


class Project {
 public:
  Project(const boost::filesystem::path &root,
          const std::vector<boost::filesystem::path> &files,
          const std::string &buildCmd,          
          const boost::filesystem::path &workDir);

  bool initialBuild();
  bool buildWithRuntime(const boost::filesystem::path &header);
  void backupFiles();
  void restoreFiles();
  void saveFilesWithPrefix(const std::string &prefix);
  void computeDiff(const boost::filesystem::path &file,
                   const boost::filesystem::path &outputFile);
  boost::filesystem::path getRoot() const;
  std::vector<boost::filesystem::path> getFiles() const;

 private:
  boost::filesystem::path root;
  std::vector<boost::filesystem::path> files;
  std::string buildCmd;
  boost::filesystem::path workDir;
};


class TestingFramework {
 public:
  TestingFramework(const Project &project,
                   const boost::filesystem::path &driver,
                   const uint testTimeout);
  
  bool isPassing(const std::string &testId);

 private:
  Project project;
  boost::filesystem::path driver;
  uint testTimeout;
};
