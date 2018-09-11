#include "WisLTEBG96FILE.h"

WisLTEBG96FILE::WisLTEBG96FILE()
{

}

WisLTEBG96FILE::~WisLTEBG96FILE()
{
    cleanBuffer();
}

WisLTEBG96FILE::WisLTEBG96FILE(Stream &atserial, Stream &dserial) : WisLTEBG96TCPIP(atserial, dserial)
{

}

bool WisLTEBG96FILE::GetFliesSpace(unsigned long &free_bytes, unsigned long &total_bytes)
{
    char cmd[16]; 
    strcpy(cmd,FILE_SPACE_INFORMATION);
    strcat(cmd,"=\"UFS\"");

    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(": ");
        char *med_buf = searchChrBuffer(',');
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *med_buf = '\0';
        *end_buf = '\0';
        free_bytes = atol(sta_buf + 2);
        total_bytes = atol(med_buf + 1);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::GetFilesList(char *list)
{
    char cmd[16];
    strcpy(cmd, FILE_LIST_FILES);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(FILE_LIST_FILES);
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        strcpy(list, sta_buf);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::DeleteFiles(char *filename)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_DELETE_FILES);
    sprintf(buf, "=\"%s\"", filename);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::UploadFiles(char *filename, char *u_file)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_UPLOAD_FILES);
    sprintf(buf, "=\"%s\",%d", filename, strlen(u_file));
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 5)){
        if(sendDataAndCheck(u_file, RESPONSE_OK, RESPONSE_ERROR, 10)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96FILE::DownloadFiles(char *filename, char *d_file)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_DOWNLOAD_FILE);
    sprintf(buf, "=\"%s\"", filename);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 30)){
        char *end_buf = searchStrBuffer(FILE_DOWNLOAD_FILE);
        *end_buf = '\0';
        char *sta_buf = searchStrBuffer(RESPONSE_CONNECT);
        strcpy(d_file, sta_buf + strlen(RESPONSE_CONNECT) + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::OpenFile(char *filename, Open_File_Mode_t mode, unsigned int &file_index)
{
    char cmd[32],buf[32];
    strcpy(cmd, FILE_OPEN_FILE);
    sprintf(buf, "=\"%s\",%d", filename, mode);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 2)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        file_index = atoi(sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::ReadFile(unsigned int file_index, char *read_data)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_READ_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 10)){
        char *sta_buf = searchStrBuffer("\r\n");
        char *end_buf = searchStrBuffer("\r\nOK");
        *end_buf = '\0';
        strcpy(read_data, sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::WriteFile(unsigned int file_index, char *write_data)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_WRITE_FILE);
    sprintf(buf, "=%d,%d", file_index, strlen(write_data));
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_CONNECT, RESPONSE_ERROR, 5)){
        if(sendDataAndCheck(write_data, RESPONSE_OK, RESPONSE_ERROR, 10)){
            return true;
        }
    }
    return false;
}

bool WisLTEBG96FILE::CloseFlie(unsigned int file_index)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_CLOSE_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::SetFilePointer(unsigned int file_index, unsigned int offset, Pointer_Mode_t p_mode)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_SET_POSITION_FILE);
    sprintf(buf, "=%d,%d,%d", file_index, offset, p_mode);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::GetFilePointer(unsigned int file_index, unsigned int &offset)
{
    char cmd[32],buf[16];
    strcpy(cmd, FILE_GET_POSITION_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        char *sta_buf = searchStrBuffer(": ");
        char *end_buf = searchStrBuffer(RESPONSE_CRLF_OK);
        *end_buf = '\0';
        offset = atoi(sta_buf + 2);
        return true;
    }
    return false;
}

bool WisLTEBG96FILE::TruncateFile(unsigned int file_index)
{
    char cmd[16],buf[8];
    strcpy(cmd, FILE_TRUNCATE_FILE);
    sprintf(buf, "=%d", file_index);
    strcat(cmd, buf);
    if(sendAndSearch(cmd, RESPONSE_OK, RESPONSE_ERROR, 5)){
        return true;
    }
    return false;
}