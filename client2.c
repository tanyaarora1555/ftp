#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SERVER_PORT 21
#define BUFFER_SIZE 4096
#define LOCAL_FOLDER_DOWNLOAD "/home/tanya/Downloads/"

int sock;

void read_response(int sock) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    memset(buffer, 0, sizeof(buffer));
    bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("read");
        return;
    } else if (bytes_read == 0) {
        printf("Connection closed by server.\n");
        return;
    }

    buffer[bytes_read] = '\0';
    printf("%s", buffer);
}

void send_command(int sock, const char *cmd) {
    send(sock, cmd, strlen(cmd), 0);
    read_response(sock);
}

int create_data_connection(char *ip, int port) {
    int data_sock;
    struct sockaddr_in data_addr;

    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (data_sock < 0) {
        perror("Data socket error");
        exit(1);
    }

    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &data_addr.sin_addr);

    if (connect(data_sock, (struct sockaddr*)&data_addr, sizeof(data_addr)) < 0) {
        perror("Data connect failed");
        exit(1);
    }

    return data_sock;
}

int enter_passive_mode(int sock, char *ip, int *port) {
    char buffer[BUFFER_SIZE], *ptr;
    int ip1, ip2, ip3, ip4, p1, p2;

    send(sock, "PASV\r\n", 6, 0);
    int bytes = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) return -1;
    buffer[bytes] = '\0';
    printf("%s", buffer);

    ptr = strchr(buffer, '(');
    if (!ptr) return -1;

    sscanf(ptr+1, "%d,%d,%d,%d,%d,%d", &ip1, &ip2, &ip3, &ip4, &p1, &p2);
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    *port = p1 * 256 + p2;

    return 0;
}
// List directory
void list_dir(int sock) {
    char ip[32];
    int port, data_sock;
    if (enter_passive_mode(sock, ip, &port) == -1) {
        printf("Failed to enter passive mode\n");
        return;
    }

    data_sock = create_data_connection(ip, port);
    send_command(sock, "LIST\r\n");

    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = read(data_sock, buffer, sizeof(buffer)-1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    close(data_sock);
    read_response(sock);
}

void upload_file(int sock, const char *filepath) {
    char ip[32], cmd[256];
    int port, data_sock, file_fd;


    char *filename = strrchr(filepath, '/');
    if (filename) filename++; else filename = (char *)filepath;

    file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        perror("File open");
        return;
    }

    char *server_folder = "./Downloads/";
    if (server_folder && strlen(server_folder) > 0) {
        sprintf(cmd, "CWD %s\r\n", server_folder);
        send_command(sock, cmd);
    }

   
    if (enter_passive_mode(sock, ip, &port) == -1) {
       
        close(file_fd);
        return;
    }

   
    data_sock = create_data_connection(ip, port);

    sprintf(cmd, "STOR %s\r\n", filename);
    
    send_command(sock, cmd);

    
    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = read(file_fd, buffer, sizeof(buffer))) > 0) {
        send(data_sock, buffer, bytes, 0);
    }

    close(file_fd);
    close(data_sock);

}
// Download file
void download_file(int sock, const char *filename) {
    char ip[32], cmd[256], path[512];
    int port, data_sock, file_fd;

    sprintf(path, "%s%s", LOCAL_FOLDER_DOWNLOAD, filename);
    file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) {
        perror("File open");
        return;
    }

    if (enter_passive_mode(sock, ip, &port) == -1) {
        printf("Passive mode failed\n");
        return;
    }

    data_sock = create_data_connection(ip, port);
    sprintf(cmd, "RETR %s\r\n", filename);
    send_command(sock, cmd);

    char buffer[BUFFER_SIZE];
    int bytes;
    while ((bytes = read(data_sock, buffer, sizeof(buffer))) > 0) {
        write(file_fd, buffer, bytes);
    }

    close(file_fd);
    close(data_sock);
    read_response(sock);
}

int ftp_connect(char* ip, char* username, char* pass) {
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, ip, &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }

    read_response(sock);

    sprintf(buffer, "USER %s\r\n", username);
    send_command(sock, buffer);

    sprintf(buffer, "PASS %s\r\n", pass);
    send_command(sock, buffer);

    return 0;
}
void change_directory(int sock, const char *folder) {
    char cmd[256];
    sprintf(cmd, "CWD %s\r\n", folder);
    send_command(sock, cmd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n connect <ip> <user> <pass>\n upload <file>\n connect_upload <ip> <user> <pass> <file>\n quit\n list\n download <file>\n");
        return 1;
    }

    if (strcmp(argv[1], "connect") == 0) {
        ftp_connect(argv[2], argv[3], argv[4]);
    }
    else if (strcmp(argv[1], "upload") == 0) {
        upload_file(sock, argv[2]);
    }
    else if (strcmp(argv[1], "connect_upload") == 0) {
        ftp_connect(argv[2], argv[3], argv[4]);
        upload_file(sock, argv[5]);
    }
    else if (strcmp(argv[1], "connect_download") == 0) {
    ftp_connect(argv[2], argv[3], argv[4]);
    download_file(sock, argv[5]);
	}
    else if (strcmp(argv[1], "connect_list") == 0) {
    ftp_connect(argv[2], argv[3], argv[4]);
    list_dir(sock);
	}
    else if (strcmp(argv[1], "connect_list_folder") == 0) {
    ftp_connect(argv[2], argv[3], argv[4]);
    change_directory(sock, argv[5]); // folder path
    list_dir(sock);
      }
      
    else if (strcmp(argv[1], "quit") == 0) {
        send_command(sock, "QUIT\r\n");
    }
    else {
        printf("Unknown command.\n");
    }

    close(sock);
    return 0;
}

