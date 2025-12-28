#include "Dialog.h"

// Constructor
Dialog::Dialog()
    : rect_{0, 0, 0, 0}, title_(""), isOpen_(false),
      inputs_(nullptr), inputCount_(0), inputCapacity_(0),
      buttons_(nullptr), buttonCount_(0), buttonCapacity_(0) {}

Dialog::Dialog(float x, float y, float width, float height, const std::string& title)
    : rect_{x, y, width, height}, title_(title), isOpen_(false),
      inputs_(nullptr), inputCount_(0), inputCapacity_(0),
      buttons_(nullptr), buttonCount_(0), buttonCapacity_(0) {}
// Destructor
Dialog::~Dialog() {
    delete[] inputs_;
    delete[] buttons_;
}

// Add a text input to the dialog
void Dialog::AddTextInput(float startX, float startY, float w, float h, const std::string& placeholder) {
    // Resize if needed
    if (inputCount_ == inputCapacity_) {
        int newCap = inputCapacity_ == 0 ? 4 : inputCapacity_ * 2;
        TextInput* newArr = new TextInput[newCap];
        for (int i = 0; i < inputCount_; ++i) {
            newArr[i] = inputs_[i];
        }
        delete[] inputs_;
        inputs_ = newArr;
        inputCapacity_ = newCap;
    }
    // Add new input
    inputs_[inputCount_] = TextInput(rect_.x + startX, rect_.y + startY, w, h, placeholder);
    inputCount_++;
}

void Dialog::AddButton(float startX, float startY, float w, float h, const std::string& text,
                       Color normalColor, Color hoverColor, Color textColor,
                       std::function<void()> onClick) {
    // Resize if needed
    if (buttonCount_ == buttonCapacity_) {
        int newCap = buttonCapacity_ == 0 ? 4 : buttonCapacity_ * 2;
        Button* newArr = new Button[newCap];
        for (int i = 0; i < buttonCount_; ++i) {
            newArr[i] = buttons_[i];
        }
        delete[] buttons_;
        buttons_ = newArr;
        buttonCapacity_ = newCap;
    }
    // Add new button
    buttons_[buttonCount_] = Button(rect_.x + startX, rect_.y + startY, w, h, text,
                                    normalColor, hoverColor, textColor, onClick);
    buttonCount_++;
}

void Dialog::ResetInputs() {
    for (int i = 0; i < inputCount_; ++i) {
        inputs_[i].Clear();
    }
}

void Dialog::SetInputText(int index, const std::string& text) {
    if (index >= 0 && index < inputCount_) {
        inputs_[index].SetText(text);
    }
}

void Dialog::SetInputMaxLength(int index, int maxLen) {
    if (index >= 0 && index < inputCount_) {
        inputs_[index].SetMaxLength(maxLen);
    }
}

void Dialog::Update() {
    if (!isOpen_) return;

    // Update inputs and buttons
    for (int i = 0; i < inputCount_; ++i) {
        inputs_[i].Update();
    }
    for (int i = 0; i < buttonCount_; ++i) {
        buttons_[i].Update();
    }
}

void Dialog::Draw() {
    if (!isOpen_) return;

    // Draw background overlay, 50% transparent black
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    // Draw dialog box
    DrawRectangleRec(rect_, WHITE);
    DrawRectangleLinesEx(rect_, 3.0f, BLACK);

    // Draw title
    DrawText(title_.c_str(), rect_.x + 10, rect_.y + 10, 24, BLACK);

    // Draw inputs and buttons
    for (int i = 0; i < inputCount_; ++i) {
        inputs_[i].Draw();
    }
    for (int i = 0; i < buttonCount_; ++i) {
        buttons_[i].Draw();
    }
}

// Get text from a specific input (by index)
std::string Dialog::GetInputText(int index) const {
    if (index >= 0 && index < inputCount_) {
        return inputs_[index].GetText();
    }
    return "";
}