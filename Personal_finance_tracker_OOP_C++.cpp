#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <fstream> // Required for file handling

using namespace std;

// ====================== BASE CLASS ======================
class Transaction {
protected:
    string date;
    string category;
    double amount;
    string description;

public:
    Transaction(string d, string c, double a, string desc)
        : date(d), category(c), amount(a), description(desc) {}

    virtual void display() {
        cout << "Date: " << date
             << " | Category: " << category
             << " | Amount: " << amount
             << " | Description: " << description;
    }

    double getAmount() const { return amount; }
    string getCategory() const { return category; }

    virtual ~Transaction() {}
};

// ====================== DERIVED CLASS: INCOME ======================
class Income : public Transaction {
private:
    string source;

public:
    Income(string d, string c, double a, string desc, string s)
        : Transaction(d, c, a, desc), source(s) {}

    void display() override {
        cout << "[INCOME] ";
        Transaction::display();
        cout << " | Source: " << source << endl;
    }
};

// ====================== DERIVED CLASS: EXPENSE ======================
class Expense : public Transaction {
private:
    string paymentMethod;

public:
    Expense(string d, string c, double a, string desc, string pay)
        : Transaction(d, c, a, desc), paymentMethod(pay) {}

    void display() override {
        cout << "[EXPENSE] ";
        Transaction::display();
        cout << " | Payment Method: " << paymentMethod << endl;
    }
};

// ====================== BASE CLASS: SUGGESTION ======================
class Suggestion {
public:
    virtual void suggest(double income, double expense) {
        cout << "General Suggestion: Track your expenses regularly.\n";
    }

    virtual ~Suggestion() {}
};

// ====================== DERIVED CLASS: SMART SUGGESTION ======================
class SmartSuggestion : public Suggestion {
public:
    void suggest(double income, double expense) override {
        cout << "\n--- Smart Financial Suggestion ---\n";
        if (expense > income)
            cout << "Warning: You are spending more than you earn!\n";
        else if (expense > 0.8 * income)
            cout << "Caution: Your spending is above 80% of your income.\n";
        else if (expense < 0.5 * income)
            cout << "Good: You're saving a healthy portion of your income.\n";
        else
            cout << "Keep monitoring your expenses.\n";
    }
};

// ====================== EXPENSE MANAGER ======================
class ExpenseManager {
private:
    vector<Transaction*> records;

public:
    void addTransaction(Transaction* t) {
        records.push_back(t);
    }

    void showAll() {
        cout << "\n======= ALL TRANSACTIONS =======\n";
        for (auto t : records)
            t->display();
    }

    double totalIncome() const {
        double total = 0;
        for (auto t : records) {
            const Income* inc = dynamic_cast<const Income*>(t);
            if (inc)
                total += inc->getAmount();
        }
        return total;
    }

    double totalExpense() const {
        double total = 0;
        for (auto t : records) {
            const Expense* exp = dynamic_cast<const Expense*>(t);
            if (exp)
                total += exp->getAmount();
        }
        return total;
    }

    // Function to display and save expense breakdown by category
    void showExpenseBreakdown() const {
        double total_exp = totalExpense();
        if (total_exp == 0) {
            cout << "No expenses recorded yet.\n";
            return;
        }

        map<string, double> category_totals;
        for (auto t : records) {
            const Expense* exp = dynamic_cast<const Expense*>(t);
            if (exp) {
                category_totals[exp->getCategory()] += exp->getAmount();
            }
        }

        cout << fixed << setprecision(2);
        cout << "\n--- Expense Breakdown by Category ---\n";
        for (const auto& pair : category_totals) {
            double percentage = (pair.second / total_exp) * 100;
            cout << pair.first << ": " << pair.second << " (" << percentage << "%)\n";
        }
    }

    // Function to save expense data to a file for the Python script
    void saveExpenseData() const {
        double total_exp = totalExpense();
        if (total_exp == 0) {
            return;
        }

        map<string, double> category_totals;
        for (auto t : records) {
            const Expense* exp = dynamic_cast<const Expense*>(t);
            if (exp) {
                category_totals[exp->getCategory()] += exp->getAmount();
            }
        }

        // Write data to a simple text file
        ofstream dataFile("expense_data.txt");
        if (dataFile.is_open()) {
            for (const auto& pair : category_totals) {
                dataFile << pair.first << "," << pair.second << "\n";
            }
            dataFile.close();
            cout << "Expense data saved to 'expense_data.txt' for charting.\n";
        } else {
            cout << "Error: Unable to open file for writing.\n";
        }
    }

    ~ExpenseManager() {
        for (auto t : records)
            delete t;
    }
};

// ====================== MAIN FUNCTION ======================
int main() {
    ExpenseManager manager;
    Suggestion* sugg = new SmartSuggestion();

    int choice;
    do {
        cout << "\n========= PERSONAL FINANCE MANAGER =========";
        cout << "\n1. Add Income";
        cout << "\n2. Add Expense";
        cout << "\n3. Show All Transactions";
        cout << "\n4. View Summary & Suggestions";
        cout << "\n5. Show Expense Breakdown & Generate Chart Data"; // Revised option
        cout << "\n6. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        cin.ignore(); // clear input buffer

        if (choice == 1) {
            string date, category, desc, source;
            double amount;
            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, date);
            cout << "Enter Category: ";
            getline(cin, category);
            cout << "Enter Amount: ";
            cin >> amount;
            cin.ignore();
            cout << "Enter Description: ";
            getline(cin, desc);
            cout << "Enter Source (e.g., Salary, Freelance): ";
            getline(cin, source);

            Transaction* t = new Income(date, category, amount, desc, source);
            manager.addTransaction(t);
            cout << "Income added successfully.\n";
        }

        else if (choice == 2) {
            string date, category, desc, method;
            double amount;
            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, date);
            cout << "Enter Category: ";
            getline(cin, category);
            cout << "Enter Amount: ";
            cin >> amount;
            cin.ignore();
            cout << "Enter Description: ";
            getline(cin, desc);
            cout << "Enter Payment Method (Cash/Card/UPI): ";
            getline(cin, method);

            Transaction* t = new Expense(date, category, amount, desc, method);
            manager.addTransaction(t);
            cout << "Expense added successfully.\n";
        }

        else if (choice == 3) {
            manager.showAll();
        }

        else if (choice == 4) {
            double inc = manager.totalIncome();
            double exp = manager.totalExpense();
            cout << "\n===== FINANCIAL SUMMARY =====\n";
            cout << "Total Income: " << inc << "\n";
            cout << "Total Expense: " << exp << "\n";
            cout << "Net Savings: " << (inc - exp) << "\n";
            sugg->suggest(inc, exp);
        }

        else if (choice == 5) {
            manager.showExpenseBreakdown();
            manager.saveExpenseData();
            cout << "\nTo view the pie chart, run the Python script `generate_chart.py`.\n";
        }

        else if (choice == 6) {
            cout << "\nThank you for using Personal Finance Manager!\n";
        }

        else {
            cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 6);

    delete sugg;
    return 0;
}
