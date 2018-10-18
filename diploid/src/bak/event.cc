#include <event.hh>
namespace diploid{
event::event(void)
{
    ;
}
event::event(const uint32_t &_timestamp,const event_t &_type,const json &_params)
{
    this->_timestamp=_timestamp;
    this->_type=_type;
    this->_params=_params;
}
event::event(const event &_event)
{
    this->_timestamp=_event._timestamp;
    this->_type=_event._type;
    this->_params=_event._params;
}
event& event::operator=(const event &_event)
{
    this->_timestamp=_event._timestamp;
    this->_type=_event._type;
    this->_params=_event._params;
    return(*this);
}
event::~event(void)
{

}
uint32_t event::timestamp(void)
{
    return(this->_timestamp);
}
void event::timestamp(const uint32_t &_timestamp)
{
    this->_timestamp=_timestamp;
}
event_t event::type(void)
{
    return(this->_type);
}
void event::type(const event_t &_type)
{
    this->_type=_type;
}
json event::params(void)
{
    return(this->_params);
}
void event::params(const json &_params)
{
    this->_params=_params;
}
};
