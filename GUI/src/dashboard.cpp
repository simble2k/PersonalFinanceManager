#include "dashboard.h"

// window settings
const int screenWidth = 1280;
const int screenHeight = 800;


void drawDashboard(const WalletsData& data, std::function<void(int)> onButtonClick) {
    // --- LAYOUT CONSTANTS ---
    const int PADDING = 60;
    const int LEFT_COLUMN_WIDTH = 800;
    const int RIGHT_COLUMN_START_X = screenWidth - 300; // Start of the Navigation Menu column , right column is 300px wide
    const int FONT_SIZE_TITLE = 36;
    const int FONT_SIZE_HEADER = 26;
    const int FONT_SIZE_NORMAL = 20;

    // !!!! x, y start from top-left corner of the window !!!! 

    // --- 1. Draw Title and Main Header ---

    // Use LayoutHelper to center the title
    const char* titleText = "PERSONAL FINANCE MANAGER";
    LayoutHelper::CenterText(titleText, FONT_SIZE_TITLE, DARKBLUE, screenWidth / 2.0f, PADDING);

    // Draw Separator ( below title )
    DrawLine(PADDING, PADDING + FONT_SIZE_TITLE + 15, screenWidth - PADDING, PADDING + FONT_SIZE_TITLE + 15, LIGHTGRAY);


    // --- 2. Draw Total Balance (Left Column, Top Section) ---

    // Formatting the total balance display
    string totalStr = "TOTAL BALANCE: ";
    LayoutHelper::MeasureAndDrawText(totalStr, FONT_SIZE_HEADER, DARKGRAY, PADDING, PADDING + 80);

    string totalValueStr = FormatCurrency(data.totalBalance); // [get the total balance data]
    int totalValueStrWidth = MeasureText(totalValueStr.c_str(), 40);
    // Measure width for right alignment

    // Highlight the value
    DrawText(totalValueStr.c_str(), 800 - totalValueStrWidth - 40, PADDING + 80, 40, LIME);

    // Draw Separator ( below total balance )
    DrawLine(PADDING, PADDING + 140, LEFT_COLUMN_WIDTH, PADDING + 140, LIGHTGRAY);

    //////at y = 200 = PADDING + 140

    // --- 3. Draw Wallet Balances List (Left Column, Main Section) ---
    LayoutHelper::CenterText("--- WALLET BALANCES DETAILS ---", FONT_SIZE_HEADER, DARKBLUE,(LEFT_COLUMN_WIDTH + PADDING) / 2.0f, PADDING + 150);

    int yOffset = PADDING + 200;
    // Starting Y position for the list

    const int LINE_HEIGHT = 35;

    // Table Headers
    DrawText("Wallet Name", PADDING + 20, yOffset, FONT_SIZE_NORMAL + 2, DARKGRAY);
    DrawText("Current Balance (VND)", LEFT_COLUMN_WIDTH - 250, yOffset, FONT_SIZE_NORMAL + 2, DARKGRAY);

    yOffset += 30;
    DrawLine(PADDING, yOffset, LEFT_COLUMN_WIDTH, yOffset, GRAY);

    yOffset += 10;

    // Loop through the Wallet Array (Custom Dynamic Array)
    for (int i = 0; i < data.walletCount; i++) { //  [get walletCount data]
        // Prepare Name and Balance strings
        string name = data.wallets[i].name; //  [get wallet name data]
        string balanceStr = FormatCurrency(data.wallets[i].balance); //  [get wallet balance data]

        // Color of balance text
        Color balanceColor = (data.wallets[i].balance == 0) ? GRAY : GREEN; // [get wallet balance data]

        // Draw Wallet Name
        DrawText(name.c_str(), PADDING + 20, yOffset, FONT_SIZE_NORMAL, BLACK);

        // Draw Balance (Aligned Right within the column)
        int balanceTextWidth = MeasureText(balanceStr.c_str(), FONT_SIZE_NORMAL);
        DrawText(balanceStr.c_str(), LEFT_COLUMN_WIDTH - balanceTextWidth - 40, yOffset, FONT_SIZE_NORMAL, balanceColor);

        yOffset += LINE_HEIGHT;
    }

	//draw column separator
	DrawLine(RIGHT_COLUMN_START_X - 20, PADDING + 150 , RIGHT_COLUMN_START_X - 20, screenHeight - PADDING, LIGHTGRAY);

    // --- 4. Draw Navigation Menu (Right Column) ---

    DrawText("--- FEATURES ---", RIGHT_COLUMN_START_X, PADDING + 145, FONT_SIZE_HEADER, DARKBLUE);

    const int BUTTON_WIDTH = 250;
    const int BUTTON_HEIGHT = 50;
    const int BUTTON_SPACING = 20;
    const int NUM_BUTTONS = 5;

    // Button data
    std::string buttonTexts[NUM_BUTTONS] = {
        "1. Add Transaction",
        "2. Reports & Statistics",
        "3. Master Data Center",
        "4. Add Recurring",
        "0. EXIT Application"
    };
    Color buttonColors[NUM_BUTTONS] = {BLUE, PURPLE, ORANGE, SKYBLUE, RED};
    Color hoverColors[NUM_BUTTONS] = {DARKBLUE, DARKPURPLE, DARKGRAY, DARKBLUE, MAROON};
    Color textColors[NUM_BUTTONS] = {WHITE, WHITE, BLACK, BLACK, WHITE};

    // Use CreateGrid for button positions (5 rows, 1 col, vertical stack)
    int totalRects = NUM_BUTTONS;
    Rectangle* buttonRects = LayoutHelper::CreateGrid(RIGHT_COLUMN_START_X, PADDING + 190, NUM_BUTTONS, 1, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_SPACING);

    // Create and store buttons in an array
    Button* buttons = new Button[totalRects];
    for (int i = 0; i < totalRects; ++i) {
        std::function<void()> onClickAction = nullptr;
        if (onButtonClick) {
            onClickAction = [onButtonClick, i]() { onButtonClick(i); };
        }
        buttons[i] = Button(buttonRects[i].x, buttonRects[i].y, buttonRects[i].width, buttonRects[i].height,
                            buttonTexts[i], buttonColors[i], hoverColors[i], textColors[i], onClickAction);
    }

    // Update and Draw buttons
    for (int i = 0; i < totalRects; ++i) {
        buttons[i].Update();
        buttons[i].Draw();
    }

    // Clean up
    delete[] buttonRects;
    delete[] buttons;
}
