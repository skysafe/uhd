//
// Copyright 2010-2013 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <uhd/property_tree.hpp>
#include <uhd/usrp_clock/multi_usrp_clock.hpp>
#include <uhd/usrp_clock/octoclock_eeprom.hpp>


#include <uhd/exception.hpp>
#include <uhd/utils/log.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

using namespace uhd;
using namespace uhd::usrp_clock;

/***********************************************************************
 * Multi USRP Clock implementation
 **********************************************************************/
class multi_usrp_clock_impl : public multi_usrp_clock{
public:
    multi_usrp_clock_impl(const device_addr_t &addr){
        _dev = device::make(addr, device::CLOCK);
        _tree = _dev->get_tree();
    }

    device::sptr get_device(void){
        return _dev;
    }

    std::string get_pp_string(void){
        std::string buff = str(boost::format("%s USRP Clock Device\n")
            % ((get_num_boards() > 1) ? "Multi" : "Single")
        );
        for(size_t i = 0; i < get_num_boards(); i++){
            buff += str(boost::format("  Board %s\n") % i);
            buff += str(boost::format("    Reference: %s\n")
                        % (get_sensor("using_ref", i).value)
                    );
        }

        return buff;
    }

    size_t get_num_boards(void){
        return _tree->list("/mboards").size();
    }

    uint32_t get_time(size_t board){
        std::string board_str = str(boost::format("/mboards/%d") % board);

        return _tree->access<uint32_t>(board_str / "time").get();
    }

    sensor_value_t get_sensor(const std::string &name, size_t board){
        std::string board_str = str(boost::format("/mboards/%d") % board);

        return _tree->access<sensor_value_t>(board_str / "sensors" / name).get();
    }

    std::vector<std::string> get_sensor_names(size_t board){
        std::string board_str = str(boost::format("/mboards/%d") % board);

        return _tree->list(board_str / "sensors");
    }

private:
    device::sptr _dev;
    property_tree::sptr _tree;
};

multi_usrp_clock::~multi_usrp_clock(void){
    /* NOP */
}

/***********************************************************************
 * Multi USRP Clock factory function
 **********************************************************************/
multi_usrp_clock::sptr multi_usrp_clock::make(const device_addr_t &dev_addr){
    UHD_LOGGER_DEBUG("OCTOCLOCK") << "multi_usrp_clock::make with args " << dev_addr.to_pp_string() ;
    return sptr(new multi_usrp_clock_impl(dev_addr));
}
