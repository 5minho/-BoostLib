//
// Created by 오민호 on 2017. 4. 7..
//

#ifndef BOOSTPROJ_PROTOCOL_H
#define BOOSTPROJ_PROTOCOL_H

#include <iostream>
#include "protocol_decl.h"

//프로토콜 패킷에 사용되는 구조체들
namespace packet {
    //헤더
    struct Header {
        short len;
        char version;
        char flag;
        int reserved;
    };
    //요청 데이터 구조체 정의
    namespace REQ {
        struct Login {
            Header header;
            char id[user::MAX_ID_LEN];
            char pw[user::MAX_PW_LEN];

            void init() {
                header.len = sizeof(Login);
                header.version = protocol::VERSION;
                header.flag = protocol::LOGIN::REQ;
                header.reserved = protocol::RESERVE;
            }
        };
        struct Pos {
            Header header;
            void init() {
                header.len = sizeof(Pos);
                header.version = protocol::VERSION;
                header.flag = protocol::USER_POS::REQ;
                header.reserved = protocol::RESERVE;
            }
        };
        struct Lobby_users {
            Header header;
            void init() {
                header.len = sizeof(Lobby_users);
                header.version = protocol::VERSION;
                header.flag = protocol::LOBBY::USER::REQ::LIST;
                header.reserved = protocol::RESERVE;
            }
        };

        namespace chatroom {
            struct List {
                Header header;
                void init() {
                    header.len = sizeof(List);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::LIST;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Create {
                Header header;
                char title[chatroom::MAX_NAME_LEN];
                void init() {
                    header.len = sizeof(Create);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::CREATE;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Join {
                Header header;
                int room_num;
                void init() {
                    header.len = sizeof(Join);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::JOIN;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Exit {
                Header header;
                void init() {
                    header.len = sizeof(Exit);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::EXIT;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Chat {
                Header header;
                char messageBuf[chatting::MAX_CHAT_LEN];
                void init() {
                    header.len = sizeof(Chat);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::CHAT;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Userlist {
                Header header;
                void init() {
                    header.len = sizeof(Userlist);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::USER_LIST;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Invite {
                Header header;
                char id[user::MAX_ID_LEN];
                void init() {
                    header.len = sizeof(Invite);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::REQ::INVITE;
                    header.reserved = protocol::RESERVE;
                }
            };

        }
    }

    //응답 데이터 구조체 정의
    namespace RES {
        struct Login  {

            Header header;
            char res;

            void init() {
                header.len = sizeof(Login);
                header.version = protocol::VERSION;
                header.flag = protocol::LOGIN::RES;
                header.reserved = protocol::RESERVE;
            }
        };
        struct Pos {
            Header header;
            char res;

            void init() {
                header.len = sizeof(Pos);
                header.version = protocol::VERSION;
                header.flag = protocol::USER_POS::RES;
                header.reserved = protocol::RESERVE;
            }
        };
        struct Lobby_users {
            Header header;
            size_t count;
            char id_list[user::MAX_ID_LEN][user::MAX_COUNT];

            void init() {
                header.len = sizeof(Lobby_users);
                header.version = protocol::VERSION;
                header.flag = protocol::LOBBY::USER::RES::LIST;
                header.reserved = protocol::RESERVE;
            }
        };
        namespace chatroom {
            struct List {
                Header header;
                char res;
                void init() {
                    header.len = sizeof(List);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::LIST;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Create {
                Header header;
                char res;
                void init() {
                    header.len = sizeof(Create);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::CREATE;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Join {
                Header header;
                char res;
                void init() {
                    header.len = sizeof(Join);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::JOIN;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Exit {
                Header header;
                char res;
                void init() {
                    header.len = sizeof(Exit);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::EXIT;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Chat {
                Header header;
                char messageBuf[chatting::MAX_CHAT_LEN];
                void init() {
                    header.len = sizeof(Chat);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::CHAT;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Userlist {
                Header header;
                int usercount;
                char userlist[user::MAX_ID_LEN][chatroom::MAX_USER_IN_ROOM];

                void init() {
                    header.len = sizeof(Userlist);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::USER_LIST;
                    header.reserved = protocol::RESERVE;
                }
            };

            struct Invite {
                Header header;
                char res;
                char fromId[user::MAX_ID_LEN];
                char toId[user::MAX_ID_LEN];

                void init() {
                    header.len = sizeof(Invite);
                    header.version = protocol::VERSION;
                    header.flag = protocol::CHATROOMS::RES::INVITE;
                    header.reserved = protocol::RESERVE;
                }
            };
        }
    }
}

#endif //BOOSTPROJ_PROTOCOL_H