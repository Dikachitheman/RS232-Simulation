#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct messageElements {
    int bit;
    char *name;
    int representation;
};

struct messageElements m[12] = { { 1, "Continuation bit", 8},
                                 { 2, "Primary account number (PAN)", 19},
                                 { 3, "Processing code", 6}, 
                                 { 4, "	Amount transaction", 16},
                                 { 5, "Amount reconciliation", 16},
                                 { 6, "Amount cardholder billing", 16}, 
                                 { 7, "Date and time transmission", 10},
                                 { 8, "Amount cardholder billing fee", 12},
                                 { 9, "Conversion rate reconciliation",8}, 
                                 { 10, "Conversion rate cardholder billing", 8},
                                 { 11, "Systems trace audit number", 12},
                                 { 12, "Date and time local transaction" ,14}
                                };



void splitBuffer(const char* buffer, int firstArrayLength, char** firstArray, char** secondArray) {
    int totalLength = strlen(buffer);
    int secondArrayLength = totalLength - firstArrayLength;

    // Allocate memory for the first array
    *firstArray = (char*)malloc((firstArrayLength + 1) * sizeof(char));
    strncpy(*firstArray, buffer, firstArrayLength);
    (*firstArray)[firstArrayLength] = '\0';

    // Allocate memory for the second array
    *secondArray = (char*)malloc((secondArrayLength + 1) * sizeof(char));
    strncpy(*secondArray, buffer + firstArrayLength, secondArrayLength);
    (*secondArray)[secondArrayLength] = '\0';
}

int function(char buffer[])
{
    int firstArrayLength = 4;
    char* firstArray;
    char* secondArray;

    splitBuffer(buffer, firstArrayLength, &firstArray, &secondArray);

    printf("MTI: %s\n", firstArray);
    printf("Bitmap and DataElements %s\n", secondArray);
    Sleep(500);	

    firstArrayLength = 12;
    splitBuffer(secondArray, firstArrayLength, &firstArray, &secondArray);

    printf("bitmap: %s\n", firstArray);
    printf("DataElements %s\n", secondArray);
    Sleep(500);	

    char *bitmap = (char*)malloc((12) * sizeof(char));
    strncpy(bitmap, firstArray, 12);
    printf("%s\n", bitmap);

    for (int i = 0; i < 12; i++)
    {
        if (bitmap[i] == '1')
        {
            printf("%d--- ", m[i].bit);
            firstArrayLength = m[i].representation;
            splitBuffer(secondArray, firstArrayLength, &firstArray, &secondArray);
            printf("name: %s --- value %s\n", m[i].name, firstArray);
            printf("\n");
        }

        // for (int j = 0; j < strlen(secondArray); j++)
        // {
        //     printf("%c", secondArray[j]);
        // }

    
    }


    free(bitmap);
    free(firstArray);
    free(secondArray);

    return 0;
}

int main() {
    char ISO_Message[] = "1010000101000010154635475875875675769474654584463535353568347796476467000000";
    printf("\n");
    printf("iso message %s\n", ISO_Message);

    function(ISO_Message);

    return 0;
}
