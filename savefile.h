#ifndef SAVEFILE_H
#define SAVEFILE_H

class SaveFile
{
    struct {
        char identifier[12];
        char version;
        char data_offset;
    } header;
public:
    SaveFile();
};

#endif // SAVEFILE_H
