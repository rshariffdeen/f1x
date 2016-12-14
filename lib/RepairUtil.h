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

#include <memory>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include "Config.h"


void addClangHeadersToCompileDB(boost::filesystem::path projectRoot);


enum class Kind {
  OPERATOR, VARIABLE, CONSTANT, PARAMETER
};


enum class Type {
  BOOLEAN, INTEGER, POINTER, BITVECTOR
};


enum class Operator {
  EQ, NEQ, LT, LE, GT, GE, OR, AND, ADD, SUB, MUL, DIV, MOD, NEG, NOT,
  BV_AND, BV_XOR, BV_OR, BV_SHL, BV_SHR, BV_NOT,
  BV_TO_INT, INT_TO_BV // auxiliary operators
};

Type operatorType(const Operator &op);

Operator binaryOperatorByString(const std::string &repr);

Operator unaryOperatorByString(const std::string &repr);

std::string operatorToString(const Operator &op);


struct Expression {
  Kind kind;
  
  Type type;

  /*
    only if kind is OPERATOR
   */
  Operator op;

  /* 
     Either 
       char, unsinged char, unsigned short, unsigned int, unsigned long, signed char, short, int, long
     or
       the base type of pointer
   */
  std::string rawType;

  /*
    1, 2,... for constants; "x", "y",... for variables; ">=",... for ops
  */
  std::string repr;

  std::vector<Expression> args;
};

std::string expressionToString(const Expression &expression);


enum class DefectClass {
  CONDITION,  // existing program conditions in if, for, while, etc.
  EXPRESSION, // right side of assignments, call arguments
  GUARD       // adding guard for existing statement
};


struct Location {
  uint fileId;
  uint locId;  
  uint beginLine;
  uint beginColumn;
  uint endLine;
  uint endColumn;
};


struct CandidateLocation {
  DefectClass defect;
  Location location;
  Expression original;
  std::vector<Expression> components;
};


enum class Transformation {
  NONE,
  ALTERNATIVE,    // alternative operator e.g. > --> >=
  SWAPING,        // swaping arguments
  GENERALIZATION, // e.g. 1 --> x
  CONCRETIZATION, // e.g. x --> 1
  SUBSTITUTION,   // (generic) substution of subnode
  WIDENING,       // adding "|| something"
  NARROWING       // adding "&& something"
};


struct PatchMeta {
  Transformation transformation;
  uint distance;
};


struct SearchSpaceElement {
  std::shared_ptr<CandidateLocation> buggy;
  uint id;
  Expression patch;
  PatchMeta meta;
};


class FromDirectory {
 public:
  FromDirectory(const boost::filesystem::path &path);
  ~FromDirectory();

 private:
  boost::filesystem::path original;
};


class parse_error : public std::logic_error {
 public:
  using std::logic_error::logic_error;
};


std::vector<std::shared_ptr<CandidateLocation>> loadCandidateLocations(const boost::filesystem::path &path);


class TestingFramework {
 public:
  TestingFramework(const boost::filesystem::path &root,
                   const boost::filesystem::path &driver);
  
  bool isPassing(const std::string &testId);

 private:
  boost::filesystem::path root;
  boost::filesystem::path driver;
};


void backupSource(const boost::filesystem::path &workDir,
                  const boost::filesystem::path &root,
                  const std::vector<boost::filesystem::path> &files);


void restoreSource(const boost::filesystem::path &workDir,
                   const boost::filesystem::path &root,
                   const std::vector<boost::filesystem::path> &files);


void computeDiff(const boost::filesystem::path &root,
                 const boost::filesystem::path &file,
                 const uint id,
                 const boost::filesystem::path &output);
