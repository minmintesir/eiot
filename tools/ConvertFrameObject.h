//
// Created by 何继胜 on 2017/4/25.
//

#ifndef OSLA_NEW_CONVERTFRAMEARRAY_H
#define OSLA_NEW_CONVERTFRAMEARRAY_H

#include <object/BaseObject.h>
#include <sslconnection/Frame.h>
#include "log.h"

class ConvertFrameObject {
public:
    template<class T>
    static bool convertFrame2Object(BaseObject *ptr, T &obj) {
        if (nullptr == ptr) {
            throw std::exception();
        }
        Frame *frame = dynamic_cast<Frame *>(ptr);
        const QByteArray &body = frame->getMessageBody();
        try {
            if (false == obj.ParseFromArray(body.data(), body.size())) {
                return false;
            }
        } catch (...) {
            return false;
        }
        return true;
    }

    template<class T>
    static bool convertObject2Frame(BaseObject *ptr, T &obj, int type) {
        if (nullptr == ptr) {
            throw std::exception();
        }
        Frame *frame = dynamic_cast<Frame *>(ptr);
        QByteArray message;
        message.resize(obj.ByteSize());
        obj.SerializeToArray(message.data(), obj.ByteSize());



        return frame->EncodeFrame(type, message, frame->getSequence());
    }
};

#endif //OSLA_NEW_CONVERTFRAMEARRAY_H
