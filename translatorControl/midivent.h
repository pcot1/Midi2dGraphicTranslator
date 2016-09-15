#ifndef MIDIVENT_H
#define MIDIVENT_H

typedef enum midiventTypeName { None, NoteOn, NoteOff } MidiventTypeName;

class Midivent
{
public:
    Midivent();
    Midivent(MidiventTypeName theType, int theNote = 0);
    ~Midivent();
    MidiventTypeName getType(void);
    int getNote(void);
private:
    MidiventTypeName    typeName;
    unsigned char       noteId;
};


#endif // MIDIVENT_H
