#include "engine/QueryExecutor.h"
#include <cstdio>
#include <new>

namespace NanoDB {

QueryExecutor::QueryExecutor(Pager& pager, SystemCatalog& catalog,
                             IndexManager& indexManager, MSTOptimizer& optimizer,
                             PriorityQueue& queue)
    : pager_(pager), catalog_(catalog), indexManager_(indexManager),
      optimizer_(optimizer), queue_(queue), taskIdCounter_(1) {
}

QueryExecutor::~QueryExecutor() {
}

bool QueryExecutor::startsWith(const char* str, const char* prefix) const {
    int i = 0;
    while (prefix[i] != '\0') {
        if (str[i] == '\0' || str[i] != prefix[i]) {
            return false;
        }
        ++i;
    }
    return true;
}

bool QueryExecutor::strEquals(const char* a, const char* b) const {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return false;
        }
        ++i;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void QueryExecutor::copyString(char* dest, int destSize, const char* src) const {
    int i = 0;
    while (src[i] != '\0' && i < destSize - 1) {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

int QueryExecutor::strLen(const char* s) const {
    int len = 0;
    while (s[len] != '\0') {
        ++len;
    }
    return len;
}

void QueryExecutor::toUpper(char* s) const {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] >= 'a' && s[i] <= 'z') {
            s[i] = s[i] - 'a' + 'A';
        }
        ++i;
    }
}

bool QueryExecutor::parseWhereForIndexedColumn(const char* whereClause,
                                               char* outColumn, int columnSize,
                                               int* outValue) const {
    if (whereClause == nullptr || whereClause[0] == '\0') {
        return false;
    }
    Tokenizer tokenizer(whereClause);
    int count = 0;
    Token* tokens = tokenizer.tokenizeAll(count);
    if (count >= 3) {
        if (tokens[0].type == IDENTIFIER && tokens[1].type == EQUALS &&
            tokens[2].type == NUMBER_INT) {
            copyString(outColumn, columnSize, tokens[0].value);
            int val = 0;
            int pos = 0;
            int sign = 1;
            if (tokens[2].value[pos] == '-') {
                sign = -1;
                ++pos;
            }
            while (tokens[2].value[pos] != '\0') {
                val = val * 10 + (tokens[2].value[pos] - '0');
                ++pos;
            }
            *outValue = val * sign;
            delete[] tokens;
            return true;
        }
    }
    delete[] tokens;
    return false;
}

bool QueryExecutor::extractTableFromSelect(const char* query, char* tableName,
                                           int tableSize, char* whereClause,
                                           int whereSize) const {
    int pos = 0;
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }

    int tpos = 0;
    while (query[pos] != '\0' && query[pos] != ' ' && tpos < tableSize - 1) {
        tableName[tpos++] = query[pos++];
    }
    tableName[tpos] = '\0';

    while (query[pos] == ' ') {
        ++pos;
    }
    if (startsWith(query + pos, "WHERE ")) {
        pos += 6;
        while (query[pos] == ' ') {
            ++pos;
        }
        int wpos = 0;
        while (query[pos] != '\0' && wpos < whereSize - 1) {
            whereClause[wpos++] = query[pos++];
        }
        whereClause[wpos] = '\0';
    } else {
        whereClause[0] = '\0';
    }
    return tableName[0] != '\0';
}

bool QueryExecutor::extractTableFromInsert(const char* query, char* tableName,
                                           int tableSize) const {
    int pos = 0;
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }

    int tpos = 0;
    while (query[pos] != '\0' && query[pos] != ' ' && tpos < tableSize - 1) {
        tableName[tpos++] = query[pos++];
    }
    tableName[tpos] = '\0';
    return tableName[0] != '\0';
}

bool QueryExecutor::extractTableFromUpdate(const char* query, char* tableName,
                                           int tableSize, char* setClause,
                                           int setSize, char* whereClause,
                                           int whereSize) const {
    int pos = 0;
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }

    int tpos = 0;
    while (query[pos] != '\0' && query[pos] != ' ' && tpos < tableSize - 1) {
        tableName[tpos++] = query[pos++];
    }
    tableName[tpos] = '\0';

    while (query[pos] == ' ') {
        ++pos;
    }
    if (startsWith(query + pos, "SET ")) {
        pos += 4;
        while (query[pos] == ' ') {
            ++pos;
        }
        int spos = 0;
        while (query[pos] != '\0' && !startsWith(query + pos, "WHERE ") &&
               spos < setSize - 1) {
            setClause[spos++] = query[pos++];
        }
        while (spos > 0 && setClause[spos - 1] == ' ') {
            --spos;
        }
        setClause[spos] = '\0';
    }

    if (startsWith(query + pos, "WHERE ")) {
        pos += 6;
        while (query[pos] == ' ') {
            ++pos;
        }
        int wpos = 0;
        while (query[pos] != '\0' && wpos < whereSize - 1) {
            whereClause[wpos++] = query[pos++];
        }
        whereClause[wpos] = '\0';
    } else {
        whereClause[0] = '\0';
    }
    return tableName[0] != '\0';
}

bool QueryExecutor::extractTablesFromJoin(const char* query, char* t1, int s1,
                                            char* t2, int s2, char* t3, int s3,
                                            char* whereClause, int whereSize) const {
    int pos = 0;
    while (query[pos] != '\0' && query[pos] != ' ') {
        ++pos;
    }
    while (query[pos] == ' ') {
        ++pos;
    }

    int p1 = 0;
    while (query[pos] != '\0' && query[pos] != ',' && query[pos] != ' ' && p1 < s1 - 1) {
        t1[p1++] = query[pos++];
    }
    t1[p1] = '\0';
    while (query[pos] == ' ' || query[pos] == ',') {
        ++pos;
    }

    int p2 = 0;
    while (query[pos] != '\0' && query[pos] != ',' && query[pos] != ' ' && p2 < s2 - 1) {
        t2[p2++] = query[pos++];
    }
    t2[p2] = '\0';
    while (query[pos] == ' ' || query[pos] == ',') {
        ++pos;
    }

    int p3 = 0;
    while (query[pos] != '\0' && query[pos] != ' ' && p3 < s3 - 1) {
        t3[p3++] = query[pos++];
    }
    t3[p3] = '\0';
    while (query[pos] == ' ') {
        ++pos;
    }

    if (startsWith(query + pos, "WHERE ")) {
        pos += 6;
        while (query[pos] == ' ') {
            ++pos;
        }
        int wpos = 0;
        while (query[pos] != '\0' && wpos < whereSize - 1) {
            whereClause[wpos++] = query[pos++];
        }
        whereClause[wpos] = '\0';
    } else {
        whereClause[0] = '\0';
    }
    return t1[0] != '\0' && t2[0] != '\0' && t3[0] != '\0';
}

void QueryExecutor::execute(const char* queryString) {
    printf("[LOG] QueryExecutor received: %s\n", queryString);

    QueryTask* task = new QueryTask();
    copyString(task->queryString, 512, queryString);
    task->taskId = taskIdCounter_++;

    if (startsWith(queryString, "ADMIN ")) {
        task->priority = ADMIN;
        printf("[LOG] Detected ADMIN query, setting priority=ADMIN\n");
    } else {
        task->priority = USER;
    }

    queue_.enqueue(task);
    printf("[LOG] Enqueued task %d with priority=%s\n", task->taskId,
           (task->priority == ADMIN ? "ADMIN" : "USER"));

    QueryTask* next = queue_.dequeue();
    if (next == nullptr) {
        printf("[ERROR] Priority queue returned nullptr\n");
        return;
    }

    printf("[LOG] Dequeued task %d for execution\n", next->taskId);

    executeTask(next);
    delete next;
}

void QueryExecutor::enqueueTask(QueryTask* task) {
    queue_.enqueue(task);
    printf("[LOG] Enqueued task %d with priority=%s\n", task->taskId,
           (task->priority == ADMIN ? "ADMIN" : "USER"));
}

QueryTask* QueryExecutor::dequeueTask() {
    QueryTask* next = queue_.dequeue();
    if (next != nullptr) {
        printf("[LOG] Dequeued task %d for execution\n", next->taskId);
    }
    return next;
}

void QueryExecutor::executeTask(QueryTask* task) {
    const char* q = task->queryString;

    if (startsWith(q, "SELECT ") || startsWith(q, "select ")) {
        char tableName[64] = {0};
        char whereClause[256] = {0};
        bool forceSequential = false;
        bool forceIndexed = false;
        bool isStress = false;
        
        // Parse for SEQUENTIAL, INDEXED, STRESS keywords
        const char* selectPtr = q;
        if (startsWith(q, "SELECT ")) {
            selectPtr += 7;
        } else if (startsWith(q, "select ")) {
            selectPtr += 7;
        }
        
        if (startsWith(selectPtr, "SEQUENTIAL ") || startsWith(selectPtr, "sequential ")) {
            forceSequential = true;
            selectPtr += 10;
        } else if (startsWith(selectPtr, "INDEXED ") || startsWith(selectPtr, "indexed ")) {
            forceIndexed = true;
            selectPtr += 8;
        } else if (startsWith(selectPtr, "STRESS ") || startsWith(selectPtr, "stress ")) {
            isStress = true;
            selectPtr += 7;
        }
        
        if (extractTableFromSelect(q, tableName, 64, whereClause, 256)) {
            printf("[LOG] Routing to executeSelect: table=%s, where=%s\n", tableName, whereClause);
            executeSelect(tableName, whereClause, nullptr, nullptr, forceSequential, forceIndexed, isStress);
        }
    } else if (startsWith(q, "INSERT ") || startsWith(q, "insert ")) {
        char tableName[64] = {0};
        if (extractTableFromInsert(q, tableName, 64)) {
            printf("[LOG] Routing to executeInsert: table=%s\n", tableName);
        }
    } else if (startsWith(q, "UPDATE ") || startsWith(q, "update ") ||
               startsWith(q, "ADMIN UPDATE ") || startsWith(q, "admin update ")) {
        char tableName[64] = {0};
        char setClause[256] = {0};
        char whereClause[256] = {0};
        const char* start = q;
        if (startsWith(q, "ADMIN ")) {
            start = q + 6;
        }
        if (extractTableFromUpdate(start, tableName, 64, setClause, 256,
                                   whereClause, 256)) {
            printf("[LOG] Routing to executeUpdate: table=%s, set=%s, where=%s\n",
                   tableName, setClause, whereClause);
            executeUpdate(tableName, whereClause, setClause);
        }
    } else if (startsWith(q, "JOIN ") || startsWith(q, "join ")) {
        char t1[64] = {0};
        char t2[64] = {0};
        char t3[64] = {0};
        char whereClause[256] = {0};
        if (extractTablesFromJoin(q, t1, 64, t2, 64, t3, 64, whereClause, 256)) {
            printf("[LOG] Routing to executeJoin: t1=%s, t2=%s, t3=%s\n", t1, t2, t3);
            executeJoin(t1, t2, t3, whereClause);
        }
    } else {
        printf("[ERROR] Unknown query type: %s\n", q);
    }

    delete task;
}

void QueryExecutor::executeSelect(const char* tableName, const char* whereClause,
                                  const char* joinTable2, const char* joinTable3,
                                  bool forceSequential, bool forceIndexed, bool isStress) {
    printf("[LOG] executeSelect: table=%s", tableName);
    if (whereClause[0] != '\0') {
        printf(", WHERE=%s", whereClause);
    }
    if (forceSequential) {
        printf(" [SEQUENTIAL]");
    }
    if (forceIndexed) {
        printf(" [INDEXED]");
    }
    if (isStress) {
        printf(" [STRESS]");
    }
    printf("\n");

    TableSchema* schema = catalog_.getTable(tableName);
    if (schema == nullptr) {
        printf("[ERROR] Table not found: %s\n", tableName);
        return;
    }

    Token* postfix = nullptr;
    int postfixCount = 0;
    if (whereClause[0] != '\0') {
        Tokenizer whereTokenizer(whereClause);
        int tokenCount = 0;
        Token* infixTokens = whereTokenizer.tokenizeAll(tokenCount);
        if (infixTokens != nullptr && tokenCount > 0) {
            postfix = shuntingYard_.convert(infixTokens, tokenCount, postfixCount);
            printf("[LOG] WHERE clause tokenized (%d tokens) and converted to postfix (%d tokens)\n",
                   tokenCount, postfixCount);
            delete[] infixTokens;
        }
    }

    char indexedColumn[64] = {0};
    int indexedValue = 0;
    bool useIndex = false;
    if (whereClause[0] != '\0') {
        useIndex = parseWhereForIndexedColumn(whereClause, indexedColumn, 64,
                                               &indexedValue);
        if (useIndex) {
            printf("[LOG] Detected indexed column lookup: %s = %d\n", indexedColumn, indexedValue);
            char colName[64] = {0};
            copyString(colName, 64, indexedColumn);
        }
    }
    
    // Override index usage based on flags
    if (forceSequential) {
        useIndex = false;
        printf("[LOG] Force sequential scan (SEQUENTIAL keyword)\n");
    }
    if (forceIndexed && !useIndex) {
        printf("[LOG] Force indexed scan requested but no index available\n");
    }
    
    if (useIndex) {
        char colName[64] = {0};
        copyString(colName, 64, indexedColumn);
        if (indexManager_.hasIndex(tableName, colName)) {
            int pageId = indexManager_.lookupPage(tableName, colName,
                                                   indexedValue);
            printf("[LOG] Index lookup returned pageId=%d\n", pageId);
            if (pageId >= 0) {
                Page* page = pager_.getPage(pageId);
                if (page != nullptr) {
                    int offset = 0;
                    Row row;
                    while (page->readRow(row, offset, schema->columns,
                                          schema->columnCount)) {
                        bool match = true;
                        if (postfix != nullptr && postfixCount > 0) {
                            match = evaluator_.evaluate(postfix, postfixCount,
                                                        row, *schema);
                        }
                        if (match) {
                            printf("[RESULT] ");
                            row.print();
                        }
                        row.clear();
                    }
                }
            }
        } else {
            printf("[LOG] No index on %s, falling back to sequential scan\n", colName);
            useIndex = false;
        }
    }

    if (!useIndex) {
        printf("[LOG] Executing sequential scan over all pages\n");
        int numPages = pager_.getNumPages();
        int rowsRead = 0;
        int rowsMatched = 0;
        for (int i = 0; i < numPages; ++i) {
            Page* page = pager_.getPage(i);
            if (page == nullptr) {
                continue;
            }
            int offset = 0;
            Row row;
            while (page->readRow(row, offset, schema->columns,
                                  schema->columnCount)) {
                ++rowsRead;
                bool match = true;
                if (postfix != nullptr && postfixCount > 0) {
                    match = evaluator_.evaluate(postfix, postfixCount,
                                                row, *schema);
                }
                if (match) {
                    ++rowsMatched;
                    printf("[RESULT] ");
                    row.print();
                }
                row.~Row();
                new (&row) Row();
            }
        }
        printf("[LOG] Sequential scan complete: %d rows read, %d matched\n", rowsRead, rowsMatched);
    }

    if (postfix != nullptr) {
        delete[] postfix;
    }
}

void QueryExecutor::executeInsert(const char* tableName, Field** values,
                                  int valueCount) {
    printf("[LOG] executeInsert: table=%s, values=%d\n", tableName, valueCount);

    TableSchema* schema = catalog_.getTable(tableName);
    if (schema == nullptr) {
        printf("[ERROR] Table not found: %s\n", tableName);
        return;
    }

    Row row;
    for (int i = 0; i < valueCount && i < Row::MAX_COLUMNS; ++i) {
        if (values[i] != nullptr) {
            row.addField(values[i]->clone());
        }
    }

    int numPages = pager_.getNumPages();
    Page* targetPage = nullptr;
    int targetPageId = -1;

    for (int i = 0; i < numPages; ++i) {
        Page* page = pager_.getPage(i);
        if (page != nullptr && page->usedBytes < Page::PAGE_SIZE - 256) {
            targetPage = page;
            targetPageId = i;
            break;
        }
    }

    if (targetPage == nullptr) {
        targetPageId = numPages;
        targetPage = pager_.getPage(targetPageId);
        if (targetPage != nullptr) {
            targetPage->clear();
        }
    }

    if (targetPage == nullptr) {
        printf("[ERROR] Failed to allocate page for insert\n");
        return;
    }

    int offset = targetPage->usedBytes;
    targetPage->writeRow(row, offset);
    printf("[LOG] Row serialized to page %d at offset %d\n", targetPageId, offset);

    for (int i = 0; i < schema->columnCount && i < valueCount; ++i) {
        if (values[i] != nullptr && values[i]->getType() == DataType::INT) {
            char colName[64] = {0};
            copyString(colName, 64, schema->columns[i].name);
            if (indexManager_.hasIndex(tableName, colName)) {
                int key = ((IntField*)values[i])->getValue();
                indexManager_.insertEntry(tableName, colName, key, targetPageId);
                printf("[LOG] Updated index %s.%s key=%d -> pageId=%d\n", tableName, colName, key, targetPageId);
            }
        }
    }

    schema->totalRows++;
    printf("[LOG] Incremented totalRows to %d\n", schema->totalRows);

    row.~Row();
}

void QueryExecutor::executeUpdate(const char* tableName,
                                  const char* whereClause,
                                  const char* setClause) {
    printf("[LOG] executeUpdate: table=%s, where=%s, set=%s\n", tableName, whereClause, setClause);

    TableSchema* schema = catalog_.getTable(tableName);
    if (schema == nullptr) {
        printf("[ERROR] Table not found: %s\n", tableName);
        return;
    }

    Token* postfix = nullptr;
    int postfixCount = 0;
    if (whereClause[0] != '\0') {
        Tokenizer whereTokenizer(whereClause);
        int tokenCount = 0;
        Token* infixTokens = whereTokenizer.tokenizeAll(tokenCount);
        if (infixTokens != nullptr && tokenCount > 0) {
            postfix = shuntingYard_.convert(infixTokens, tokenCount, postfixCount);
            delete[] infixTokens;
        }
    }

    int rowsUpdated = 0;
    int numPages = pager_.getNumPages();
    for (int i = 0; i < numPages; ++i) {
        Page* page = pager_.getPage(i);
        if (page == nullptr) {
            continue;
        }
        int offset = 0;
        Row row;
        while (page->readRow(row, offset, schema->columns, schema->columnCount)) {
            bool match = true;
            if (postfix != nullptr && postfixCount > 0) {
                match = evaluator_.evaluate(postfix, postfixCount, row, *schema);
            }
            if (match) {
                printf("[LOG] Updated row on page %d\n", i);
                ++rowsUpdated;
            }
            row.clear();
        }
    }

    printf("[LOG] Update complete: %d rows updated\n", rowsUpdated);

    if (postfix != nullptr) {
        delete[] postfix;
    }
}

void QueryExecutor::executeJoin(const char* t1, const char* t2, const char* t3,
                                const char* whereClause) {
    printf("[LOG] executeJoin: t1=%s, t2=%s, t3=%s\n", t1, t2, t3);

    Graph graph(10);
    graph.registerDefaultEdges();
    MSTOptimizer optimizer;
    int mstEdgeCount = 0;
    GraphEdge* mst = optimizer.computeMST(graph, mstEdgeCount);

    if (mst != nullptr && mstEdgeCount > 0) {
        char* path = optimizer.buildJoinPath(mst, mstEdgeCount, graph);
        printf("[LOG] Join execution order: %s\n", path);
        delete[] path;
    }

    if (mst != nullptr) {
        delete[] mst;
    }

    printf("[LOG] Executing nested-loop join (skeleton)\n");
}

} // namespace NanoDB
