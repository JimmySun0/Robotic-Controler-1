
#pragma once
#include <stdint.h>
#include "base/SharedMemory.hpp"

struct stateData_t
{
	unsigned int        WKC;
    uint8_t             WC_state;
    unsigned int        redundancy_active;

    unsigned int        slaves_responding; /**< Sum of responding slaves on all
                                      Ethernet devices. */
    uint8_t             al_states; /**< Application-layer states of all slaves.
                                  The states are coded in the lower 4 bits.
                                  If a bit is set, it means that at least one
                                  slave in the bus is in the corresponding
                                  state:
                                  - Bit 0: \a INIT
                                  - Bit 1: \a PREOP
                                  - Bit 2: \a SAFEOP
                                  - Bit 3: \a OP */
    uint8_t             link_up; /**< \a true, if at least one Ethernet link is
                                up. */
    uint8_t             isEnable;
};

class StateData : public SharedMemory
{
	private:

	public:

        StateData();
		stateData_t getData() const;
		void writeData(const stateData_t &data) const;
        ~StateData();
};