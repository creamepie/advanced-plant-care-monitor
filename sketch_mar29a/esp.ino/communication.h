// Buffer for receiving data
const int MAX_MESSAGE_LENGTH = 512; // Increased from 384 to 512 for larger messages
extern char* receivedChars; // Changed: Remove size from extern declaration
extern boolean messageInProgress;
extern int charIndex;
