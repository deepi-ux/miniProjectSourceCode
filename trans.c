// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
// Bank-account program with Account Type and Interest Calculation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // last name
    char firstName[10];   // first name
    char accountType[10]; // savings/current
    double balance;       // account balance
};

// function prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void addInterest(FILE *fPtr);

// main function
int main(int argc, char *argv[])
{
    FILE *cfPtr;
    unsigned int choice;

    // open file
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // menu loop
    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            addInterest(cfPtr);
            break;

        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr);

    return 0;
}

// create formatted text file
void textFile(FILE *readPtr)
{
    FILE *writePtr;

    struct clientData client = {0, "", "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("File could not be opened.");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%-12s%10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Type",
            "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-16s%-11s%-12s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.accountType,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("accounts.txt created successfully.\n");
}

// update account
void updateRecord(FILE *fPtr)
{
    unsigned int account;
    double transaction;

    struct clientData client = {0, "", "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    scanf("%u", &account);

    // validation
    if (account < 1 || account > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    // move pointer
    fseek(fPtr,
          (account - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    // account not found
    if (client.acctNum == 0)
    {
        printf("Account #%u has no information.\n",
               account);
    }
    else
    {
        printf("\nCurrent Account Details\n");

        printf("%-6u%-16s%-11s%-12s%10.2f\n",
               client.acctNum,
               client.lastName,
               client.firstName,
               client.accountType,
               client.balance);

        printf("\nEnter charge (+) or payment (-): ");
        scanf("%lf", &transaction);

        // prevent negative balance
        if (client.balance + transaction < 0)
        {
            printf("Insufficient balance.\n");
            return;
        }

        // update balance
        client.balance += transaction;

        // move pointer back
        fseek(fPtr,
              -((long)sizeof(struct clientData)),
              SEEK_CUR);

        // write updated record
        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Balance updated successfully.\n");
    }
}

// delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%u", &accountNum);

    // validation
    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    // move pointer
    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    // account does not exist
    if (client.acctNum == 0)
    {
        printf("Account %u does not exist.\n",
               accountNum);
    }
    else
    {
        // move pointer again
        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        // overwrite with blank record
        fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account deleted successfully.\n");
    }
}

// create new account
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", "", 0.0};

    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    scanf("%u", &accountNum);

    // validation
    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    // move pointer
    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    // account already exists
    if (client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n",
               client.acctNum);
    }
    else
    {
        printf("Enter lastname firstname accountType balance\n? ");

        scanf("%14s%9s%9s%lf",
              client.lastName,
              client.firstName,
              client.accountType,
              &client.balance);

        client.acctNum = accountNum;

        // move pointer
        fseek(fPtr,
              (client.acctNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        // write new record
        fwrite(&client,
               sizeof(struct clientData),
               1,
               fPtr);

        printf("Account added successfully.\n");
    }
}

// add yearly interest
void addInterest(FILE *fPtr)
{
    struct clientData client;

    double rate;

    printf("Enter interest rate (example 5 for 5%%): ");
    scanf("%lf", &rate);

    rate = rate / 100;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        // valid account only
        if (client.acctNum != 0 &&
            client.balance > 0)
        {
            printf("\nBefore Interest\n");

            printf("Account: %u  Type: %s  Balance: %.2f\n",
                   client.acctNum,
                   client.accountType,
                   client.balance);

            // savings gets full interest
            if (strcmp(client.accountType, "savings") == 0)
            {
                client.balance += client.balance * rate;
            }

            // current gets half interest
            else if (strcmp(client.accountType, "current") == 0)
            {
                client.balance += client.balance * (rate / 2);
            }

            printf("After Interest\n");

            printf("Account: %u  Balance: %.2f\n",
                   client.acctNum,
                   client.balance);

            // move pointer back
            fseek(fPtr,
                  -((long)sizeof(struct clientData)),
                  SEEK_CUR);

            // update file
            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);
        }
    }

    printf("\nInterest added successfully.\n");
}

// menu
unsigned int enterChoice(void)
{
    unsigned int menuChoice;

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - add yearly interest\n"
           "6 - end program\n? ");

    scanf("%u", &menuChoice);

    return menuChoice;
}
