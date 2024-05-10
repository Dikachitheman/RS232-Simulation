#include <Windows.h>    
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define EX_FATAL 1


// Prototype the functions to be used
void initPort(HANDLE* hCom, const char* COMPORT, int nComRate, int nComBits, COMMTIMEOUTS timeout, int comNum);
void purgePort(HANDLE* hCom);
void outputToPort(HANDLE* hCom, LPCVOID buf, DWORD szBuf);
DWORD inputFromPort(HANDLE* hCom, LPVOID buf, DWORD szBuf);

// Sub functions
void createPortFile(HANDLE* hCom, const char* COMPORT, int comNum);
static int SetComParms(HANDLE* hCom, int nComRate, int nComBits, COMMTIMEOUTS timeout);


struct messageElements {
    int bit;
    char *name;
    int representation;
};

struct messageElements m[12] = { { 1, "Continuation bit", 8},
                                 { 2, "Primary account number (PAN)", 19},
                                 { 3, "Processing code", 6}, 
                                 { 4, "Amount transaction", 16},
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
	
	for (int h = 0; h < 70; h++)
	{
		printf("*");
		Sleep(20);
	}

	printf("\n\nDecoding ISO message\n");
	printf("\n");
    printf("MTI: {%s}\n", firstArray);
    printf("Bitmap and DataElements {%s}\n", secondArray);
	Sleep(500);	

    firstArrayLength = 12;
    splitBuffer(secondArray, firstArrayLength, &firstArray, &secondArray);

    printf("bitmap: {%s}\n", firstArray);
    printf("DataElements {%s}\n\n", secondArray);
	Sleep(500);	

    char *bitmap = (char*)malloc((12) * sizeof(char));
    strncpy(bitmap, firstArray, 12);
    // printf("%s\n", bitmap);

    for (int i = 0; i < 12; i++)
    {
        if (bitmap[i] == '1')
        {
            printf("*********bit: %d--- ", m[i].bit);
            firstArrayLength = m[i].representation;
            splitBuffer(secondArray, firstArrayLength, &firstArray, &secondArray);
            printf("name: %s --- value %s\n", m[i].name, firstArray);
            printf("\n");
        }
		Sleep(300);
        // for (int j = 0; j < strlen(secondArray); j++)
        // {
        //     printf("%c", secondArray[j]);
        // }

    
    }


    free(bitmap);
    free(firstArray);
    free(secondArray);

	Sleep(500);	

    return 0;
}




// Initializes the port and sets the communication parameters
void initPort(HANDLE* hCom, const char* COMPORT, int nComRate, int nComBits, COMMTIMEOUTS timeout, int comNum) {
	createPortFile(hCom, COMPORT, comNum);						// Initializes hCom to point to PORT#
	purgePort(hCom);									// Purges the COM port
	SetComParms(hCom, nComRate, nComBits, timeout);		// Uses the DCB structure to set up the COM port
	purgePort(hCom);
}

// Purge any outstanding requests on the serial port (initialize)
void purgePort(HANDLE* hCom) {
	PurgeComm(*hCom, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

// Output/Input messages to/from ports 
void outputToPort(HANDLE* hCom, LPCVOID buf, DWORD szBuf) {
	int i=0;
	DWORD NumberofBytesTransmitted;
	LPDWORD lpErrors=0;
	LPCOMSTAT lpStat=0; 

	i = WriteFile(
		*hCom,										// Write handle pointing to COM port
		buf,										// Buffer size
		szBuf,										// Size of buffer
		&NumberofBytesTransmitted,					// Written number of bytes
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nWrite Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.	
	}
	else
		printf("\nSuccessful transmission, there were %ld bytes transmitted\n", NumberofBytesTransmitted);
}

DWORD inputFromPort(HANDLE* hCom, LPVOID buf, DWORD szBuf) {
	int i = 0;
	DWORD NumberofBytesRead;
	LPDWORD lpErrors = 0;
	LPCOMSTAT lpStat = 0;

	i = ReadFile(
		*hCom,										// Read handle pointing to COM port
		buf,										// Buffer size
		szBuf,  									// Size of buffer - Maximum number of bytes to read
		&NumberofBytesRead,
		NULL
	);
	// Handle the timeout error
	if (i == 0) {
		printf("\nRead Error: 0x%x\n", GetLastError());
		ClearCommError(hCom, lpErrors, lpStat);		// Clears the device error flag to enable additional input and output operations. Retrieves information ofthe communications error.
	}
	else
	
		function(buf);
		for (int h = 0; h < 70; h++)
		{
			printf("*");
			Sleep(20);
		}
		Sleep(500);
		printf("\nSuccessful reception!, There were %ld bytes read\n", NumberofBytesRead);

	return(NumberofBytesRead);
}



// Sub functions called by above functions
/**************************************************************************************/
// Set the hCom HANDLE to point to a COM port, initialize for reading and writing, open the port and set securities
void createPortFile(HANDLE* hCom, const char* COMPORT, int comNum) {
	// Call the CreateFile() function 
	*hCom = CreateFile(
		COMPORT,									// COM port number  --> If COM# is larger than 9 then use the following syntax--> "\\\\.\\COM10"
		GENERIC_READ | GENERIC_WRITE,				// Open for read and write
		NULL,										// No sharing allowed
		NULL,										// No security
		OPEN_EXISTING,								// Opens the existing com port
		FILE_ATTRIBUTE_NORMAL,						// Do not set any file attributes --> Use synchronous operation
		NULL										// No template
	);
	
	if (*hCom == INVALID_HANDLE_VALUE) {
		printf("\nFatal Error 0x%x: Unable to open\n", GetLastError());
	}
	else {
		printf("\nCOM %d is now open\n", comNum);
	}
}

static int SetComParms(HANDLE* hCom, int nComRate, int nComBits, COMMTIMEOUTS timeout) {
	DCB dcb;										// Windows device control block
	// Clear DCB to start out clean, then get current settings
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	if (!GetCommState(*hCom, &dcb))
		return(0);

	// Set our own parameters from Globals
	dcb.BaudRate = nComRate;						// Baud (bit) rate
	dcb.ByteSize = (BYTE)nComBits;					// Number of bits(8)
	dcb.Parity = 0;									// No parity	
	dcb.StopBits = ONESTOPBIT;						// One stop bit
	if (!SetCommState(*hCom, &dcb))
		return(0);

	// Set communication timeouts (SEE COMMTIMEOUTS structure in MSDN) - want a fairly long timeout
	memset((void *)&timeout, 0, sizeof(timeout));
	timeout.ReadIntervalTimeout = 500;				// Maximum time allowed to elapse before arival of next byte in milliseconds. If the interval between the arrival of any two bytes exceeds this amount, the ReadFile operation is completed and buffered data is returned
	timeout.ReadTotalTimeoutMultiplier = 1;			// The multiplier used to calculate the total time-out period for read operations in milliseconds. For each read operation this value is multiplied by the requested number of bytes to be read
	timeout.ReadTotalTimeoutConstant = 5000;		// A constant added to the calculation of the total time-out period. This constant is added to the resulting product of the ReadTotalTimeoutMultiplier and the number of bytes (above).
	SetCommTimeouts(*hCom, &timeout);
	return(1);
}



// Note: Comment out the Tx or Rx sections below to operate in single sided mode

// Declare constants, variables and communication parameters
const int BUFSIZE = 140;							// Buffer size
const char COMPORT_Rx[] = "COM7";						// COM port used for Tx (use L"COM7" for receiving program)
const char COMPORT_Tx[] = "COM6";
int comSix = 6;
int comSeven = 7;		

// Communication variables and parameters
HANDLE hComRx;										// Pointer to the selected COM port (Receiver)
HANDLE hComTx;										// Pointer to the selected COM port (Transmitter)
int nComRate = 9600;								// Baud (Bit) rate in bits/second 
int nComBits = 8;									// Number of bits per frame
COMMTIMEOUTS timeout;								// A commtimeout struct variable

 // The client - A testing main that calls the functions below
int main() {

	// Set up both sides of the comm link
	initPort(&hComTx, COMPORT_Tx, nComRate, nComBits, timeout, comSix);	// Initialize the Rx port
	Sleep(500);
	initPort(&hComRx, COMPORT_Rx, nComRate, nComBits, timeout, comSeven);	// Initialize the Tx port
	Sleep(500);

	// Transmit side 
	char msgOut[] = "101000010100001015463547587587567576947465458446353535356834";	// Sent message	
	// char msgOut[] = "101010111110010101546354758758756757694746545844635353535683447587587567576947465458446353535354758758756757694746545844635353535";							// Sent message	
	Sleep(500);
	printf("\nMessage Sent: %s\n\n", msgOut);
	outputToPort(&hComTx, msgOut, strlen(msgOut)+1);			// Send string to port - include space for '\0' termination
	Sleep(500);													// Allow time for signal propagation on cable 

	// Receive side  
	char msgIn[BUFSIZE];
	DWORD bytesRead;
	bytesRead = inputFromPort(&hComRx, msgIn, BUFSIZE);			// Receive string from port
	//printf("Length of received msg = %d", bytesRead);
	msgIn[bytesRead] = '\0';
	printf("\nMessage Received: %s\n\n", msgIn);				// Display message from port
	
	// Tear down both sides of the comm link
	purgePort(&hComRx);											// Purge the Rx port
	purgePort(&hComTx);											// Purge the Tx port
	CloseHandle(hComRx);										// Close the handle to Rx port 
	CloseHandle(hComTx);										// Close the handle to Tx port 
	
	system("pause");
}