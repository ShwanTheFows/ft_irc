#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"


class channel{
    private:
        std::string _name;
        std::string _topic;
        bool havekey;
        std::string _key;
        //bool Ulimit;
        bool isPrivate;
    public:
        bool isempty;
        std::vector<Client *> clients;
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
};


#endif 