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
*Handles web requests and serves HTML templates.
*Manages user sessions.
*Uses the subprocess module to interact with the C backend.
### Backend (C - Socket Programming):
*Implements core FTP functionalities such as:
-connect: Establishes a connection to the FTP server.
-disconnect: Closes the FTP connection.
-list directory: Retrieves a list of files and directories on the server.
-connect_download: Handles file downloads.
-connect_upload: Handles file uploads.
-change directory: Changes the current directory on the FTP server.
