//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef __INET_IIEEE80211ERRORMODEL_H
#define __INET_IIEEE80211ERRORMODEL_H

#include "inet/physicallayer/common/ModulationType.h"

namespace inet {

namespace physicallayer {

class IIeee80211ErrorModel
{
  public:
    IIeee80211ErrorModel() {};
    virtual ~IIeee80211ErrorModel() {};
    virtual double GetChunkSuccessRate(ModulationType mode, double snr, uint32_t nbits) const = 0;
};

} // namespace physicallayer

} // namespace inet

#endif /* IIEEE80211ERRORMODEL_H_ */

