#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure for storing names
struct Node {
    char name[100];            // Name stored in node
    struct Node* next;         // Pointer to next node
};

// Create a new node
struct Node* createNode(const char* name) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    strcpy(newNode->name, name);
    newNode->next = NULL;
    return newNode;
}

// Insert node at the end of the circular linked list
void insertEnd(struct Node** head, const char* name) {
    struct Node* newNode = createNode(name);

    if (*head == NULL) {
        *head = newNode;
        newNode->next = *head;
        return;
    }

    struct Node* temp = *head;
    while (temp->next != *head) {
        temp = temp->next;
    }

    temp->next = newNode;
    newNode->next = *head;
}

// Display the circular linked list
void display(struct Node* head) {
    if (head == NULL) return;

    struct Node* temp = head;
    printf("Names in the list:\n");
    do {
        printf("%s -> ", temp->name);
        temp = temp->next;
    } while (temp != head);
    printf("(back to head)\n");
}

// Sort the circular linked list alphabetically using bubble sort
void sortList(struct Node* head) {
    if (head == NULL) return;

    struct Node *i, *j;
    char tempName[100];

    for (i = head; i->next != head; i = i->next) {
        for (j = i->next; j != head; j = j->next) {
            if (strcmp(i->name, j->name) > 0) {
                // Swap names
                strcpy(tempName, i->name);
                strcpy(i->name, j->name);
                strcpy(j->name, tempName);
            }
        }
    }
}

// Main function
int main() {
    struct Node* head = NULL;
    int n;
    char input[100];

    printf("Enter the number of names: ");
    scanf("%d", &n);
    getchar();  // Consume newline left by scanf

    for (int i = 0; i < n; i++) {
        printf("Enter name %d: ", i + 1);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline character
        insertEnd(&head, input);
    }

    printf("\nOriginal Circular Linked List:\n");
    display(head);

    sortList(head);

    printf("\nSorted Circular Linked List (Alphabetical Order):\n");
    display(head);

    return 0;
}   
