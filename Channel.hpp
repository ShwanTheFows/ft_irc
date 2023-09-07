#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"


class Channel{
    private:
        std::string _name;
        std::string _topic;
        bool havekey;
        std::string _key;
        int userLimit;
        bool Ulimit;
        bool isPrivate;
        bool haveTopic;
    public:
        bool isempty;
        std::vector<Client *> clients;
        std::vector<std::string> operators;
        std::vector<std::string> inviteList;
        
        Channel();
        Channel(std::string name, Client& member);
        Channel(std::string name, Client& member, std::string key);
        Channel(const Channel& other);
        ~Channel();
        Channel& operator=(const Channel& other);

        std::string getTopic(void);
        void setTopic(std::string subj);
        std::string getchannelName(void);
        bool addMember(Client& member);
        void setKey(std::string key);
        std::string getKey(void);
        bool sethaveKey();
        void haveKey(bool keyVal);
        bool getPrivate();
        void setPrivate(bool prv);
        std::string getClientNames(void);
        void removeMember(std::string clientName);
        int getLimit();
        void setLimit(int lim);
        void boolTopic(bool settop);
        bool hasTopic();
        void haveLimit(bool change);
        void removeOp(std::string name);
        void setOperators(std::string newOperators);
        bool isOp(std::string name);
        std::string getClientsSize(void);
        void addToInviteList(std::string clientName);
        void removeFromInviteList(std::string clientName);
        bool isInInviteList(std::string clientName);
        void changeNickname(std::string oldNick, std::string newNick);
};


#endif 