#ifndef BASE_H
#define BASE_H

#include <iostream>

#include "common.h"

#define DECLARE_MESSAGE_MAP(theClass) \
    public: \
        virtual INT16 DispatchMsg(UINT32 eventNO, UINT8* pucMsg); \ 
        typedef INT16 (theClass::*Head)(UINT8*); \
        struct msgToHand \
        { \
            UINT32 event; \ 
            Head head; \
        }; \
        static const msgToHand m_msgToHand[];

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
        INT16 theClass::DispatchMsg(UINT32 eventNO, UINT8* pucMsg) \
        { \
            msgToHand* ptMsgHand = m_msgToHand; \    
            while (ptMsgHand) \
            { \
                if (eventNO == ptMsgHand->event) \
                { \
                    Head h = ptMsgHand->head; \
                    return (this->*h)(pucMsg); \
                } \
                else \
                { \
                    ptMsgHand++; \
                } \
            } \
            return baseClass::DispatchMsg(eventNO, NULL); \
        } \
        const theClass::msgToHand theClass::m_msgToHand[] = { 
            #define ON_MESSAGE_MAP(evno, head) \
            { evno, head }
            #define END_MESSAGE_MAP \
            { 0, NULL } 
        };

class Base
{
public:
    Base() {};
    ~Base() {}; 
    DECLARE_MESSAGE_MAP(Base)
};








#endif
