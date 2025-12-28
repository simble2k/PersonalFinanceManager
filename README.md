# 💰 Personal Finance Manager

![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![Raylib](https://img.shields.io/badge/GUI-Raylib%20v5.5-red.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS-lightgrey.svg)
![Status](https://img.shields.io/badge/Status-Active-brightgreen.svg)

> **A robust, lightweight, and efficient tool to master your financial life.** > Built from scratch using C++ and the Raylib graphical library.

---

## 📖 Overview

The **Personal Finance Manager** is a desktop application designed to help users track their income, expenses, and financial habits without the bloat of modern web apps. It features a custom-built backend for data management and a responsive graphical user interface.

Whether you are managing multiple wallets, tracking recurring bills, or analyzing your annual spending, this tool provides the data integrity and speed you need.

### 🎥 Watch the Demo
See the application in action:

[![Watch the Demo](https://img.youtube.com/vi/onqQoX757z0/0.jpg)](https://www.youtube.com/watch?v=onqQoX757z0)

[Click here to watch on YouTube](https://www.youtube.com/watch?v=onqQoX757z0)

---

## ✨ Key Features

* **📊 Interactive Dashboard:** View your total balance and individual wallet standings at a glance.
* **💸 Transaction Tracking:** Easily record Incomes and Expenses with dates, descriptions, and categories.
* **🔄 Recurring Automation:** Set up recurring payments (e.g., Rent, Salary). The system automatically checks dates and adds transactions when you open the app—never miss a bill again!
* **🗂️ Master Data Management:** Full control to Create, Edit, and Delete Wallets, Income Sources, and Expense Categories.
    * *Includes smart protection to prevent deleting data currently in use.*
* **📈 Reports & Statistics:**
    * Time-based summaries.
    * Wallet-specific breakdowns.
    * Annual overviews and category percentage splits.
* **💾 Secure Persistence:** All data is saved locally in binary format (`.dat`) for speed and privacy.

---

## 🚀 How to Run

Open run_app.exe and it should run without any problems.

---

## 🛠️ Project Structure

The codebase adheres to a modular architecture separating the backend logic from the frontend GUI.

```text
PersonalFinanceManager/
├── GUI/
│   ├── include/       # GUI Header files (App.h, Button.h, etc.)
│   └── src/           # GUI Implementation (Raylib integration)
├── include/           # Backend Headers (Wallet.h, Transaction.h)
├── src/               # Backend Logic (Data structures, File I/O)
├── saves/             # Binary data storage (Generated at runtime)
└── README.md
```
## 🏗️ Technology Stack
Language: C++17

GUI Library: Raylib v5.5

Data Structures: Custom Hash Tables (NameTable) & Dynamic Arrays.

Persistence: Custom Binary Serialization.

## 👥 Contributors
**Phạm Quốc Bảo**:

• Backend Architecture: Core logic for Transactions and
Recurring Manager (IncomeTransaction.cpp, ExpenseTransaction.cpp, recurring.cpp).

• Data Management: Designed NameTable structure and
Binary Persistence (Save/Load).

• System Reliability: Developed Console Interface (CLI),
Logic Validation, and Technical Documentation.

• Testing Support: Assisted with manual testing scenarios and bug reporting.

**Nguyễn Quốc Bảo**:

• User Interface (GUI): Implementation of the graphical
interface using Raylib.

• Component Design: Created reusable UI widgets (Buttons, Dropdowns, ScrollAreas).

• Visual Layout: Designed the Dashboard and Window
layouts.

**Lê Nguyễn Hải Đăng**:

• Statistic Reporting: Core logic for Statistic & Reporting features and helpers function (Developed statistic.cpp,
date.cpp)

• Testing Support: Assisted with testing Statistic & Reporting functions by Statistic Menu on Console.

<p align="center"> <sub>Made with ❤️ for CS160 Final Project</sub> </p>
