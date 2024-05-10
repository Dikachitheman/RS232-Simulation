// ask for input in iso message format
// save input in dynamic memory 
// take first four numbers 
// take the bitmap and insert into array or dynamic memory or stack
// iterate through bitmap --> if i = 0 then skip and increment j, if i == 1 then check index of table array using j and
// ...get value and tokenize bitmap with value and increment j
// write function to iterate through token values of MTI-- if !== 0 then print function
// 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 40

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

char *split_function(int length, char new_array[])
{
    int index;
    int length_of_message = strlen(new_array);
    int new_length = length_of_message - m[length].representation;

    char first_array[m[length].representation];
    char *second_array = (char*)malloc(length_of_message * sizeof(char));

    
    int j = 0;
    for (int i = 0; i < length_of_message; i++)
    {
        if (i < m[length].representation)
        {
            first_array[i] = new_array[i];
        }
        else
        {
            second_array[j] = new_array[i];
            j++;
        }
    }

    printf("%s -- ", m[length].name);
    for (int i = 0; i < m[length].representation; i++)
    {
        printf("%c", first_array[i]);
    }

    return (second_array);

    free(second_array);
}


int split_dataElements(char bitmap[], char dataElements[], int length)
{
    char *name = (char*)malloc(100 * sizeof(char));
    char *new_array = dataElements;

    for (int i = 0; i < 12; i++)
    {
        if (bitmap[i] == 1)
        {
            name = m[i].name;

            new_array = split_function(m[i].representation, new_array);
            printf("\n");
        }
    }

    free(name);
}

int bitmap_to_elements(char bitmap_and_elements[])
{
    int index;
    int length_of_message = strlen(bitmap_and_elements);
    int new_length = length_of_message - 12;

    char bitmap[12];
    char *dataElements = (char*)malloc(length_of_message * sizeof(char));

    
    printf("\ntest  ");
    for (int i = 0; i < 12; i++)
    {
        printf("%c", bitmap_and_elements[i]);
    }

    int j = 0;
    for (int i = 0; i < length_of_message; i++)
    {
        if (i < 12)
        {
            bitmap[i] = bitmap_and_elements[i];
        }
        else
        {
            dataElements[j] = bitmap_and_elements[i];
            j++;
        }
    }

    printf("\n");
    printf("bitmap  ");
    for (int i = 0; i < 12; i++)
    {
        printf("%c", bitmap[i]);
    }

    printf("\n");
    printf("data el  ");
    for (int i = 0; i < new_length; i++)
    {
        printf("%c", dataElements[i]);
    }

    split_dataElements(bitmap, dataElements, new_length);

    free(dataElements);
}

int split(char ISO_Message[])
{
    int length_of_message = strlen(ISO_Message);
    int new_length = length_of_message - 4;

    char message_type_indicator[4];
    char *bitmap_and_dataElements = (char*)malloc(length_of_message * sizeof(char));

    int j = 0;
    for (int i = 0; i < length_of_message; i++)
    {
        if (i < 4)
        {
            message_type_indicator[i] = ISO_Message[i];
        }
        else
        {
            bitmap_and_dataElements[j] = ISO_Message[i];
            j++;
        }
    }

    printf("\n");
    printf("MTI  ");
    for (int i = 0; i < 4; i++)
    {
        printf("%c", message_type_indicator[i]);
    }

    printf("\n");
    printf("bitmap + el  ");
    for (int i = 0; i < new_length; i++)
    {
        printf("%c", bitmap_and_dataElements[i]);
    }


    bitmap_to_elements(bitmap_and_dataElements);


    free(bitmap_and_dataElements);
}


int main() {

    char *ISO_Message = (char*)malloc(40 * sizeof(char));

    scanf("%s", ISO_Message);

    printf("%s", ISO_Message);

    int length_of_message = strlen(ISO_Message);

    printf("\n%d", length_of_message);

    split(ISO_Message);

    free(ISO_Message);



}
