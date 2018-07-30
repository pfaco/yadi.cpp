///@file

#ifndef INCLUDE_EMODE_H_
#define INCLUDE_EMODE_H_

#include <yadi/phy_layer.h>

namespace dlms
{
    class emode
    {
        public:
            static bool connect(PhyLayer &phy);
    };
}

#endif /* INCLUDE_EMODE_H_ */
