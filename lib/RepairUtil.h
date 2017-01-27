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

#include <memory>
#include <stdexcept>
#include <map>

#include <boost/filesystem.hpp>

#include "F1XConfig.h"


struct F1XID {
  uint base;
  uint int2;
  uint bool2;
  uint cond3;
  uint param;

  bool operator==(const F1XID &other) const { 
    return (base == other.base
            && int2 == other.int2
            && bool2 == other.bool2
            && cond3 == other.cond3
            && param == other.param);
  }
};

// http://stackoverflow.com/questions/19195183/how-to-properly-hash-the-custom-struct
template <class T>
inline void hash_combine(std::size_t & s, const T & v)
{
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

namespace std {
  template<>
    struct hash<F1XID> {
    inline size_t operator()(const F1XID& id) const {
      size_t value = 0;
      hash_combine(value, id.base);
      hash_combine(value, id.int2);
      hash_combine(value, id.bool2);
      hash_combine(value, id.cond3);
      hash_combine(value, id.param);
      return value;
    }
  };
}


enum class NodeKind {
  OPERATOR, VARIABLE, CONSTANT, PARAMETER,
  BV2, INT2, BOOL2, BOOL3 // auxiliary kinds
};


enum class Type {
  BOOLEAN, INTEGER, POINTER, BITVECTOR
};


enum class Operator {
  NONE, // this is when node is not an operator
  EQ, NEQ, LT, LE, GT, GE, OR, AND, ADD, SUB, MUL, DIV, MOD, NEG, NOT,
  BV_AND, BV_XOR, BV_OR, BV_SHL, BV_SHR, BV_NOT,
  BV_TO_INT, INT_TO_BV, // auxiliary operators for synthesizer to separate arithmetic and bitwise parts
  INT_CAST // auxiliary operator for INT2 substitutions, because other types are not supported inside runtime
};

Type operatorType(const Operator &op);

Operator binaryOperatorByString(const std::string &repr);

Operator unaryOperatorByString(const std::string &repr);

std::string operatorToString(const Operator &op);


struct Expression {
  NodeKind kind;
  Type type;
  Operator op;   /* should be NONE if not of the kind OPERATOR */
  std::string rawType; /* either innteger type (char, unsinged char, unsigned short, ...) or pointer base type */
  std::string repr; /* 1, 2,... for constants; "x", "y",... for variables; ">=",... for ops */
  std::vector<Expression> args;
};

std::string expressionToString(const Expression &expression);

Expression getIntegerExpression(int n);

Expression getNullPointer();


struct Location {
  uint fileId;
  uint beginLine;
  uint beginColumn;
  uint endLine;
  uint endColumn;

  bool operator==(const Location &other) const { 
    return (fileId == other.fileId
            && beginLine == other.beginLine
            && beginColumn == other.beginColumn
            && endLine == other.endLine
            && endColumn == other.endColumn);
  }
};


namespace std {
  template<>
    struct hash<Location> {
    inline size_t operator()(const Location& id) const {
      size_t value = 0;
      hash_combine(value, id.fileId);
      hash_combine(value, id.beginLine);
      hash_combine(value, id.beginColumn);
      hash_combine(value, id.endLine);
      hash_combine(value, id.endColumn);
      return value;
    }
  };
}

/*
  We use the following structure to represent patching:
  - TransformationSchema is a high-level transformation rule
  - ModificationKind is a kind of expression modification
  - SchemaApplication is an application of a schema to a program location
  - SearchSpaceElement is a concrete patch
 */


enum class TransformationSchema {
  EXPRESSION,  // changing existing expression
  IF_GUARD,    // adding guard for existing statement
  ARRAY_INIT   // adding memset for array
};


enum class ModificationKind {
  OPERATOR,       // operator replacement e.g. > --> >=
  SWAPING,        // swaping arguments
  SIMPLIFICATION, // simplifying expression
  GENERALIZATION, // e.g. 1 --> x
  CONCRETIZATION, // e.g. x --> 1
  LOOSENING,      // adding "|| something"
  TIGHTENING,     // adding "&& something"
  NEGATION,       // (logically) negate or remove negation
  NULL_CHECK,     // adding null check
  SUBSTITUTION    // (generic) substution of subnode
};


enum struct LocationContext {
  CONDITION, //NOTE: this is for if and loop conditions, etc.
  UNKNOWN
};


struct SchemaApplication {
  uint appId;
  TransformationSchema schema;
  Location location;
  LocationContext context;
  Expression original;
  std::vector<Expression> components;
};


struct PatchMetadata {
  ModificationKind kind;
  uint distance;
};


struct SearchSpaceElement {
  F1XID id;
  std::shared_ptr<SchemaApplication> app;
  Expression modified;
  PatchMetadata meta;
};


std::string visualizeF1XID(const F1XID &id);


std::string visualizeChange(const SearchSpaceElement &el);


std::string visualizeElement(const SearchSpaceElement &el, const boost::filesystem::path &file);


class FromDirectory {
 public:
  FromDirectory(const boost::filesystem::path &path);
  ~FromDirectory();

 private:
  boost::filesystem::path original;
};


class InEnvironment {
 public:
  InEnvironment(const std::map<std::string, std::string> &env);
  ~InEnvironment();

 private:
  std::map<std::string, std::string> original;
};


class parse_error : public std::logic_error {
 public:
  using std::logic_error::logic_error;
};


std::vector<std::shared_ptr<SchemaApplication>> loadSchemaApplications(const boost::filesystem::path &path);


bool isExecutable(const char *file);
