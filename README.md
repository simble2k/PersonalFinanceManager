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

### Option 1: Run Pre-built Binary (Easiest)

#### Prebuilt (Windows)
- Download `dist/PersonalFinanceManager-Windows.zip` from the repository (or Releases).
- Extract and run `PersonalFinanceManager-Windows.exe`.
- Ensure `raylib.dll` and `glfw3.dll` are in the same folder as the `.exe` (both bundled in the prebuild ZIP).
- Requires Microsoft Visual C++ Redistributable (2015–2022). If missing, install from https://aka.ms/vs/17/release/vc_redist.x64.exe.

#### Prebuilt (macOS)
- Download the prebuilt executable from `prebuild/PersonalFinanceManager` in the repository.
- Make it executable: `chmod +x PersonalFinanceManager`.
- Run: `./PersonalFinanceManager`.
- If blocked by Gatekeeper, allow under System Settings → Privacy & Security → "Open Anyway".

### Option 2: Build from Source

To build the project yourself, you will need a C++17 compatible compiler and the Raylib library.

#### 🪟 Windows (MinGW / MSYS2)

1.  **Prerequisites:** Install MinGW (`g++`) and download the [Raylib](https://www.raylib.com/) library.
2.  **Compile:** Run the following command in your terminal (adjust paths to your Raylib installation):
    ```powershell
    g++ -std=c++17 GUI/mainGUI.cpp GUI/src/*.cpp src/*.cpp \
    -Iinclude -IGUI/include \
    -o finance_app.exe \
    -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -static
    ```
3.  **Run:** `./finance_app.exe`

#### 🍎 macOS

1.  **Prerequisites:** Install Raylib via Homebrew:
    ```bash
    brew install raylib
    ```
2.  **Compile:**
    ```bash
    clang++ -std=c++17 GUI/mainGUI.cpp GUI/src/*.cpp src/*.cpp \
    -Iinclude -IGUI/include \
    -o finance_app \
    -lraylib -framework IOKit -framework Cocoa -framework OpenGL
    ```
3.  **Run:** `./finance_app`

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
