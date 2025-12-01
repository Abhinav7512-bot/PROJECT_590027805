#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LOAN_FILE "loan.txt"
#define PAYMENT_FILE "payments.txt"

typedef struct {
    char name[50];
    float amount;
    float rate;
    int tenure;
    float emi;
} Loan;

// ---------------- FUNCTION DECLARATIONS ----------------
int checkEligibility(float salary, int credit);
float calculateEMI(float principal, float rate, int months);
void applyLoan();
void generateSchedule();
void payInstallment();
void showHistory();

// --------------------------------------------------------

int checkEligibility(float salary, int credit) {
    if (salary < 25000) return 0;
    if (credit < 650) return 0;
    return 1;
}

float calculateEMI(float P, float annualRate, int months) {
    float r = annualRate / (12 * 100);
    return P * r * pow(1+r, months) / (pow(1+r, months) - 1);
}

void applyLoan() {
    Loan L;
    float salary; 
    int credit;

    printf("Enter your name: ");
    scanf("%s", L.name);

    printf("Enter monthly salary: ");
    scanf("%f", &salary);

    printf("Enter credit score: ");
    scanf("%d", &credit);

    if (!checkEligibility(salary, credit)) {
        printf("\n❌ Loan Rejected due to low salary or credit score!\n");
        return;
    }

    printf("Enter loan amount: ");
    scanf("%f", &L.amount);

    printf("Enter annual interest rate: ");
    scanf("%f", &L.rate);

    printf("Enter tenure (months): ");
    scanf("%d", &L.tenure);

    L.emi = calculateEMI(L.amount, L.rate, L.tenure);

    FILE *f = fopen(LOAN_FILE, "w");
    fprintf(f, "%s %.2f %.2f %d %.2f\n", 
            L.name, L.amount, L.rate, L.tenure, L.emi);
    fclose(f);

    f = fopen(PAYMENT_FILE, "w");
    for (int i = 1; i <= L.tenure; i++)
        fprintf(f, "%d pending\n", i);
    fclose(f);

    printf("\n✅ Loan Approved! EMI = %.2f\n", L.emi);
}

void generateSchedule() {
    FILE *f = fopen(LOAN_FILE, "r");
    if (!f) {
        printf("No loan found!\n");
        return;
    }

    Loan L;
    fscanf(f, "%s %f %f %d %f", 
           L.name, &L.amount, &L.rate, &L.tenure, &L.emi);
    fclose(f);

    float balance = L.amount;
    float r = L.rate / (12 * 100);

    printf("\nMonth | Interest | Principal | Balance\n");

    for (int i = 1; i <= L.tenure; i++) {
        float interest = balance * r;
        float principal = L.emi - interest;
        balance -= principal;

        printf("%3d   | %.2f     | %.2f      | %.2f\n", 
               i, interest, principal, balance);
    }
}

void payInstallment() {
    int installment;
    printf("Enter installment number to pay: ");
    scanf("%d", &installment);

    FILE *f = fopen(PAYMENT_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    int num; char status[20];
    while (fscanf(f, "%d %s", &num, status) != EOF) {
        if (num == installment)
            fprintf(temp, "%d paid\n", num);
        else
            fprintf(temp, "%d %s\n", num, status);
    }

    fclose(f);
    fclose(temp);

    remove(PAYMENT_FILE);
    rename("temp.txt", PAYMENT_FILE);

    printf("\n✅ Installment %d marked as PAID!\n", installment);
}

void showHistory() {
    FILE *f = fopen(PAYMENT_FILE, "r");
    if (!f) {
        printf("No history found!\n");
        return;
    }

    printf("\nInstallment Status:\n");
    int paid = 0, pending = 0;
    int num; char status[20];

    while (fscanf(f, "%d %s", &num, status) != EOF) {
        printf("Month %d : %s\n", num, status);
        if (strcmp(status, "paid") == 0) paid++;
        else pending++;
    }

    fclose(f);

    printf("\nTotal Paid: %d\n", paid);
    printf("Pending: %d\n", pending);
}

int main() {
    int choice;

    while (1) {
        printf("\n===== BANK LOAN MANAGEMENT SYSTEM =====\n");
        printf("1. Apply Loan\n");
        printf("2. Generate EMI Schedule\n");
        printf("3. Pay Installment\n");
        printf("4. View Loan History\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: applyLoan(); break;
            case 2: generateSchedule(); break;
            case 3: payInstallment(); break;
            case 4: showHistory(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}
