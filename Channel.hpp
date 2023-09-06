#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"


class channel{
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
        channel();
        channel(std::string name, Client& member);
        channel(std::string name, Client& member, std::string key);
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
};


#endif 