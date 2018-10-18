#ifndef _EVENT_H_
#define _EVENT_H_
#include <glob.hh>
enum event_t {CREATE=1654067888,INCREMENT=2313715464,SPLIT=396272975,DECREMENT=2641718177,ENDSIM=2660050579};
class event
{
private:
    uint32_t _timestamp;
    event_t  _type;
    json     _params;

public:
    event(void);
    event(const uint32_t &_timestamp,const event_t &_type,const json &_params);
    event(const event &_event);
    event& operator=(const event &_event);
    ~event(void);

    uint32_t timestamp(void);
    void timestamp(const uint32_t &_timestamp);
    event_t type(void);
    void type(const event_t &_type);
    json params(void);
    void params(const json &_params);
};
#endif
