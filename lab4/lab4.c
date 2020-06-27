#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node_t {
  char* data;
  struct node_t *next;
} Node;

Node * insert_bottom(char* str, Node *head)
{
    Node *current_node = head;
    Node *new_node;

    while ( current_node != NULL && current_node->next != NULL) {
        current_node = current_node->next;
    }

    new_node = (Node *) malloc(sizeof(Node));
    new_node->data = str;
    new_node->next= NULL;
    if (current_node != NULL)
        current_node->next = new_node;
    else
        head = new_node;
    return head;
}

void print(Node *head)
{
    Node *current_node = head;

    while ( current_node != NULL) {
        printf("%s", current_node->data);
        current_node = current_node->next;
    }
}

void empty_str(char* str)
{
    for (int j = 0; j < 2048; ++j) {
            str[j] = '\0';
        }
}

int main()
{
    char raw_str[2048];
    char* str = NULL;
    Node *_list = NULL;

    fgets(raw_str, 2048, stdin);
    while(strcmp(raw_str, ".\n") != 0) {
        str = (char*)malloc((strlen(raw_str)+1)*sizeof(char));
        strcpy(str, raw_str);
        _list = insert_bottom(str, _list);
        str = NULL;
        empty_str(raw_str);
        fgets(raw_str, 2048, stdin);
    }

    print(_list);
    free(_list);
    free(str);

    return 0;
}
