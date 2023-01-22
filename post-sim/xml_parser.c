#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>



int main() {
    xmlDoc *document;
    xmlNode *root, *first_child, *node;
    xmlChar* key;
    char *filename;

    filename = "/home/ulyana/Desktop/Experiments/model_input/skladConfig2.xml";
    document = xmlReadFile(filename, NULL, 0);

    if (document == NULL) {
        printf("Nope\n");
        return 1;
    }

    root = xmlDocGetRootElement(document);
    printf("Root is <%s> (%i)\n", root->name, root->type);
    first_child = root->children;
    for (node = first_child; node; node = node->next)  {
        if (node->type == 1) {
        printf("\t Child <%s> (%i)\n", node->name, node->type);
        key = xmlNodeGetContent(node);
        printf("\tkey: %s\n", (char*)key);
        }
    }
    printf("End\n");

    return 0;
}