#include "FileUtilEstoque.h"
#include "Estoque.h"
#include "stdio.h"
#include "Windows.h"
#include "Shlobj.h"
#include "stdlib.h"
#include "string.h"

char* getBinPath();
char* getFilePath();
void writePath(FILE* f, const char* path);

FILE* estq_create(const char* fileName){
    char buffer[200] = "";

    char* path = getBinPath();
    if(path == NULL){
        return NULL;
    }

    strcat(buffer, path);
    strcat(buffer, "\\");
    strcat(buffer, fileName);
    strcat(buffer, ".estq");

    FILE* f = fopen(buffer, "wb+");
    if(f == NULL){
        return NULL;
    }

    writePath(f, buffer);

    printf("Arquivo criado em: %s\n", buffer);

    return f;
}

FILE* estq_open(){
    char* path = getFilePath();
    FILE* f = fopen(path, "rb+");
    if(f == NULL){
        return NULL;
    }

    writePath(f, path);

    printf("Arquivo aberto em: %s\n", path);
    return f;
}


int estq_close(FILE* f){
    if(fclose(f) == 0){
        return 0;
    } else {
        return 1;
    }
}

char* getBinPath(){
    BROWSEINFOW bi = {0}; 
    bi.lpszTitle = L"Diretório onde o arquivo será criado"; 
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; 

    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi); 
    if (pidl != NULL) {
        wchar_t wpath[MAX_PATH];
        if (SHGetPathFromIDListW(pidl, wpath)) { 
            char path[MAX_PATH];
            wcstombs(path, wpath, MAX_PATH);

            char* selectedPath = (char*)malloc(strlen(path) + 1);
            if (selectedPath) {
                strcpy(selectedPath, path);
            }

            CoTaskMemFree(pidl);
            return selectedPath;
        }
        CoTaskMemFree(pidl);
    }
    return NULL;
}

char* getFilePath(){
    OPENFILENAME ofn;      
    char szFile[260];

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Arquivos Estoque\0*.estq\0All Files\0*.*\0"; // Filtro para arquivos .estq
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        char* selectedPath = (char*)malloc(strlen(ofn.lpstrFile) + 1);
        if (selectedPath) {
            strcpy(selectedPath, ofn.lpstrFile);
        }
        return selectedPath;
    } else {
        return NULL;
    }
}

void writePath(FILE* f, const char* path){
    if(f == NULL){
        return;
    }
    
    short pathFlag = -79;
    int pathSize = strlen(path);

    fseek(f, 0, SEEK_END);
    fwrite(path, pathSize, 1, f);
    fwrite(&pathFlag, sizeof(short), 1, f);
    fwrite(&pathSize, sizeof(int), 1, f);
    rewind(f);
}