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

    #define SET_NODE_VIEW_(thisShape, thisColor, thisStr) \
        strcpy(nodeView->shape , #thisShape);             \
        strcpy(nodeView->color , #thisColor );            \
        strcpy(nodeView->str   , #thisStr   );            \
        break

    switch ((int)node->nodeType)
    {
        case ADD:      { SET_NODE_VIEW_(octagon, yellow, +)  ; }
        case SUB:      { SET_NODE_VIEW_(octagon, yellow, -)  ; }
        case MUL:      { SET_NODE_VIEW_(octagon, yellow, *)  ; }
        case DIV:      { SET_NODE_VIEW_(octagon, yellow, /)  ; }
        case POW:      { SET_NODE_VIEW_(octagon, yellow, ^)  ; }
        case SIN:      { SET_NODE_VIEW_(circle, yellow, sin) ; }
        case COS:      { SET_NODE_VIEW_(circle, yellow, cos) ; }
        case LN:       { SET_NODE_VIEW_(circle, yellow, ln)  ; }
        case VARIABLE: { SET_NODE_VIEW_(rectangle, red, x)   ; }
        case CONST:
        {
            sprintf(nodeView->str, "%g", node->value);
            strcpy(nodeView->shape, "rectangle");
            strcpy(nodeView->color, "green");
            break;
        }
        default:
        {
            printf("Error in NodeViewBuild!\n");
            break;
        }
    }

    #undef SET_NODE_VIEW_

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

Node_t* TreeInsert(Tree_t *tree, Node_t *node, const NodeChild child, TreeErrorCode *treeError)
{
    assert(tree      != nullptr);
    assert(node      != nullptr);
    assert(treeError != nullptr);

    *treeError = TREE_NO_ERROR;
    Node_t *newNode = nullptr;
    #define TREE_INSERT_(branch)                                            \
        do                                                                  \
        {                                                                   \
        node->branch = (Node_t*)calloc(1, sizeof(Node_t));                  \
        if (node->branch == nullptr)                                        \
        {                                                                   \
            *treeError = TREE_INSERT_ERROR;                                 \
        }                                                                   \
        node->branch->parent = node;                                        \
        newNode = node->branch;                                             \
        } while(0)

    if (child == LEFT_CHILD)
    {
        TREE_INSERT_(leftChild);
    }
    else
    {
        TREE_INSERT_(rightChild);
    }

    #undef TREE_INSERT_

    if (tree->size == 0)
    {
        Node_t *newBeginNode = tree->root->leftChild;
        free(tree->root);
        tree->root = newBeginNode;
        tree->root->parent = nullptr;
    }
    tree->size = tree->size + 1;

    return newNode;
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

static NodeType DefineNodeType(const char *str)
{
    assert(str != nullptr);

    #define STRCOMPARE_(thisStr) \
        strcmp(str, thisStr) == 0

    if (STRCOMPARE_("x"))
    {
        return VARIABLE;
    }
    else if (STRCOMPARE_("sin"))
    {
        return SIN;
    }
    else if (STRCOMPARE_("cos"))
    {
        return COS;
    }
    else if (STRCOMPARE_("ln"))
    {
        return LN;
    }
    else if (STRCOMPARE_("+"))
    {
        return ADD;
    }
    else if (STRCOMPARE_("-"))
    {
        return SUB;
    }
    else if (STRCOMPARE_("*"))
    {
        return MUL;
    }
    else if (STRCOMPARE_("/"))
    {
        return DIV;
    }
    else if (STRCOMPARE_("^"))
    {
        return POW;
    }
    else
    {
        return CONST;
    }

    #undef STRCOMPARE_
}

void SetNodeTypeAndValue(Node_t *node, const NodeType nodeType, const double value)
{
    assert(node != nullptr);

    node->nodeType = nodeType;
    node->value    = value;
}

static char* NodeBuild(Tree_t *tree, Node_t *node, char *str, TreeErrorCode *treeError, const NodeChild child)
{
    assert(tree      != nullptr);
    assert(node      != nullptr);
    assert(str       != nullptr);
    assert(treeError != nullptr);

    char mathOperation[3] = {};

    if (*str == '(' && *(str + 1) == '(')
    {
        Node_t *newNode = TreeInsert(tree, node, child, treeError);

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);

        strncpy(mathOperation, str, 1);
        newNode->nodeType = DefineNodeType(mathOperation);
        newNode->value = -1.0;

        str = str + 1;
        str = NodeBuild(tree, newNode, str, treeError, RIGHT_CHILD);
    }
    else
    {
        str = str + 1;
        Node_t *newNode = TreeInsert(tree, node, child, treeError);
        if (strncmp(str, "sin", 3) == 0 || strncmp(str, "cos", 3) == 0)
        {
            strncpy(mathOperation, str, 3);
            str = str + 3;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else if (strncmp(str, "ln", 2) == 0)
        {
            strncpy(mathOperation, str, 2);
            str = str + 2;
            str = NodeBuild(tree, newNode, str, treeError, LEFT_CHILD);
        }
        else
        {
            char *endStr = StrBufferFindEndStr(str);
            *endStr = '\0';
            strcpy(mathOperation, str);
            str = endStr + 1;
        }

        newNode->nodeType = DefineNodeType(mathOperation);
        if (newNode->nodeType == CONST)
        {
            newNode->value = atof(mathOperation);
        }
        else
        {
            newNode->value = -1.0;
        }

        while (*str == ')')
        {
            str = str + 1;
        }
    }

    return str;
}

TreeErrorCode TreeBuild(Tree_t *tree, FILE *finput)
{
    assert(tree   != nullptr);
    assert(finput != nullptr);

    char *str = TreeReadData(finput);
    if (str == nullptr)
    {
        return TREE_FILL_ERROR;
    }

    char *strCopy = str;
    TreeErrorCode treeError = TREE_NO_ERROR;

    NodeBuild(tree, tree->root, strCopy, &treeError, LEFT_CHILD);

    free(str);
    return treeError;
}

static void NodeSaveInFile(Node_t *node, FILE *foutput)
{
    assert(node    != nullptr);
    assert(foutput != nullptr);

    if (node->nodeType == CONST)
    {
        fprintf(foutput, "%g", node->value);
    }
    else if (node->nodeType == VARIABLE)
    {
        fprintf(foutput, "%c", node->nodeType);
    }
    else if (node->nodeType == SIN)
    {
        fprintf(foutput, "\\sin");
        fprintf(foutput, "{");
        fprintf(foutput, "(");
        NodeSaveInFile(node->leftChild, foutput);
        fprintf(foutput, ")");
        fprintf(foutput, "}");
    }
    else if (node->nodeType == COS)
    {
        fprintf(foutput, "\\cos");
        fprintf(foutput, "{");
        fprintf(foutput, "(");
        NodeSaveInFile(node->leftChild, foutput);
        fprintf(foutput, ")");
        fprintf(foutput, "}");
    }
    else if (node->nodeType == LN)
    {
        fprintf(foutput, "\\ln");
        fprintf(foutput, "{");
        fprintf(foutput, "(");
        NodeSaveInFile(node->leftChild, foutput);
        fprintf(foutput, ")");
        fprintf(foutput, "}");
    }
    else
    {
        if (node->nodeType == MUL)
        {
            fprintf(foutput, "{");
            NodeSaveInFile(node->leftChild, foutput);
            fprintf(foutput, "}");

            fprintf(foutput, "\\cdot");

            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput);
            fprintf(foutput, "}");
        }
        else if (node->nodeType == DIV)
        {
            fprintf(foutput, "\\frac");

            fprintf(foutput, "{");
            NodeSaveInFile(node->leftChild, foutput);
            fprintf(foutput, "}");

            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput);
            fprintf(foutput, "}");
        }
        else if (node->nodeType == POW)
        {
            NodeSaveInFile(node->leftChild, foutput);
            fprintf(foutput, "%c", node->nodeType);
            fprintf(foutput, "{");
            NodeSaveInFile(node->rightChild, foutput);
            fprintf(foutput, "}");
        }
        else
        {
            fprintf(foutput, "(");
            NodeSaveInFile(node->leftChild, foutput);
            fprintf(foutput, "%c", node->nodeType);
            NodeSaveInFile(node->rightChild, foutput);
            fprintf(foutput, ")");
        }
    }
}

TreeErrorCode TreeSaveInFile(Tree_t *tree, FILE* data, const char *str)
{
    assert(tree != nullptr);
    assert(data != nullptr);
    assert(str  != nullptr);

    fprintf(data, "%s", str);
    fprintf(data, "\\[");
    NodeSaveInFile(tree->root, data);
    fprintf(data, "\\]");
    fprintf(data, "\n\n");

    return TREE_NO_ERROR;
}

int SubtreeCompare(const Node_t *node1, const Node_t *node2)
{
    assert(node1 != nullptr);
    assert(node2 != nullptr);

    if (node1->nodeType != node2->nodeType || node1->value != node2->value)
    {
        return NOT_EQUAL;
    }
    else
    {
        if ((node1->leftChild != nullptr && node2->leftChild != nullptr) && (node1->rightChild != nullptr && node2->rightChild != nullptr))
            return SubtreeCompare(node1->leftChild, node2->leftChild) * SubtreeCompare(node1->rightChild, node2->rightChild);
        else if ((node1->leftChild == nullptr && node2->leftChild == nullptr) && (node1->rightChild == nullptr && node2->rightChild == nullptr))
            return EQUAL;
        else
            return NOT_EQUAL;
    }
}
