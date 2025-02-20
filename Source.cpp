// Include the main Windows header file which contains declarations for the Windows API functions, macros, and data types.
#include <windows.h>

// Include the TCHAR header for generic text mappings. This lets you write code that works with either ANSI or Unicode.
#include <tchar.h>

// Include the C++ string header to use std::basic_string (which will work with TCHAR).
#include <string>

// Define unique integer IDs for the controls (widgets) we will create.
// These are used to identify controls when processing messages.
#define IDC_CHAT_DISPLAY 101
#define IDC_CHAT_INPUT   102
#define IDC_SEND_BUTTON  103

// Global variables to store the window handles for our controls. A window handle (HWND) is an identifier for a window/control.
HWND hChatDisplay, hChatInput, hSendButton;

// WindowProc is the callback function that handles messages sent to our main window.
// Every time an event occurs (like creating the window, a button click, or closing the window), this function is called.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // We use a switch statement to handle different types of messages (uMsg).
    switch (uMsg)
    {
        // WM_CTLCOLORSTATIC and WM_CTLCOLOREDIT are messages sent when a static control (like a label) or an edit control (text box) is about to be drawn.
        // We handle them together because we want to set the text color and background color for our edit controls.
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    {
        // The wParam parameter is a handle to the device context (HDC) for the control.
        // The device context is like a canvas used for drawing text and shapes.
        HDC hdc = (HDC)wParam;
        // Set the text color to green (RGB(0, 255, 0)).
        SetTextColor(hdc, RGB(0, 255, 0));
        // Set the background color to black (RGB(0, 0, 0)).
        SetBkColor(hdc, RGB(0, 0, 0));
        // Return a handle to a black brush. This tells Windows to fill the background of the control with black.
        return (LRESULT)GetStockObject(BLACK_BRUSH);
    }

    // WM_CREATE is sent when the window is first created.
    case WM_CREATE:
    {
        // Create a multi-line, read-only edit control for displaying chat messages.
        // CreateWindowEx is used to create an extended window (a control in this case).
        hChatDisplay = CreateWindowEx(
            WS_EX_CLIENTEDGE,          // Extended window style: gives the control a sunken edge look.
            _T("EDIT"),                // The predefined window class for an edit control.
            _T(""),                    // Initial text in the control (empty in this case).
            WS_CHILD | WS_VISIBLE |    // WS_CHILD: it's a child window of our main window.
            ES_MULTILINE |             // ES_MULTILINE: allows multiple lines of text.
            ES_AUTOVSCROLL |           // ES_AUTOVSCROLL: automatically scrolls vertically as text is added.
            ES_READONLY |              // ES_READONLY: user cannot edit this control.
            WS_VSCROLL,               // WS_VSCROLL: gives the control a vertical scrollbar.
            10, 10, 460, 250,          // x position, y position, width, height of the control.
            hwnd,                     // Parent window handle (our main window).
            (HMENU)IDC_CHAT_DISPLAY,  // Control ID.
            ((LPCREATESTRUCT)lParam)->hInstance,  // Instance handle from the creation structure.
            NULL                      // No additional parameters.
        );

        // Create an edit control for text input.
        hChatInput = CreateWindowEx(
            WS_EX_CLIENTEDGE,          // Extended style for a 3D look.
            _T("EDIT"),                // Class name for an edit control.
            _T(""),                    // Start with empty text.
            WS_CHILD | WS_VISIBLE |    // Child and visible.
            ES_AUTOHSCROLL,            // Automatically scroll horizontally if text is long.
            10, 270, 360, 25,          // x, y, width, height for the input control.
            hwnd,                     // Parent window is our main window.
            (HMENU)IDC_CHAT_INPUT,    // Control ID.
            ((LPCREATESTRUCT)lParam)->hInstance,  // Instance handle.
            NULL                      // No additional data.
        );

        // Create a "Send" button that the user can click to send their message.
        hSendButton = CreateWindow(
            _T("BUTTON"),             // Predefined class name for buttons.
            _T("Send"),               // The text displayed on the button.
            WS_TABSTOP | WS_VISIBLE |  // WS_TABSTOP: allows keyboard navigation to the button.
            WS_CHILD | BS_DEFPUSHBUTTON,  // BS_DEFPUSHBUTTON: style for a default push button.
            380, 270, 90, 25,          // x, y, width, height for the button.
            hwnd,                     // Parent window handle.
            (HMENU)IDC_SEND_BUTTON,   // Control ID.
            ((LPCREATESTRUCT)lParam)->hInstance,  // Instance handle.
            NULL                      // No additional parameters.
        );
        break;  // Break out of the WM_CREATE case.
    }

    // WM_COMMAND is sent when the user interacts with a control, such as clicking the Send button.
    case WM_COMMAND:
    {
        // LOWORD(wParam) extracts the control ID from wParam.
        // Check if the Send button was clicked by comparing the control ID.
        if (LOWORD(wParam) == IDC_SEND_BUTTON)
        {
            // Create a buffer to store the text from the input box.
            TCHAR inputBuffer[256];
            // GetWindowText copies the text from the hChatInput control into inputBuffer.
            // sizeof(inputBuffer) / sizeof(TCHAR) calculates the number of TCHAR elements in the array.
            GetWindowText(hChatInput, inputBuffer, sizeof(inputBuffer) / sizeof(TCHAR));

            // Check if there is any text entered (i.e., the length is greater than 0).
            if (_tcslen(inputBuffer) > 0)
            {
                // Get the current length of the text in the chat display.
                int currentLen = GetWindowTextLength(hChatDisplay);
                // Create a basic_string (using TCHAR) to hold the existing text.
                // We add one extra character for the terminating null character.
                std::basic_string<TCHAR> currentText(currentLen + 1, _T('\0'));
                // Retrieve the text from the chat display control.
                GetWindowText(hChatDisplay, &currentText[0], currentLen + 1);

                // Append the new message (from the input buffer) to the current text.
                currentText.append(inputBuffer);
                // Append a carriage return and newline to move to the next line.
                currentText.append(_T("\r\n"));

                // Update the chat display control with the new text.
                SetWindowText(hChatDisplay, currentText.c_str());

                // Clear the input box by setting its text to an empty string.
                SetWindowText(hChatInput, _T(""));
            }
        }
        break;  // Break out of the WM_COMMAND case.
    }

    // WM_DESTROY is sent when the window is being closed.
    case WM_DESTROY:
        // Post a quit message to exit the message loop.
        PostQuitMessage(0);
        return 0;  // Return 0 to indicate we handled the message.
    }

    // For all other messages, call the default window procedure.
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// _tWinMain is the entry point for a Windows GUI application that supports both Unicode and ANSI.
// It is the generic version of the WinMain function.
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    // Define a constant string for the window class name.
    const TCHAR CLASS_NAME[] = _T("ChatWindowClass");

    // Create and zero-initialize a WNDCLASS structure which holds the window class attributes.
    WNDCLASS wc = {};
    // Set the window procedure function for this class.
    wc.lpfnWndProc = WindowProc;
    // Set the instance handle (provided by the OS).
    wc.hInstance = hInstance;
    // Set the name of the window class.
    wc.lpszClassName = CLASS_NAME;
    // Set the background brush to a black brush so the window background is black.
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    // Register the window class with Windows.
    RegisterClass(&wc);

    // Create the main application window.
    // CreateWindowEx allows for extended styles; here, we are not using any extra extended styles.
    HWND hwnd = CreateWindowEx(
        0,                              // No extended window styles.
        CLASS_NAME,                     // The window class name we just registered.
        _T("Basic Chat Interface"),     // The window title that appears in the title bar.
        WS_OVERLAPPEDWINDOW,            // The window style that includes title bar, border, minimize and maximize buttons, and a system menu.
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 350,  // Position (x, y) and size (width, height) of the window.
        NULL,       // No parent window (this is the main window).
        NULL,       // No menu.
        hInstance,  // Instance handle.
        NULL        // No additional creation data.
    );

    // Check if the window was created successfully.
    if (hwnd == NULL)
    {
        return 0;  // Exit if window creation failed.
    }

    // Show the window on the screen.
    ShowWindow(hwnd, nCmdShow);

    // Enter the message loop. This loop retrieves messages from the OS and dispatches them to our WindowProc.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); // Translates virtual-key messages into character messages.
        DispatchMessage(&msg);  // Dispatches the message to the window procedure (WindowProc).
    }

    // Return the exit code when the message loop ends.
    return (int)msg.wParam;
}
