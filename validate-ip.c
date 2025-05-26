#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Function: validate_ip
 * Purpose: Validates whether a given string represents a valid IPv4 address
 * 
 * IPv4 Address Format Requirements:
 * - Must contain exactly 4 octets (numbers) separated by 3 dots
 * - Each octet must be a number between 0 and 255 (inclusive)
 * - No leading zeros allowed (except for the number "0" itself)
 * - Only digits and dots are permitted characters
 * - Total length must be between 7 and 15 characters
 * 
 * Parameter: ip - pointer to null-terminated string containing the IP address to validate
 * Returns: 1 if valid IPv4 address, 0 if invalid
 */
int validate_ip(const char* ip) {
    // First safety check: ensure the input pointer is not NULL
    // This prevents segmentation faults when accessing the string
    if (ip == NULL) {
        return 0;  // Invalid: null pointer means no string to validate
    }
    
    // Create a buffer to hold a copy of the input string
    // IPv4 addresses have a maximum length of 15 characters: "255.255.255.255"
    char ip_copy[16];
    int len = strlen(ip);  // Get the length of the input string
    
    // Validate the overall length of the input string
    // Minimum valid IPv4: "0.0.0.0" (7 characters)
    // Maximum valid IPv4: "255.255.255.255" (15 characters)
    if (len < 7 || len > 15) {
        return 0;  // Invalid: string length is outside acceptable bounds
    }
    
    // Create a working copy of the input string because strtok() modifies the original
    // This preserves the original string for the caller
    strcpy(ip_copy, ip);
    
    // Initialize counter for dots in the IP address
    // A valid IPv4 address must have exactly 3 dots
    int dot_count = 0;
    char* token;      // Pointer to hold each parsed octet
    char* ptr = ip_copy;  // Pointer to our working copy of the string
    
    // First pass: Count dots and validate that only digits and dots are present
    // This preliminary check catches obvious format errors early
    for (int i = 0; i < len; i++) {
        if (ip[i] == '.') {
            dot_count++;  // Count each dot we encounter
        } else if (!isdigit(ip[i])) {
            // If character is not a digit and not a dot, the IP is invalid
            return 0;  // Invalid: contains non-digit, non-dot character
        }
    }
    
    // Check that we have exactly 3 dots (which creates 4 octets)
    if (dot_count != 3) {
        return 0;  // Invalid: wrong number of dots (too few or too many octets)
    }
    
    // Second pass: Parse each octet and validate its value
    // Use strtok to split the string by dots and examine each piece
    token = strtok(ptr, ".");  // Get the first octet
    int octet_count = 0;       // Keep track of how many octets we've processed
    
    // Process each octet until we've checked all 4 or encounter an error
    while (token != NULL && octet_count < 4) {
        // Check for empty octets (which would occur with consecutive dots like "192..1.1")
        if (strlen(token) == 0) {
            return 0;  // Invalid: empty octet found
        }
        
        // Validate leading zeros rule
        // Leading zeros are not allowed in IPv4 octets, except for "0" itself
        // Examples: "01", "001", "010" are invalid, but "0" is valid
        if (strlen(token) > 1 && token[0] == '0') {
            return 0;  // Invalid: leading zeros not allowed
        }
        
        // Convert the string token to an integer for range checking
        int num = atoi(token);
        
        // Check if the octet value is within the valid IPv4 range (0-255)
        if (num < 0 || num > 255) {
            return 0;  // Invalid: octet value is outside the 0-255 range
        }
        
        // Additional validation: ensure the conversion was accurate
        // This catches cases where atoi() might not parse correctly or overflow occurs
        // Convert the number back to string and compare with original token
        char temp[4];  // Buffer to hold the converted number as string (max "255")
        sprintf(temp, "%d", num);
        if (strcmp(temp, token) != 0) {
            return 0;  // Invalid: conversion mismatch indicates parsing error
        }
        
        // Move to the next octet
        token = strtok(NULL, ".");  // Get next token (octet) from the string
        octet_count++;              // Increment our octet counter
    }
    
    // Final validation: ensure we processed exactly 4 octets
    // This catches cases where there might be fewer than 4 octets
    if (octet_count != 4) {
        return 0;  // Invalid: wrong number of octets processed
    }
    
    // If we've made it through all validations, the IP address is valid
    return 1;  // Valid IPv4 address
}

/**
 * Function: main
 * Purpose: Interactive program entry point that allows users to input and validate IP addresses
 * 
 * Program Flow:
 * 1. Display welcome message and instructions
 * 2. Prompt user for IP address input
 * 3. Validate the entered IP address
 * 4. Display validation result with helpful feedback
 * 5. Ask user if they want to validate another IP
 * 6. Repeat until user chooses to exit
 * 
 * Returns: 0 on successful program completion
 */
int main() {
    // Declare variables for user interaction
    char ip_input[100];  // Buffer to store user's IP address input (generous size)
    char choice;         // Variable to store user's yes/no choice for continuing
    
    // Display program header and welcome information
    printf("IP Address Validator\n");
    printf("====================\n\n");
    
    // Main program loop - continues until user chooses to exit
    do {
        // Prompt user to enter an IP address for validation
        printf("Enter an IP address to validate: ");
        
        // Read user input using fgets() for safe input handling
        // fgets() is safer than scanf() as it prevents buffer overflow
        // and handles spaces/special characters better
        if (fgets(ip_input, sizeof(ip_input), stdin) != NULL) {
            // Remove the trailing newline character that fgets() includes
            // This newline comes from the user pressing Enter
            int len = strlen(ip_input);
            if (len > 0 && ip_input[len-1] == '\n') {
                ip_input[len-1] = '\0';  // Replace newline with null terminator
            }
            
            // Call our validation function to check if the IP is valid
            int result = validate_ip(ip_input);
            
            // Display the validation result to the user
            // Show both the input and whether it's valid or invalid
            printf("Result: '%s' is %s\n", ip_input, result ? "VALID" : "INVALID");
            
            // If the IP is invalid, provide helpful information about the correct format
            if (!result) {
                printf("Note: Valid IPv4 format is xxx.xxx.xxx.xxx where each xxx is 0-255\n");
                printf("      Examples: 192.168.1.1, 10.0.0.1, 255.255.255.0\n");
                printf("      Invalid examples: 256.1.1.1, 192.168.01.1, 192.168.1\n");
            }
        } else {
            // Handle the case where fgets() fails (unlikely but good practice)
            printf("Error reading input.\n");
        }
        
        // Ask user if they want to validate another IP address
        printf("\nDo you want to validate another IP address? (y/n): ");
        
        // Read user's choice using scanf with space before %c
        // The space before %c consumes any whitespace (including newlines)
        scanf(" %c", &choice);
        
        // Consume the remaining newline character left by scanf
        // This prevents it from interfering with the next fgets() call
        getchar();
        
        // Add spacing for better readability
        printf("\n");
        
    // Continue the loop if user entered 'y' or 'Y' (case-insensitive)
    } while (choice == 'y' || choice == 'Y');
    
    // Display goodbye message when user chooses to exit
    printf("Thank you for using the IP Address Validator!\n");
    
    // Return 0 to indicate successful program completion
    return 0;
}