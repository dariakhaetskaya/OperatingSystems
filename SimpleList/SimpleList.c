#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct Node {
    char *data;
    struct Node* nextNode;
} Node;

void freeNode(Node* node){
    node->nextNode = NULL;
    free(node->data);
    free(node);
}

Node* createNode(){
    Node* node = (Node*)malloc(1 * sizeof(Node));
    if (node == NULL){
        perror("malloc(3C) failed to allocate a new node\n");
        return NULL;
    }
    node->nextNode = NULL;
    node->data = NULL;
    return node;
}

Node* fillNode(char* line){
    Node* node = createNode();

    node->data = (char*)calloc((strlen(line) + 1), sizeof(char));
    if (node->data == NULL){
        free(node);
        return NULL;
    }

    memcpy(node->data, line, (strlen(line) + 1) * sizeof(char));

    node->nextNode = NULL;
    return node;
}

void freeList(Node* head){
    if (head == NULL){
        return;
    }

    Node* nextNode = head->nextNode;
    Node* savedNode = NULL;
    while (nextNode != NULL){
        savedNode = nextNode->nextNode;
        freeNode(nextNode);
        nextNode = savedNode;
    }

    freeNode(head);
}

void printList(Node* head){
    Node* node;
    for (node = head->nextNode; node != NULL; node = node->nextNode){
        printf("%s", node->data);
    }
}

int main(){
    char buffer[BUFSIZ];

    Node* head = createNode();
    if(head == NULL){
        perror("No available memory\n");
        return 0;
    }
    Node* currentNode = head;

    printf("Please enter your strings here: \n");

    while (fgets(buffer, BUFSIZ, stdin) != EOF){

        if(buffer[0] == '.'){
            break;
        }

        currentNode->nextNode = fillNode(buffer);
        if(currentNode->nextNode == NULL){
            perror("No available memory\n");
            freeList(head);
            return 0;
        }

        currentNode = currentNode->nextNode;
    }

    printList(head);
    freeList(head);

    return 0;
}
