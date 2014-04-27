#ifndef FCENGINEEDITOR_COMMANDBASE_H__INCLUDE
#define FCENGINEEDITOR_COMMANDBASE_H__INCLUDE

class CommandBase
{
public:
    CommandBase();
    virtual ~CommandBase();

    virtual void Do() = 0;
    virtual void UnDo() = 0;
private:

};

#endif
