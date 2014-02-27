#ifndef BEATS_UTILITY_MAILMANAGER_MAILMANAGER_H__INCLUDE
#define BEATS_UTILITY_MAILMANAGER_MAILMANAGER_H__INCLUDE

#include <vector>
#include <string.h>
#include <assert.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")

/* Usage sample

CMailManager* pMailMangaer = CMailManager::GetInstance();
pMailMangaer->SetSubject("测试邮件abc");
pMailMangaer->SetSenderName("卢尼玛");
pMailMangaer->SetSenderMail("xxxxxx@163.com");
pMailMangaer->SetReplyTo("xxxxx@163.com");
pMailMangaer->SetXMailer("Microsoft Outlook Express 5.50.4807.1700");
pMailMangaer->SetLogin("xxxxxx");
pMailMangaer->SetPassword("xxxxxxxx");
pMailMangaer->SetXPriority(XPRIORITY_HIGH);
pMailMangaer->SetSMTPServer("smtp.163.com");

pMailMangaer->AddRecipient("xxxx@qq.com", "扣扣卢尼玛");
pMailMangaer->AddBCCRecipient("xxxx@qq.com", "暗影卢尼玛");
pMailMangaer->AddCCRecipient("xxxx@qq.com", "CC卢尼玛");
pMailMangaer->AddAttachment("C:\\xxxx");
pMailMangaer->AddAttachment("C:\\xxxxx");
pMailMangaer->AddAttachment("C:\\xxxxx");

pMailMangaer->AddMsgLine("你好:");
pMailMangaer->AddMsgLine("第一个附件是这个");
pMailMangaer->AddMsgLine("第二个附件是那个");
pMailMangaer->AddMsgLine("第三个附件自己看");
pMailMangaer->AddMsgLine("完毕");

pMailMangaer->Send();
*/

#define TIME_IN_SEC		10		// how long client will wait for server response in non-blocking mode
#define BUFFER_SIZE 10240	  // SendData and RecvData buffers sizes
#define MSG_SIZE_IN_MB 5		// the maximum size of the message with all attachments
#define COUNTER_VALUE	100		// how many times program will try to receive data

const char BOUNDARY_TEXT[] = "__MESSAGE__ID__54yg6f6h6y456345";

enum CSmptXPriority
{
    XPRIORITY_HIGH = 2,
    XPRIORITY_NORMAL = 3,
    XPRIORITY_LOW = 4
};

class ECSmtp
{
public:
    enum CSmtpError
    {
        CSMTP_NO_ERROR = 0,
        WSA_STARTUP = 100, // WSAGetLastError()
        WSA_VER,
        WSA_SEND,
        WSA_RECV,
        WSA_CONNECT,
        WSA_GETHOSTBY_NAME_ADDR,
        WSA_INVALID_SOCKET,
        WSA_HOSTNAME,
        WSA_IOCTLSOCKET,
        WSA_SELECT,
        BAD_IPV4_ADDR,
        UNDEF_MSG_HEADER = 200,
        UNDEF_MAIL_FROM,
        UNDEF_SUBJECT,
        UNDEF_RECIPIENTS,
        UNDEF_LOGIN,
        UNDEF_PASSWORD,
        UNDEF_RECIPIENT_MAIL,
        COMMAND_MAIL_FROM = 300,
        COMMAND_EHLO,
        COMMAND_AUTH_LOGIN,
        COMMAND_DATA,
        COMMAND_QUIT,
        COMMAND_RCPT_TO,
        MSG_BODY_ERROR,
        CONNECTION_CLOSED = 400, // by server
        SERVER_NOT_READY, // remote server
        SERVER_NOT_RESPONDING,
        SELECT_TIMEOUT,
        FILE_NOT_EXIST,
        MSG_TOO_BIG,
        BAD_LOGIN_PASS,
        UNDEF_XYZ_RESPONSE,
        LACK_OF_MEMORY,
        TIME_ERROR,
        RECVBUF_IS_EMPTY,
        SENDBUF_IS_EMPTY,
        OUT_OF_MSG_RANGE,
    };
    ECSmtp(CSmtpError err_) : ErrorCode(err_) {}
    CSmtpError GetErrorNum(void) const {return ErrorCode;}
    std::string GetErrorText(void) const;

private:
    CSmtpError ErrorCode;
};

class CMailManager
{
    BEATS_DECLARE_SINGLETON(CMailManager)
public:
    void AddRecipient(const char *email, const char *name=NULL);
    void AddBCCRecipient(const char *email, const char *name=NULL);
    void AddCCRecipient(const char *email, const char *name=NULL);
    void AddAttachment(const char *path);   
    void AddMsgLine(const char* text);
    void DelRecipients(void);
    void DelBCCRecipients(void);
    void DelCCRecipients(void);
    void DelAttachments(void);
    void DelMsgLines(void);
    void DelMsgLine(unsigned int line);
    void ModMsgLine(unsigned int line,const char* text);
    size_t GetBCCRecipientCount() const;    
    size_t GetCCRecipientCount() const;
    size_t GetRecipientCount() const;    
    const char* GetLocalHostIP() const;
    const char* GetLocalHostName() const;
    const char* GetMsgLineText(size_t line) const;
    size_t GetMsgLines(void) const;
    const char* GetReplyTo() const;
    const char* GetMailFrom() const;
    const char* GetSenderName() const;
    const char* GetSubject() const;
    const char* GetXMailer() const;
    CSmptXPriority GetXPriority() const;

    // Do all the setting before call Send().
    void Send();
    void SetSubject(const char*);
    void SetSenderName(const char*);
    void SetSenderMail(const char*);
    void SetReplyTo(const char*);
    void SetXMailer(const char*);
    void SetLogin(const char*);
    void SetPassword(const char*);
    void SetXPriority(CSmptXPriority);
    void SetSMTPServer(const char* server,const unsigned short port=0);

private:	
    std::string m_sLocalHostName;
    std::string m_sMailFrom;
    std::string m_sNameFrom;
    std::string m_sSubject;
    std::string m_sXMailer;
    std::string m_sReplyTo;
    std::string m_sIPAddr;
    std::string m_sLogin;
    std::string m_sPassword;
    std::string m_sSMTPSrvName;
    unsigned short m_iSMTPSrvPort;
    CSmptXPriority m_iXPriority;
    char *SendBuf;
    char *RecvBuf;

    SOCKET hSocket;

    struct Recipient
    {
        std::string Name;
        std::string Mail;
    };

    std::vector<Recipient> Recipients;
    std::vector<Recipient> CCRecipients;
    std::vector<Recipient> BCCRecipients;
    std::vector<std::string> Attachments;
    std::vector<std::string> MsgBody;

    void ReceiveData();
    void SendData();
    void FormatHeader(char*);
    int SmtpXYZdigits();
    SOCKET ConnectRemoteServer(const char* server, const unsigned short port=0);
};


#endif