#ifndef FCENGINEEDITOR_COMMANDMANAGER_H__INCLUDE
#define FCENGINEEDITOR_COMMANDMANAGER_H__INCLUDE

class CommandBase;
class CommandManager
{
public:
    CommandManager();
    virtual ~CommandManager();
    
    void DoCommand(CommandBase*);
    void UnDo();
    void ReDo();

private:
    std::vector<CommandBase*> m_HistoryCommandVector;
};



#endif