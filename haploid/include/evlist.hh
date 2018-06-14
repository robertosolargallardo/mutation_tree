#ifndef _EVLIST_H_
#define _EVLIST_H_
#include <event.hh>
#include <util.hh>
#include <glob.hh>

class evlist {
  private:
    uint32_t _lvt;
    class comparator : public std::binary_function<std::shared_ptr<event>,std::shared_ptr<event>,bool> {
      public:
        bool operator()(const std::shared_ptr<event> &lhs,const std::shared_ptr<event> &rhs) const {
            return(lhs->timestamp()>rhs->timestamp());
        }
    };
    std::priority_queue<std::shared_ptr<event>,std::vector<std::shared_ptr<event>>,comparator> _list;

  public:
    evlist(void);
    evlist(const json &_scenario);
    evlist(const evlist &_evlist);
    evlist& operator=(const evlist &_evlist);

    std::shared_ptr<event> top(void);
    void 	 pop(void);
    bool 	 empty(void);
    uint32_t lvt(void);

    ~evlist(void);
};
#endif
