//
// Copyright (C) OpenSim Ltd
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_CLOCKUSERMODULEMIXINIMPL_H
#define __INET_CLOCKUSERMODULEMIXINIMPL_H

#include "inet/clock/contract/ClockTime.h"
#include "inet/clock/contract/IClock.h"
#include "inet/common/base/ClockUserModuleMixin.h"

namespace inet {

#ifdef WITH_CLOCK_SUPPORT

template<typename T>
IClock *ClockUserModuleMixin<T>::findClockModule() const {
    if (T::hasPar("clockModule")) {
        const char *clockModulePath = T::par("clockModule");
        if (*clockModulePath) {
            cModule *clockModule = T::getModuleByPath(clockModulePath);
            if (clockModule == nullptr)
                throw cRuntimeError("Clock module '%s' not found", clockModulePath);
            return check_and_cast<IClock *>(clockModule);
        }
        else
            return nullptr;
    }
    else
        return nullptr;
}

template<typename T>
ClockUserModuleMixin<T>::~ClockUserModuleMixin() {
#ifndef NDEBUG
    if (clock != nullptr && !usedClockApi)
        std::cerr << "*** Warning: Class '" << className << "' has a clock module set but does not use the clock API (at least in this simulation)\n";
    if (!T::hasPar("clockModule") && usedClockApi)
        std::cerr << "*** Warning: Class '" << className << "' uses the clock API but does not have a clock module parameter\n";
#endif
}

template<typename T>
void ClockUserModuleMixin<T>::initialize(int stage) {
    T::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        clock = findClockModule();
#ifndef NDEBUG
        className = T::getClassName();
#endif
    }
}

template<typename T>
void ClockUserModuleMixin<T>::scheduleClockEventAt(clocktime_t t, ClockEvent *msg) {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock != nullptr)
        clock->scheduleClockEventAt(t, msg);
    else
        T::scheduleAt(t.asSimTime(), msg);
}

template<typename T>
void ClockUserModuleMixin<T>::scheduleClockEventAfter(clocktime_t t, ClockEvent *msg) {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock != nullptr)
        clock->scheduleClockEventAfter(t, msg);
    else
        T::scheduleAfter(t.asSimTime(), msg);
}

template<typename T>
cMessage *ClockUserModuleMixin<T>::cancelClockEvent(ClockEvent *msg) {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock != nullptr)
        return clock->cancelClockEvent(msg);
    else
        return T::cancelEvent(msg);
}

template<typename T>
void ClockUserModuleMixin<T>::cancelAndDeleteClockEvent(ClockEvent *msg) {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock)
        delete clock->cancelClockEvent(msg);
    else
        T::cancelAndDelete(msg);
}

template<typename T>
clocktime_t ClockUserModuleMixin<T>::getClockTime() const {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock != nullptr)
        return clock->getClockTime();
    else
        return ClockTime::from(simTime());
}

template<typename T>
clocktime_t ClockUserModuleMixin<T>::getArrivalClockTime(ClockEvent *msg) const {
#ifndef NDEBUG
    usedClockApi = true;
#endif
    if (clock != nullptr)
        return msg->getArrivalClockTime();
    else
        return ClockTime::from(msg->getArrivalTime());
}

#endif // #ifdef WITH_CLOCK_SUPPORT

} // namespace inet

#endif // ifndef __INET_CLOCKUSERMODULEMIXINIMPL_H

