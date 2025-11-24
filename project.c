// Loan management

// Calculating Installments
void calculateInstallments(Loan* loan) 
{
    float monthlyRate = loan->interestRate / (12 * 100);
    
    int n = loan->repaymentMonths;
    loan->monthlyInstallment = (loan->loanAmount * monthlyRate * pow(1 + monthlyRate, n)) / (pow(1 + monthlyRate, n) - 1);
    loan->remainingAmount = loan->loanAmount;
}

// Checking for existing loan
int hasActiveLoan(int customerNumber) 
{
    FILE* loanFile = fopen(LOAN_FILE, "rb");
    if (!loanFile) 
    {
        printf("No loan records found.\n");
        return 0;
    }

    Loan loan;
    while (fread(&loan, sizeof(Loan), 1, loanFile)) 
    {
        if (loan.customerNumber == customerNumber && strcmp(loan.status, "Approved") == 0 && loan.remainingAmount > 0) 
        {
            fclose(loanFile);
            return 1;
        }
    }

    fclose(loanFile);
    return 0;
}

// Adding a loan
void applyForLoan() 
{
    Loan loan;
    FILE* loanFile = fopen(LOAN_FILE, "ab");
    if (!loanFile) 
    {
        perror("Error opening loan file");
        return;
    }

    loan.customerNumber = validatePositiveInt("Enter Customer Number");

    if (hasActiveLoan(loan.customerNumber)) 
    {
        printf("Customer already has an active loan. Loan application denied.\n");
        fclose(loanFile);
        return;
    }

    validateStringInput("Enter Loan Purpose", loan.purpose, sizeof(loan.purpose));
    
    loan.loanAmount = validatePositiveFloat("Enter Loan Amount (max 999,999)");

    while (loan.loanAmount > 999999) 
    {
        printf("Amount exceeds loan limit. Enter a value below 999,999.\n");
        loan.loanAmount = validatePositiveFloat("Enter Loan Amount (max 999,999)");
    }

    loan.repaymentMonths = validatePositiveInt("Enter Repayment Period (in months, max 60)");
    while (loan.repaymentMonths > 60) 
    {
        printf("Time period exceeds specified criteria.\n");
        loan.repaymentMonths = validatePositiveInt("Enter Repayment Period (in months, max 60)");
    }

    loan.interestRate = 5.0;

    loan.creditScore = validatePositiveInt("Enter Credit Score");
    if (loan.creditScore < 600) 
    {
        strcpy(loan.status, "Rejected");
        loan.monthlyInstallment = 0;
        loan.remainingAmount = 0;
        printf("Loan application rejected due to low credit score.\n");
    } 
    else 
    {
        calculateInstallments(&loan);
        strcpy(loan.status, "Approved");
        printf("Loan Approved.\nMonthly Installment: %.2f\n", loan.monthlyInstallment);
    }

    fwrite(&loan, sizeof(Loan), 1, loanFile);
    fclose(loanFile);
    printf("Loan application processed successfully.\n");
}

// Displaying loans
void displayLoans() 
{
    FILE* file = fopen(LOAN_FILE, "rb");
    Loan loan;

    if (!file) 
    {
        printf("No loans found.\n");
        return;
    }

    header("Loan Details");
    printf("%-20s | %-20s | %-20s | %-20s | %-20s | %-20s | %-20s\n", "Customer No.", "Purpose", "Amount", "Months", "Interest","Installment", "Status");

    while (fread(&loan, sizeof(Loan), 1, file)) 
    {
        printf("%-20d | %-20s | %-20.2f | %-20d | %-20.2f | %-20.2f | %-20s\n", loan.customerNumber, loan.purpose, loan.loanAmount, loan.repaymentMonths, loan.interestRate, loan.monthlyInstallment, loan.status);
    }
    fclose(file);
}

// Updating loan details
void updateLoanDetails() 
{
    FILE* loanFile = fopen(LOAN_FILE, "rb+");
    if (!loanFile) 
    {
        perror("Error opening loan file");
        return;
    }

    int customerNumber, found = 0;
    float paymentAmount;

    customerNumber = validatePositiveInt("Enter Customer Number to Update Loan Details");

    Loan loan;
    while (fread(&loan, sizeof(Loan), 1, loanFile)) 
    {
        if (loan.customerNumber == customerNumber) 
        {
            found = 1;
            printf("Current Loan Details:\n");
            printf("Loan Amount: %.2f\n", loan.loanAmount);
            printf("Remaining Amount: %.2f\n", loan.remainingAmount);

            do 
            {
                paymentAmount = validatePositiveFloat("Enter Payment Amount (0 to cancel)");
                
                if (paymentAmount == 0) 
                {
                    printf("Payment cancelled.\n");
                    break;
                }
                
                if (paymentAmount > loan.remainingAmount) 
                {
                    printf("Payment exceeds remaining amount. Please try again.\n");
                } 
                else 
                {
                	loan.remainingAmount -= paymentAmount;

                    fseek(loanFile, -sizeof(Loan), SEEK_CUR);
                    fwrite(&loan, sizeof(Loan), 1, loanFile);
                    printf("Payment recorded successfully.\nRemaining Amount: %.2f\n", loan.remainingAmount);

                    if (loan.remainingAmount == 0) 
                    {
                        printf("Loan fully repaid. Congratulations!\n");
                        strcpy(loan.status, "Completed");
                    }
                    break;
                }
            } 
            while (1);
            break;
        }
    }

    if (!found) 
    {
        printf("Loan not found for customer number %d.\n", customerNumber);
    }

    fclose(loanFile);
}

// Main Menu
void menu() 
{
    int choice;
    do 
	{
        header("Main Menu");
        printf("1. Add Customer\n");
        printf("2. Display All Customers\n");
        printf("3. Search Customer\n");
        printf("4. Apply for a Loan\n");
        printf("5. Display All Loans\n");
        printf("6. Update Loan Payment\n");
		printf("7. Exit\n");
        printSeparator();
        
        choice = validatePositiveInt("Enter your choice");
        
        if (choice < 1 || choice > 7)
        {
        	printf("Invalid choice! Try again.\n");
        	continue;
		}
		
		int confirm;
		
		printf("You selected option %d. Do you want to select another option? \nTo go back, enter 1 or any random number to continue. \nChoice: ", choice);
        scanf("%d", &confirm); 
        clearInputBuffer();

        if (confirm == 1) 
        {
            printf("Returning to the main menu...\n");
            continue;  
		}
        else
        {
        	printSeparator();
        	switch (choice) 
			{
            	case 1: 
				{
            		addCustomer();
					break;
				}
       	   		case 2: 
				{
    	        	displayCustomers();
					break;
				}
   	         	case 3: 
				{
       		     	searchCustomerByNumber();
					break;
				}
           		case 4: 
				{
            		applyForLoan();
					break;
				}
            	case 5: 
				{
        	   	 	displayLoans();
					break;
				}
         	  	case 6: 
				{
            		updateLoanDetails();
					break;
				}
				case 7: 
				{
            		header("Thank You for Using the System");
					break;
				}
            	default: 
				{
            		printf("Invalid choice. Please try again.\n");
				}
			}
		}
    } 
	while (choice != 7);
}

// Main function
int main() 
{
	system("COLOR 70");
	header("Bank Loan Managment System");
    
	menu();
    
	return 0;
}
