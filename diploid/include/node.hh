#ifndef _NODE_HH_
#define _NODE_HH_
#include <glob.hh>
class node : public std::enable_shared_from_this<node>{
private:
    std::shared_ptr<node>              _parent;
    std::vector<std::shared_ptr<node>> _children;
    uint32_t                           _references;
    std::vector<uint32_t>              _mutations;

public:
    node(void);
    node(const node&);
    node& operator=(const node&);
    ~node(void);
    void insert(std::shared_ptr<node>&);
    void remove(std::shared_ptr<node>);
    void remove(void);
    void parent(std::shared_ptr<node>&);
    std::shared_ptr<node>& parent(void);
    void increase(void);
    void clear(void);
    uint32_t references(void) const;
    void references(const uint32_t&);
    void mutate(const uint32_t&);
    std::vector<std::shared_ptr<node>>& children(void);
    std::vector<uint32_t>& mutations(void);

    json serialize(void);
};
#endif
