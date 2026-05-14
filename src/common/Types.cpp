#include "common/Types.h"

namespace NanoDB {

Field* Field::createFromTypeTag(unsigned char typeTag) {
    switch (typeTag) {
        case 0x01:
            return new IntField(0);
        case 0x02:
            return new FloatField(0.0f);
        case 0x03:
            return new StringField("");
        default:
            return nullptr;
    }
}

} // namespace NanoDB
