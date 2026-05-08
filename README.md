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
