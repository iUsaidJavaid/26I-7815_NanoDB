#!/bin/bash

# NanoDB Project Setup Script
# This script creates the complete folder structure and empty files for NanoDB

echo "Setting up NanoDB project structure..."

# Create directories
echo "Creating directories..."
mkdir -p include/common
mkdir -p include/storage
mkdir -p include/catalog
mkdir -p include/parser
mkdir -p include/index
mkdir -p include/optimizer
mkdir -p include/engine
mkdir -p src/common
mkdir -p src/storage
mkdir -p src/catalog
mkdir -p src/parser
mkdir -p src/index
mkdir -p src/optimizer
mkdir -p src/engine
mkdir -p data
mkdir -p logs
mkdir -p tests

# Create header files
echo "Creating header files..."

# include/common/Types.h
cat > include/common/Types.h << 'EOF'
#ifndef NANODB_TYPES_H
#define NANODB_TYPES_H

namespace NanoDB {

enum class ValueType {
    INTEGER,
    FLOAT,
    STRING,
    NULL_VALUE
};

class Value {
public:
    Value();
    ~Value();
    
    ValueType getType() const;
    void setType(ValueType type);
    
    int getInt() const;
    void setInt(int value);
    
    float getFloat() const;
    void setFloat(float value);
    
    const char* getString() const;
    void setString(const char* str);
    
    bool isNull() const;
    void setNull();

private:
    ValueType type_;
    union {
        int int_value_;
        float float_value_;
    };
    char* string_value_;
};

} // namespace NanoDB

#endif // NANODB_TYPES_H
EOF

# include/storage/Page.h
cat > include/storage/Page.h << 'EOF'
#ifndef NANODB_PAGE_H
#define NANODB_PAGE_H

namespace NanoDB {

class Page {
public:
    static const int PAGE_SIZE = 4096;
    
    Page();
    ~Page();
    
    char* getData();
    const char* getData() const;
    
    int getPageNumber() const;
    void setPageNumber(int pageNum);
    
    bool isDirty() const;
    void setDirty(bool dirty);
    
    void clear();
    
private:
    char data_[PAGE_SIZE];
    int page_number_;
    bool dirty_;
};

} // namespace NanoDB

#endif // NANODB_PAGE_H
EOF

# include/storage/Pager.h
cat > include/storage/Pager.h << 'EOF'
#ifndef NANODB_PAGER_H
#define NANODB_PAGER_H

#include "storage/Page.h"

namespace NanoDB {

class Pager {
public:
    Pager(const char* filename);
    ~Pager();
    
    bool open();
    void close();
    
    Page* getPage(int page_num);
    void flushPage(int page_num);
    
    int getNumPages() const;
    
private:
    char* filename_;
    int file_descriptor_;
    int num_pages_;
};

} // namespace NanoDB

#endif // NANODB_PAGER_H
EOF

# include/storage/LRUCache.h
cat > include/storage/LRUCache.h << 'EOF'
#ifndef NANODB_LRU_CACHE_H
#define NANODB_LRU_CACHE_H

#include "storage/Page.h"

namespace NanoDB {

class LRUCache {
public:
    struct CacheNode {
        int key;
        Page* value;
        CacheNode* prev;
        CacheNode* next;
    };
    
    LRUCache(int capacity);
    ~LRUCache();
    
    Page* get(int key);
    void put(int key, Page* value);
    
    void clear();
    
private:
    int capacity_;
    int size_;
    CacheNode* head_;
    CacheNode* tail_;
    CacheNode** hash_table_;
    int hash_table_size_;
    
    void moveToHead(CacheNode* node);
    void removeNode(CacheNode* node);
    CacheNode* addNodeToHead(int key, Page* value);
    void removeTail();
};

} // namespace NanoDB

#endif // NANODB_LRU_CACHE_H
EOF

# include/catalog/SystemCatalog.h
cat > include/catalog/SystemCatalog.h << 'EOF'
#ifndef NANODB_SYSTEM_CATALOG_H
#define NANODB_SYSTEM_CATALOG_H

namespace NanoDB {

class SystemCatalog {
public:
    struct TableInfo {
        int table_id;
        char name[64];
        int num_columns;
        int root_page;
    };
    
    struct ColumnInfo {
        int column_id;
        char name[64];
        int type;
        int table_id;
    };
    
    SystemCatalog();
    ~SystemCatalog();
    
    bool createTable(const char* name, int num_columns);
    bool dropTable(const char* name);
    
    TableInfo* getTable(const char* name);
    TableInfo* getTableById(int table_id);
    
    bool addColumn(int table_id, const char* name, int type);
    ColumnInfo* getColumn(int table_id, const char* name);
    
    void load();
    void save();
    
private:
    TableInfo* tables_;
    ColumnInfo* columns_;
    int num_tables_;
    int num_columns_;
    int max_tables_;
    int max_columns_;
};

} // namespace NanoDB

#endif // NANODB_SYSTEM_CATALOG_H
EOF

# include/catalog/HashMap.h
cat > include/catalog/HashMap.h << 'EOF'
#ifndef NANODB_HASH_MAP_H
#define NANODB_HASH_MAP_H

namespace NanoDB {

class HashMap {
public:
    struct KeyValuePair {
        int key;
        int value;
        KeyValuePair* next;
    };
    
    HashMap(int capacity);
    ~HashMap();
    
    bool put(int key, int value);
    bool get(int key, int* value);
    bool remove(int key);
    bool contains(int key);
    
    void clear();
    int size() const;
    
private:
    KeyValuePair** buckets_;
    int capacity_;
    int size_;
    
    int hash(int key) const;
};

} // namespace NanoDB

#endif // NANODB_HASH_MAP_H
EOF

# include/parser/Tokenizer.h
cat > include/parser/Tokenizer.h << 'EOF'
#ifndef NANODB_TOKENIZER_H
#define NANODB_TOKENIZER_H

namespace NanoDB {

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    PUNCTUATION,
    EOF_TOKEN,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    char value[64];
    int line;
    int column;
};

class Tokenizer {
public:
    Tokenizer(const char* input);
    ~Tokenizer();
    
    Token getNextToken();
    void reset();
    
private:
    const char* input_;
    int position_;
    int line_;
    int column_;
    
    bool isWhitespace(char c);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
};

} // namespace NanoDB

#endif // NANODB_TOKENIZER_H
EOF

# include/parser/ShuntingYard.h
cat > include/parser/ShuntingYard.h << 'EOF'
#ifndef NANODB_SHUNTING_YARD_H
#define NANODB_SHUNTING_YARD_H

#include "parser/Tokenizer.h"

namespace NanoDB {

class ShuntingYard {
public:
    struct StackNode {
        Token token;
        StackNode* next;
    };
    
    ShuntingYard();
    ~ShuntingYard();
    
    Token* parse(Token* tokens, int num_tokens);
    
private:
    StackNode* operator_stack_;
    Token* output_queue_;
    int output_size_;
    int output_capacity_;
    
    int getPrecedence(const Token& token);
    bool isLeftAssociative(const Token& token);
    void pushOperator(const Token& token);
    Token popOperator();
    void pushOutput(const Token& token);
};

} // namespace NanoDB

#endif // NANODB_SHUNTING_YARD_H
EOF

# include/parser/ExpressionEvaluator.h
cat > include/parser/ExpressionEvaluator.h << 'EOF'
#ifndef NANODB_EXPRESSION_EVALUATOR_H
#define NANODB_EXPRESSION_EVALUATOR_H

#include "common/Types.h"
#include "parser/ShuntingYard.h"

namespace NanoDB {

class ExpressionEvaluator {
public:
    ExpressionEvaluator();
    ~ExpressionEvaluator();
    
    Value evaluate(Token* postfix, int num_tokens);
    
private:
    struct EvalStack {
        Value value;
        EvalStack* next;
    };
    
    EvalStack* stack_;
    
    void push(const Value& value);
    Value pop();
    bool isEmpty();
};

} // namespace NanoDB

#endif // NANODB_EXPRESSION_EVALUATOR_H
EOF

# include/index/AVLTree.h
cat > include/index/AVLTree.h << 'EOF'
#ifndef NANODB_AVL_TREE_H
#define NANODB_AVL_TREE_H

namespace NanoDB {

class AVLTree {
public:
    struct AVLNode {
        int key;
        int value;
        AVLNode* left;
        AVLNode* right;
        int height;
    };
    
    AVLTree();
    ~AVLTree();
    
    bool insert(int key, int value);
    bool remove(int key);
    bool search(int key, int* value);
    
    void clear();
    int size() const;
    
private:
    AVLNode* root_;
    int size_;
    
    int height(AVLNode* node);
    int balanceFactor(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* insertNode(AVLNode* node, int key, int value);
    AVLNode* removeNode(AVLNode* node, int key);
    AVLNode* findMin(AVLNode* node);
    AVLNode* balance(AVLNode* node);
    void destroy(AVLNode* node);
};

} // namespace NanoDB

#endif // NANODB_AVL_TREE_H
EOF

# include/optimizer/Graph.h
cat > include/optimizer/Graph.h << 'EOF'
#ifndef NANODB_GRAPH_H
#define NANODB_GRAPH_H

namespace NanoDB {

class Graph {
public:
    struct Edge {
        int from;
        int to;
        int weight;
        Edge* next;
    };
    
    struct Vertex {
        int id;
        Edge* edges;
        Vertex* next;
    };
    
    Graph(int max_vertices);
    ~Graph();
    
    bool addVertex(int id);
    bool addEdge(int from, int to, int weight);
    bool removeEdge(int from, int to);
    bool hasEdge(int from, int to);
    
    int getDegree(int vertex);
    Edge* getEdges(int vertex);
    
    void clear();
    
private:
    Vertex* vertices_;
    int max_vertices_;
    int num_vertices_;
    
    Vertex* findVertex(int id);
};

} // namespace NanoDB

#endif // NANODB_GRAPH_H
EOF

# include/optimizer/MST.h
cat > include/optimizer/MST.h << 'EOF'
#ifndef NANODB_MST_H
#define NANODB_MST_H

#include "optimizer/Graph.h"

namespace NanoDB {

class MST {
public:
    struct MSTEdge {
        int from;
        int to;
        int weight;
    };
    
    MST();
    ~MST();
    
    MSTEdge* computeMST(Graph* graph, int* num_edges);
    
private:
    struct DisjointSet {
        int parent;
        int rank;
    };
    
    DisjointSet* sets_;
    int capacity_;
    
    void makeSet(int x);
    int find(int x);
    void unionSets(int x, int y);
};

} // namespace NanoDB

#endif // NANODB_MST_H
EOF

# include/engine/QueryExecutor.h
cat > include/engine/QueryExecutor.h << 'EOF'
#ifndef NANODB_QUERY_EXECUTOR_H
#define NANODB_QUERY_EXECUTOR_H

#include "common/Types.h"

namespace NanoDB {

class QueryExecutor {
public:
    QueryExecutor();
    ~QueryExecutor();
    
    bool execute(const char* query);
    Value* getResult(int* num_rows);
    
    void reset();
    
private:
    Value* result_;
    int num_rows_;
    int num_columns_;
};

} // namespace NanoDB

#endif // NANODB_QUERY_EXECUTOR_H
EOF

# include/engine/PriorityQueue.h
cat > include/engine/PriorityQueue.h << 'EOF'
#ifndef NANODB_PRIORITY_QUEUE_H
#define NANODB_PRIORITY_QUEUE_H

namespace NanoDB {

class PriorityQueue {
public:
    struct QueueNode {
        int priority;
        int value;
    };
    
    PriorityQueue(int capacity);
    ~PriorityQueue();
    
    bool enqueue(int priority, int value);
    bool dequeue(int* value);
    bool peek(int* value);
    
    bool isEmpty();
    int size();
    
    void clear();
    
private:
    QueueNode* heap_;
    int capacity_;
    int size_;
    
    void heapifyUp(int index);
    void heapifyDown(int index);
    int parent(int index);
    int leftChild(int index);
    int rightChild(int index);
    void swap(int i, int j);
};

} // namespace NanoDB

#endif // NANODB_PRIORITY_QUEUE_H
EOF

# Create source files
echo "Creating source files..."

# src/common/Types.cpp
cat > src/common/Types.cpp << 'EOF'
#include "common/Types.h"

namespace NanoDB {

Value::Value() : type_(ValueType::NULL_VALUE), string_value_(nullptr) {
}

Value::~Value() {
    if (string_value_ != nullptr) {
        delete[] string_value_;
    }
}

ValueType Value::getType() const {
    return type_;
}

void Value::setType(ValueType type) {
    type_ = type;
}

int Value::getInt() const {
    return int_value_;
}

void Value::setInt(int value) {
    type_ = ValueType::INTEGER;
    int_value_ = value;
}

float Value::getFloat() const {
    return float_value_;
}

void Value::setFloat(float value) {
    type_ = ValueType::FLOAT;
    float_value_ = value;
}

const char* Value::getString() const {
    return string_value_;
}

void Value::setString(const char* str) {
    type_ = ValueType::STRING;
    if (string_value_ != nullptr) {
        delete[] string_value_;
    }
    int len = 0;
    while (str[len] != '\0') {
        ++len;
    }
    string_value_ = new char[len + 1];
    for (int i = 0; i < len; ++i) {
        string_value_[i] = str[i];
    }
    string_value_[len] = '\0';
}

bool Value::isNull() const {
    return type_ == ValueType::NULL_VALUE;
}

void Value::setNull() {
    type_ = ValueType::NULL_VALUE;
}

} // namespace NanoDB
EOF

# Create empty .cpp files for other modules
for module in storage/Page storage/Pager storage/LRUCache catalog/SystemCatalog catalog/HashMap parser/Tokenizer parser/ShuntingYard parser/ExpressionEvaluator index/AVLTree optimizer/Graph optimizer/MST engine/QueryExecutor engine/PriorityQueue; do
    cat > src/${module}.cpp << 'EOF'
// Implementation file - to be filled with logic
EOF
done

# src/main.cpp
cat > src/main.cpp << 'EOF'
#include <iostream>
#include "common/Types.h"
#include "storage/Page.h"
#include "storage/Pager.h"
#include "storage/LRUCache.h"
#include "catalog/SystemCatalog.h"
#include "catalog/HashMap.h"
#include "parser/Tokenizer.h"
#include "parser/ShuntingYard.h"
#include "parser/ExpressionEvaluator.h"
#include "index/AVLTree.h"
#include "optimizer/Graph.h"
#include "optimizer/MST.h"
#include "engine/QueryExecutor.h"
#include "engine/PriorityQueue.h"

using namespace NanoDB;

int main() {
    std::cout << "NanoDB - Mini Database Engine" << std::endl;
    std::cout << "Project skeleton created successfully." << std::endl;
    
    return 0;
}
EOF

# tests/test_runner.cpp
cat > tests/test_runner.cpp << 'EOF'
#include <iostream>
#include "common/Types.h"
#include "storage/Page.h"
#include "storage/Pager.h"
#include "storage/LRUCache.h"
#include "catalog/SystemCatalog.h"
#include "catalog/HashMap.h"
#include "parser/Tokenizer.h"
#include "parser/ShuntingYard.h"
#include "parser/ExpressionEvaluator.h"
#include "index/AVLTree.h"
#include "optimizer/Graph.h"
#include "optimizer/MST.h"
#include "engine/QueryExecutor.h"
#include "engine/PriorityQueue.h"

using namespace NanoDB;

void runTests() {
    std::cout << "Running NanoDB Tests..." << std::endl;
    std::cout << "Test skeleton - no tests implemented yet." << std::endl;
}

int main() {
    runTests();
    return 0;
}
EOF

# Create CMakeLists.txt
echo "Creating CMakeLists.txt..."
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(NanoDB VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/common/Types.cpp
    src/storage/Page.cpp
    src/storage/Pager.cpp
    src/storage/LRUCache.cpp
    src/catalog/SystemCatalog.cpp
    src/catalog/HashMap.cpp
    src/parser/Tokenizer.cpp
    src/parser/ShuntingYard.cpp
    src/parser/ExpressionEvaluator.cpp
    src/index/AVLTree.cpp
    src/optimizer/Graph.cpp
    src/optimizer/MST.cpp
    src/engine/QueryExecutor.cpp
    src/engine/PriorityQueue.cpp
    src/main.cpp
)

# Create executable
add_executable(nanodb ${SOURCES})

# Test executable
set(TEST_SOURCES
    src/common/Types.cpp
    src/storage/Page.cpp
    src/storage/Pager.cpp
    src/storage/LRUCache.cpp
    src/catalog/SystemCatalog.cpp
    src/catalog/HashMap.cpp
    src/parser/Tokenizer.cpp
    src/parser/ShuntingYard.cpp
    src/parser/ExpressionEvaluator.cpp
    src/index/AVLTree.cpp
    src/optimizer/Graph.cpp
    src/optimizer/MST.cpp
    src/engine/QueryExecutor.cpp
    src/engine/PriorityQueue.cpp
    tests/test_runner.cpp
)

add_executable(test_runner ${TEST_SOURCES})
EOF

# Create .gitignore
echo "Creating .gitignore..."
cat > .gitignore << 'EOF'
# Build directory
build/

# Compiled binaries
*.exe
*.out
*.app

# Object files
*.o
*.obj

# Library files
*.a
*.lib
*.so
*.dll
*.dylib

# Database files
data/*.tbl
data/*.db

# Log files
logs/*.log

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# CMake files
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile
*.cmake
!CMakeLists.txt

# Debug files
*.dSYM/
*.su
*.idb
*.pdb
EOF

# Create README.md
echo "Creating README.md..."
cat > README.md << 'EOF'
# NanoDB - Mini Database Engine

A lightweight database engine implemented in C++17 without STL containers.

## Project Structure

```
.
├── include/
│   ├── common/
│   │   └── Types.h              # Custom Value type
│   ├── storage/
│   │   ├── Page.h               # Page management
│   │   ├── Pager.h              # File pager
│   │   └── LRUCache.h           # LRU cache implementation
│   ├── catalog/
│   │   ├── SystemCatalog.h      # System catalog
│   │   └── HashMap.h            # Hash map implementation
│   ├── parser/
│   │   ├── Tokenizer.h          # SQL tokenizer
│   │   ├── ShuntingYard.h       # Expression parsing
│   │   └── ExpressionEvaluator.h # Expression evaluation
│   ├── index/
│   │   └── AVLTree.h            # AVL tree index
│   ├── optimizer/
│   │   ├── Graph.h              # Query graph
│   │   └── MST.h                # Minimum spanning tree
│   └── engine/
│       ├── QueryExecutor.h      # Query execution engine
│       └── PriorityQueue.h      # Priority queue implementation
├── src/
│   ├── common/
│   │   └── Types.cpp
│   ├── storage/
│   │   ├── Page.cpp
│   │   ├── Pager.cpp
│   │   └── LRUCache.cpp
│   ├── catalog/
│   │   ├── SystemCatalog.cpp
│   │   └── HashMap.cpp
│   ├── parser/
│   │   ├── Tokenizer.cpp
│   │   ├── ShuntingYard.cpp
│   │   └── ExpressionEvaluator.cpp
│   ├── index/
│   │   └── AVLTree.cpp
│   ├── optimizer/
│   │   ├── Graph.cpp
│   │   └── MST.cpp
│   ├── engine/
│   │   ├── QueryExecutor.cpp
│   │   └── PriorityQueue.cpp
│   └── main.cpp
├── tests/
│   └── test_runner.cpp
├── data/                        # Database files
├── logs/                        # Log files
├── CMakeLists.txt
├── setup.sh
└── README.md
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./nanodb
```

## Testing

```bash
./test_runner
```

## Design Constraints

- No STL containers (std::vector, std::map, etc.) used
- Custom implementations for all data structures
- C++17 standard
- Page-based storage with 4KB pages
- LRU cache for buffer management
- AVL tree for indexing
- Hash map for catalog management
EOF

# Make setup.sh executable
chmod +x setup.sh

echo "NanoDB project structure created successfully!"
echo "To build the project:"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  make"
