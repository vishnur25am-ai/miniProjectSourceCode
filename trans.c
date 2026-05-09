#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
void login();
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void transferMoney(FILE *fPtr);
void applyInterest(FILE *fPtr);

// formatting
void printHeader();
void printRecord(struct clientData c);

// MAIN
int main()
{
    FILE *cfPtr;
    unsigned int choice;

    login();

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        printf("File could not be opened.\n");
        exit(1);
    }

    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1: textFile(cfPtr); break;
        case 2: updateRecord(cfPtr); break;
        case 3: newRecord(cfPtr); break;
        case 4: deleteRecord(cfPtr); break;
        case 6: searchRecord(cfPtr); break;
        case 7: displayAll(cfPtr); break;
        case 8: transferMoney(cfPtr); break;
        case 9: applyInterest(cfPtr); break;
        default: printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);
    return 0;
}

// LOGIN
void login()
{
    char password[20];
    printf("Enter password: ");
    scanf("%s", password);

    if (strcmp(password, "admin123") != 0)
    {
        printf("Access denied!\n");
        exit(0);
    }
}

// MENU
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n=========== BANK MENU ===========\n");
    printf("1 - Export to text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Exit\n");
    printf("6 - Search account\n");
    printf("7 - Display all accounts\n");
    printf("8 - Transfer money\n");
    printf("9 - Apply interest\n");
    printf("================================\n");
    printf("Enter choice: ");

    scanf("%u", &choice);
    return choice;
}

// FORMAT HEADER
void printHeader()
{
    printf("\n=============================================================\n");
    printf("%-8s %-15s %-15s %-12s\n",
           "Acct No", "Last Name", "First Name", "Balance");
    printf("-------------------------------------------------------------\n");
}

// FORMAT RECORD
void printRecord(struct clientData c)
{
    printf("%-8u %-15s %-15s ₹%-10.2f\n",
           c.acctNum, c.lastName, c.firstName, c.balance);
}

// EXPORT
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        printf("File error\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr, "=============================================================\n");
    fprintf(writePtr, "%-8s %-15s %-15s %-12s\n",
            "Acct No", "Last Name", "First Name", "Balance");
    fprintf(writePtr, "-------------------------------------------------------------\n");

    while (fread(&client, sizeof(client), 1, readPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-8u %-15s %-15s ₹%-10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);
    printf("Exported successfully\n");
}

// UPDATE (FIXED)
void updateRecord(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found\n");
        return;
    }

    printHeader();
    printRecord(client);

    printf("Enter amount (+ deposit / - withdraw): ");
    scanf("%lf", &amount);

    client.balance += amount;

    // SAFE WRITE POSITION (NO SEEK_CUR ISSUE)
    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fwrite(&client, sizeof(client), 1, fPtr);

    printf("Updated:\n");
    printHeader();
    printRecord(client);
}

// ADD
void newRecord(FILE *fPtr)
{
    struct clientData client = {0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account exists\n");
        return;
    }

    printf("Enter last name, first name, balance: ");
    scanf("%s %s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = acc;

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fwrite(&client, sizeof(client), 1, fPtr);

    printf("Account created\n");
}

// DELETE
void deleteRecord(FILE *fPtr)
{
    struct clientData client, blank = {0};
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Not found\n");
        return;
    }

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fwrite(&blank, sizeof(blank), 1, fPtr);

    printf("Deleted\n");
}

// SEARCH
void searchRecord(FILE *fPtr)
{
    struct clientData client;
    unsigned int acc;

    printf("Enter account number: ");
    scanf("%u", &acc);

    fseek(fPtr, (acc - 1) * sizeof(client), SEEK_SET);
    fread(&client, sizeof(client), 1, fPtr);

    if (client.acctNum == 0)
        printf("Not found\n");
    else
    {
        printHeader();
        printRecord(client);
    }
}

// DISPLAY
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);
    printHeader();

    while (fread(&client, sizeof(client), 1, fPtr))
    {
        if (client.acctNum != 0)
            printRecord(client);
    }
}

// TRANSFER
void transferMoney(FILE *fPtr)
{
    struct clientData from, to;
    unsigned int a1, a2;
    double amt;

    printf("From: "); scanf("%u", &a1);
    printf("To: "); scanf("%u", &a2);
    printf("Amount: "); scanf("%lf", &amt);

    fseek(fPtr, (a1 - 1) * sizeof(from), SEEK_SET);
    fread(&from, sizeof(from), 1, fPtr);

    fseek(fPtr, (a2 - 1) * sizeof(to), SEEK_SET);
    fread(&to, sizeof(to), 1, fPtr);

    if (from.acctNum == 0 || to.acctNum == 0)
    {
        printf("Invalid account\n");
        return;
    }

    if (from.balance < amt)
    {
        printf("Insufficient balance\n");
        return;
    }

    from.balance -= amt;
    to.balance += amt;

    fseek(fPtr, (a1 - 1) * sizeof(from), SEEK_SET);
    fwrite(&from, sizeof(from), 1, fPtr);

    fseek(fPtr, (a2 - 1) * sizeof(to), SEEK_SET);
    fwrite(&to, sizeof(to), 1, fPtr);

    printf("Transfer done\n");
}

// INTEREST (FIXED)
void applyInterest(FILE *fPtr)
{
    struct clientData client;
    float rate;

    printf("Enter interest rate: ");
    scanf("%f", &rate);

    rewind(fPtr);

    while (fread(&client, sizeof(client), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            client.balance += client.balance * (rate / 100);

            // FIX: reposition using SEEK_SET
            fseek(fPtr, (client.acctNum - 1) * sizeof(client), SEEK_SET);
            fwrite(&client, sizeof(client), 1, fPtr);

            // move forward again for next read
            fseek(fPtr, client.acctNum * sizeof(client), SEEK_SET);
        }
    }

    printf("Interest applied\n");
}