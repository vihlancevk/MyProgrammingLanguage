#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <cstddef>
#include "Tree.h"
#include "FileOperations.h"

#define DEBUG

const size_t STR_MAX_SIZE = 120;
const size_t NodeView_STR_MAX_SIZE = 20;
const int NOT_EQUAL = 0;
const int EQUAL = 1;
const char *TREE_GRAPHVIZ = "graphviz.gv";

struct NodeView
{
    char shape[NodeView_STR_MAX_SIZE];
    char color[NodeView_STR_MAX_SIZE];
    char str[NodeView_STR_MAX_SIZE];
};

static void NodeViewBuild(const Node_t *node, NodeView *nodeView)
{
    assert(node     != nullptr);
    assert(nodeView != nullptr);

    sprintf(nodeView->str, "%g", node->value);
    strcpy(nodeView->shape, "rectangle");
    strcpy(nodeView->color, "green");

}

static void TreeVisitPrintNodeInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    NodeView nodeView = {};

    NodeViewBuild(node, &nodeView);

    char str[STR_MAX_SIZE] = {};
    sprintf(str, "\t%lu[shape=record, shape=%s, style=\"filled\", fillcolor=%s, label=\"%s\"];\n",
                 (long unsigned int)node, nodeView.shape, nodeView.color, nodeView.str);
    fprintf(foutput, "%s", str);

    if (node->leftChild  != nullptr) TreeVisitPrintNodeInFile(node->leftChild, foutput);
    if (node->rightChild != nullptr) TreeVisitPrintNodeInFile(node->rightChild, foutput);
}

static void TreeVisitPrintArrowInFile(const Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    if (node->parent != nullptr)
        fprintf(foutput, "\t%lu -> %lu[color=red, fontsize=12]\n", (long unsigned int)node, (long unsigned int)node->parent);

    if (node->leftChild  != nullptr)
        fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", (long unsigned int)node, (long unsigned int)node->leftChild);

    if (node->rightChild != nullptr)
        fprintf(foutput, "\t%lu -> %lu[fontsize=12]\n", (long unsigned int)node, (long unsigned int)node->rightChild);

    if (node->leftChild  != nullptr)
        TreeVisitPrintArrowInFile(node->leftChild, foutput);
    if (node->rightChild != nullptr)
        TreeVisitPrintArrowInFile(node->rightChild, foutput);
}

void TreeDump(Tree_t *tree)
{
    FILE *graphViz = fopen(TREE_GRAPHVIZ, "w");

    fprintf(graphViz, "digraph Tree{\n\n");
    fprintf(graphViz, "\trankdir=UD;\n\n");
    fprintf(graphViz, "\tnode[fontsize=14];\n\n");

    TreeVisitPrintNodeInFile(tree->root, graphViz);

    fprintf(graphViz, "\n");

    TreeVisitPrintArrowInFile(tree->root, graphViz);

    fprintf(graphViz, "\n");

    fprintf(graphViz, "}");

    fclose(graphViz);

    system("dot -Tpng graphviz.gv -o graphviz.png");
}

TreeErrorCode TreeCtor(Tree_t *tree)
{
    assert(tree != nullptr);

    TreeErrorCode treeError = TREE_NO_ERROR;

    if (tree->status != TREE_NOT_CONSTRUCTED)
    {
        treeError = TREE_CONSTRUCTED_ERROR;
    }

    tree->root = (Node_t*)calloc(1, sizeof(Node_t));
    if (tree->root == nullptr)
    {
        treeError = TREE_CONSTRUCTED_ERROR;
    }

    tree->size = 0;
    tree->status = TREE_CONSTRUCTED;

    return treeError;
}

void NodeDtor(Node_t *node)
{
    assert(node != nullptr);

    free(node);
}

void SubtreeDtor(Node_t *node)
{
    assert(node != nullptr);

    Node_t *leftChild  = node->leftChild;
    Node_t *rightChild = node->rightChild;

    free(node);

    if (leftChild  != nullptr) SubtreeDtor(leftChild);
    if (rightChild != nullptr) SubtreeDtor(rightChild);
}

TreeErrorCode TreeDtor(Tree_t *tree)
{
    assert(tree != nullptr);

    if (tree->status == TREE_DESTRUCTED)
    {
        return TREE_DESTRUCTED_ERROR;
    }

    SubtreeDtor(tree->root);
    tree->size = 0;
    tree->size = TREE_DESTRUCTED;

    return TREE_NO_ERROR;
}

static char* TreeReadData(FILE *finput)
{
    assert(finput != nullptr);

    int numberBytesFile = GetFileSize(finput);
    char *str = (char*)calloc(numberBytesFile, sizeof(char));
    str = (char*)ReadFile(finput, str, numberBytesFile);

    return str;
}

static char* StrBufferFindEndStr(char *str)
{
    assert(str != nullptr);

    return strchr(str, ')');
}

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType, const double value)
{
    assert(node != nullptr);

    node->nodeType = nodeType;
    node->value    = value;
}
