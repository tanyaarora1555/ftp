# FTP Client
This is a Python Flask-based FTP client with a C backend for core FTP operations. It offers a user-friendly web interface to connect to FTP servers, browse directories, upload, and download files
## Features
1. Secure Login: Connect to FTP servers using an IP address, username, and password.
2. Session Management: Your credentials are saved securely in the Flask session for repeated use during your active session.
3. File Browse: List and navigate directories on the FTP server.
4. File Upload: Upload files from your local system to the FTP server.
5. File Download: Download files from the FTP server to your local system.
## Architecture
This application uses a hybrid architecture:
### Frontend (Python Flask):
* Handles web requests and serves HTML templates.
* Manages user sessions.
* Uses the subprocess module to interact with the C backend.

### Backend (C - Socket Programming):
* Implements core FTP functionalities such as:
    * `connect`: Establishes a connection to the FTP server.
    * `disconnect`: Closes the FTP connection.
    * `list directory`: Retrieves a list of files and directories on the server.
    * `connect_download`: Handles file downloads.
    * `connect_upload`: Handles file uploads.
    * `change directory`: Changes the current directory on the FTP server.
## Folder Structure

```bash
├── templates/
│ ├── login.html
│ ├── dashboard.html
│ └── file_browser.html
├── app.py # Main Python application
├── client.c # C source code for the client
```
# Getting Started
## Prerequisites
* Python 3.x
* Flask
* A C compiler (e.g., GCC)

## How To Run
1. Compile the C backend
```bash
gcc client.c -o client
```
Since the name of the backend is client we use this if the name is something else use that instaed of client
2. Run the app
```bash
python3 app.py
```
# Usage
1. Login:

    * On the login page, enter the IP address, username, and password of the FTP server you want to connect to.

    * Click the Connect button.

    * A success popup will appear upon successful connection, and you'll be redirected to the dashboard. Your credentials will be stored in the session for subsequent operations.
2. Dashboard Options:
From the dashboard, you have three main options:

    * Upload File to Server:

        * Click the "Browse" button to select a file from your local system.

        * Click the "Upload" button to transfer the selected file to the FTP server.

    * Download File from Server:

        * Direct Download: Enter the exact filename in the provided input field and click the "Download" button.

   * Browse Files on Server:

        * Browse and Download: Browse the files and folders on the server and click the "Download" button next to the desired file.

